#include "../include/Adapters/AlpacaAdapter.h"
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <thread>
#include <boost/asio/ssl/context.hpp>
#include <nlohmann/json.hpp>
#include "../include/MarketDataGateway/MarketDataParser.h"

using namespace std;

enum class AlpacaMsgType {
    SUCCESS,
    SUBSCRIPTION,
    Q,
    B,
    UNKNOWN // fallback
};

AlpacaMsgType parseAlpacaMsgType(const std::string &typeStr) {
    if (typeStr == "success")       return AlpacaMsgType::SUCCESS;
    if (typeStr == "subscription")  return AlpacaMsgType::SUBSCRIPTION;
    if (typeStr == "q")             return AlpacaMsgType::Q;
    if (typeStr == "b")             return AlpacaMsgType::B;
    // add more if needed, e.g. "t"
    return AlpacaMsgType::UNKNOWN;
}

////////////////////////////////////////
// Constructor / Destructor
////////////////////////////////////////
AlpacaAdapter::AlpacaAdapter(const std::string& configFile) {
    loadConfig(configFile);
}

AlpacaAdapter::~AlpacaAdapter() {
    gracefulDisconnect();  // Ensure graceful shutdown on object destruction
}

////////////////////////////////////////
// Cleanup
////////////////////////////////////////
void AlpacaAdapter::gracefulDisconnect() {
    try {
        std::lock_guard<std::mutex> guard(mtx);
        if (con && con->get_state() == websocketpp::session::state::open) {
            c.close(con->get_handle(), websocketpp::close::status::going_away, "Client disconnect");
        }
        std::cout << "WebSocket connection closed gracefully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error during graceful disconnect: " << e.what() << std::endl;
    }
}

////////////////////////////////////////
// Initialization
////////////////////////////////////////
void AlpacaAdapter::initialize(const std::string& configFile) {
    // If needed, re-load config or do other setup
    // loadConfig(configFile);
    std::cout << "AlpacaAdapter initialized successfully (no-op)." << std::endl;
}

void AlpacaAdapter::loadConfig(const std::string &configFile) {
    try {
        std::ifstream file(configFile);
        if (file.is_open()) {
            nlohmann::json config;
            file >> config;

            // Load Alpaca config from the nested structure
            apiKey = config["marketAdapter"]["alpacaAdapter"]["api_key"];
            secretKey = config["marketAdapter"]["alpacaAdapter"]["api_secret"];
            baseUrl = config["marketAdapter"]["alpacaAdapter"]["base_url"];
            dataUrl = config["marketAdapter"]["alpacaAdapter"]["data_url"];
            mode = config["marketAdapter"]["alpacaAdapter"]["mode"];
            mode_options = config["marketAdapter"]["alpacaAdapter"]["mode_options"];

            std::cout << "Alpaca config loaded successfully." << std::endl;
            std::cout << "Available modes: " + mode_options << std::endl;
            std::cout << "Change ../config/config.json for mode. Current mode: " + mode << std::endl;
            std::cout << "" << std::endl;
        } else {
            throw std::runtime_error("Unable to open config file.");
        }
    } catch (const std::exception &e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
    }
}

////////////////////////////////////////
// Live Data Subscription (FIXED)
////////////////////////////////////////
void AlpacaAdapter::subscribeLiveData(const std::vector<std::string>& tickers, bool testMode) {
    // Store the tickers to use them later for subscription
    subscribedTickers = tickers;

    // Suppress verbose logs
    c.get_alog().clear_channels(websocketpp::log::alevel::all);

    // Choose correct URL for test or live
    std::string url = testMode
        ? "wss://stream.data.alpaca.markets/v2/test"
        : "wss://stream.data.alpaca.markets/v2/stocks";

    // Initialize WebSocket
    try {
        initializeWebSocketConnection(c, url);

        // Set up event handlers
        c.set_open_handler(std::bind(&AlpacaAdapter::on_open, this, &c, std::placeholders::_1));
        c.set_message_handler(std::bind(&AlpacaAdapter::on_message, this, &c, std::placeholders::_1, std::placeholders::_2));
        c.set_close_handler(std::bind(&AlpacaAdapter::on_close, this, &c, std::placeholders::_1));
        c.set_fail_handler(std::bind(&AlpacaAdapter::on_fail, this, &c, std::placeholders::_1));

        // Create connection
        websocketpp::lib::error_code ec;
        con = c.get_connection(url, ec);
        if (ec) {
            std::cerr << "[subscribeLiveData] Connection error: " << ec.message() << std::endl;
            return;
        }

        // Connect & run in a separate thread, so we don't block
        c.connect(con);

        // Start the ASIO loop in a background thread
        std::thread websocket_thread([&]() {
            try {
                // This run call will block until the connection is closed
                c.run();
            } catch (const std::exception &e) {
                std::cerr << "[AlpacaAdapter] WebSocket thread error: " << e.what() << std::endl;
            }
        });

        // Detach the thread, so your main code continues
        websocket_thread.detach();

    } catch (const websocketpp::exception& e) {
        std::cerr << "[subscribeLiveData] WebSocket exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "[subscribeLiveData] General exception: " << e.what() << std::endl;
    }
}

////////////////////////////////////////
// WebSocket Event Handlers
////////////////////////////////////////
void AlpacaAdapter::on_open(websocket_client* c, connection_hdl hdl) {
    std::cout << "[AlpacaAdapter] WebSocket connected! Sending auth..." << std::endl;
    send_auth(c, hdl);

    // Option A: Immediately subscribe after auth request:
    // (In production, you might wait for a success message from the server before subscribing.)
    std::cout << "[AlpacaAdapter] Now sending subscription for tickers" << std::endl;
    send_subscription(c, hdl);
}

void AlpacaAdapter::on_message(websocket_client* c,
                               websocketpp::connection_hdl hdl,
                               websocket_client::message_ptr msg) {
    std::lock_guard<std::mutex> guard(mtx);
    std::string payload = msg->get_payload();
    nlohmann::json arr = nlohmann::json::parse(payload);

    if (!arr.is_array()) return; // Alpaca sends an array of events

    for (auto &entry : arr) {
        if (!entry.contains("T")) continue;

        std::string rawType = entry["T"].get<std::string>();
        AlpacaMsgType msgType = parseAlpacaMsgType(rawType);

        switch(msgType) {
            case AlpacaMsgType::SUCCESS:
            {
                // For "T":"success" or "T":"authenticated" messages,
                // you might build a logging DTO or just log it.
                // Example:
                std::string msgText = entry.value("msg", "");
                // Publish or log as needed
                break;
            }

            case AlpacaMsgType::SUBSCRIPTION:
            {
                // "T":"subscription" => subscription confirmation
                // e.g. build a logging dto
                std::string msgText = entry.value("msg", "");
                break;
            }

            case AlpacaMsgType::Q:
            {
                // "T":"q" => quotes
                MarketDataDTO dto;
                dto.symbol    = entry.value("S", "");
                dto.price     = entry.value("ap", 0.0); // askPrice
                dto.size      = entry.value("as", 0.0); // askSize
                dto.feed      = "Alpaca";
                // etc. for bp, bs if you like
                // publish or handle dto
                break;
            }

            case AlpacaMsgType::B:
            {
                // "T":"b" => bars
                MarketDataDTO dto;
                dto.symbol    = entry.value("S", "");
                dto.price     = entry.value("c", 0.0);  // close price
                dto.size      = entry.value("v", 0.0);  // volume
                dto.feed      = "AlpacaBars";
                // publish or handle dto
                break;
            }

            default:
            {
                // Unknown or unhandled
                break;
            }
        } // end switch
    }
}

void AlpacaAdapter::on_close(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::cout << "[AlpacaAdapter] Connection closed!" << std::endl;
}

void AlpacaAdapter::on_fail(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::cout << "[AlpacaAdapter] Connection failed!" << std::endl;
}

////////////////////////////////////////
// Helper: Build WebSocket Connection
////////////////////////////////////////
void AlpacaAdapter::initializeWebSocketConnection(websocket_client &client, const std::string &url) {
    client.init_asio();

    // TLS setup if needed
    client.set_tls_init_handler([](websocketpp::connection_hdl) {
        auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
        ctx->set_default_verify_paths();
        return ctx;
    });
}

////////////////////////////////////////
// Send Auth
////////////////////////////////////////
void AlpacaAdapter::send_auth(websocket_client* c, connection_hdl hdl) {
    // Build JSON or do the old string approach
    nlohmann::json auth_msg = {
        {"action", "auth"},
        {"key",    apiKey},
        {"secret", secretKey}
    };
    sendMessage(c, hdl, auth_msg);
    std::cout << "> Auth message: " << auth_msg.dump() << std::endl;
}

////////////////////////////////////////
// Send Subscription (Only Once!)
////////////////////////////////////////
void AlpacaAdapter::send_subscription(websocket_client* c, connection_hdl hdl) {
    // Build a single subscription message using the vector of tickers
    // std::string sub_msg = buildSubscriptionMessage(subscribedTickers);
    // Or if you really want to test "FAKEPACA" only, replace with your own logic
    std::string sub_msg = R"({"action":"subscribe","bars":["FAKEPACA"],"quotes":["FAKEPACA"]})";

    nlohmann::json j = nlohmann::json::parse(sub_msg);
    sendMessage(c, hdl, j);

    std::cout << "> Subscription message: " << sub_msg << std::endl;
}

////////////////////////////////////////
// Low-level WebSocket Send
////////////////////////////////////////
void AlpacaAdapter::sendMessage(websocket_client* c, connection_hdl hdl, const nlohmann::json &message) {
    try {
        std::string payload = message.dump();
        c->send(hdl, payload, websocketpp::frame::opcode::text);
    } catch (const std::exception &e) {
        std::cerr << "[AlpacaAdapter] Error sending WebSocket message: " << e.what() << std::endl;
    }
}

////////////////////////////////////////
// Build the Auth/Subscription Messages
////////////////////////////////////////
std::string AlpacaAdapter::buildAuthMessage() {
    nlohmann::json auth_message = {
        {"action", "auth"},
        {"key",    apiKey},
        {"secret", secretKey}
    };
    return auth_message.dump();
}

std::string AlpacaAdapter::buildSubscriptionMessage(const std::vector<std::string>& tickers) {
    // Example: subscribe to bars & quotes for each ticker
    nlohmann::json subscribe_message = {
        {"action", "subscribe"},
        {"bars",   tickers},
        {"quotes", tickers}
        // If you want trades too:  {"trades", tickers}
    };
    return subscribe_message.dump();
}

////////////////////////////////////////
// HTTP GET for Single Tick
////////////////////////////////////////
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string AlpacaAdapter::performRequest(const std::string &url) {
    try {
        CURL* hnd = curl_easy_init();
        if (!hnd) {
            throw std::runtime_error("CURL initialization failed");
        }

        std::string readBuffer;
        curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "accept: application/json");
        std::string apiKeyHeader    = "APCA-API-KEY-ID: " + apiKey;
        std::string apiSecretHeader = "APCA-API-SECRET-KEY: " + secretKey;
        headers = curl_slist_append(headers, apiKeyHeader.c_str());
        headers = curl_slist_append(headers, apiSecretHeader.c_str());
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(hnd);
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);

        return readBuffer;
    } catch (const std::exception &e) {
        std::cerr << "[AlpacaAdapter] Error performing request: " << e.what() << std::endl;
        return "";
    }
}

////////////////////////////////////////
// getLatestTick
////////////////////////////////////////
std::string AlpacaAdapter::getLatestTick(const std::string &symbol, const std::string &feed) {
    try {
        std::string url = dataUrl + "v2/stocks/quotes/latest?symbols=" + symbol + "&feed=" + feed;
        std::cout << "Fetching market data for " << symbol << " from: " << url << std::endl;
        return MarketDataParser::parseQuoteData(performRequest(url));
    } catch (const std::exception &e) {
        std::cerr << "[AlpacaAdapter] Error fetching latest tick: " << e.what() << std::endl;
        return "";
    }
}

////////////////////////////////////////
// createOrder (HTTP POST or Just JSON)
////////////////////////////////////////
std::string AlpacaAdapter::createOrder(const Order& order) {
    // Build JSON for the order
    nlohmann::json body = {
        {"symbol",         order.getInstrument()},
        {"qty",            std::to_string(order.getQuantity())},
        {"side",           order.getSide()},
        {"type",           order.getOrderType()},
        {"time_in_force",  order.getTimeInForce()}
    };
    // ... handle limit_price, stop_price, trailing, extended_hours, etc. ...
    // Return the JSON for now:
    std::cout << "[AlpacaAdapter] createOrder JSON: " << body.dump(4) << std::endl;
    return body.dump();
}
