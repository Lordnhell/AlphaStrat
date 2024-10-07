#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <string>
#include <boost/asio/ssl/context.hpp>
#include <thread>

#include "../include/Adapters/AlpacaAdapter.h"
#include "../include/MarketDataGateway/MarketDataParser.h"

using namespace std;

void AlpacaAdapter::initialize(const std::string& configFile) {
    try {
        loadConfig(configFile);
        std::cout << "AlpacaAdapter initialized successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error initializing AlpacaAdapter: " << e.what() << std::endl;
    }
}

string AlpacaAdapter::getLatestTick(const std::string &symbol, const std::string &feed) {
    try {
        std::string url = dataUrl + "v2/stocks/quotes/latest?symbols=" + symbol + "&feed=" + feed;
        std::cout << "Fetching market data for " << symbol << " from: " << url << std::endl;
        return MarketDataParser::parseQuoteData(performRequest(url));
    } catch (const std::exception &e) {
        std::cerr << "Error fetching latest tick: " << e.what() << std::endl;
        return "";
    }
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void AlpacaAdapter::subscribeLiveData(const std::vector<std::string>& tickers, bool testMode) {
    websocket_client c;
    c.get_alog().clear_channels(websocketpp::log::alevel::frame_header |
        websocketpp::log::alevel::frame_payload |
        websocketpp::log::alevel::control);

    std::string url = testMode ? "wss://stream.data.alpaca.markets/v2/test" : "wss://stream.data.alpaca.markets/v2/stocks";

    // Initialize WebSocket connection
    initializeWebSocketConnection(c, url);

    try {
        websocketpp::lib::error_code ec;
        websocket_client::connection_ptr con = c.get_connection(url, ec);

        if (ec) {
            std::cerr << "Failed to initialize connection: " << ec.message() << std::endl;
            return;
        }

        // Set message handler to handle incoming messages (only once)
        c.set_message_handler(std::bind(&AlpacaAdapter::on_message, this, &c, std::placeholders::_1, std::placeholders::_2));

        // Connect and start the WebSocket event loop
        c.connect(con);

        // WebSocket run in a separate thread
        std::thread websocket_thread([&c]() {
            try {
                c.run();
            } catch (const std::exception &e) {
                std::cerr << "[THREAD ERROR] WebSocket thread error: " << e.what() << std::endl;
            }
        });

        // Short delay to ensure the connection is established
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Build and send the authentication message
        std::cout << "> Sending authentication message..." << std::endl;
        std::string auth_message = buildAuthMessage();
        std::cout << "Auth message: " << auth_message << std::endl; // Log the actual message

        if (con->get_state() == websocketpp::session::state::value::open) {
            c.send(con->get_handle(), auth_message, websocketpp::frame::opcode::text);
            std::cout << "Auth message sent" << std::endl;
        } else {
            std::cerr << "WebSocket connection not open. Cannot send authentication message." << std::endl;
        }

        websocket_thread.join();

    } catch (const websocketpp::exception& e) {
        std::cerr << "WebSocket exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "General exception: " << e.what() << std::endl;
    }
}

void AlpacaAdapter::loadConfig(const std::string &configFile) {
    try {
        std::ifstream file(configFile);
        if (file.is_open()) {
            nlohmann::json config;
            file >> config;
            apiKey = config["api_key"];
            secretKey = config["api_secret"];
            baseUrl = config["base_url"];
            dataUrl = config["data_url"];
            std::cout << "Config loaded successfully." << std::endl;
        } else {
            throw std::runtime_error("Unable to open config file.");
        }
    } catch (const std::exception &e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
    }
}

string AlpacaAdapter::performRequest(const std::string &url) {
    try {
        CURL* hnd = curl_easy_init();
        if (!hnd) {
            throw std::runtime_error("CURL initialization failed");
        }

        std::string readBuffer;
        curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");
        std::string apiKeyHeader = "APCA-API-KEY-ID: " + apiKey;
        headers = curl_slist_append(headers, apiKeyHeader.c_str());
        std::string apiSecretHeader = "APCA-API-SECRET-KEY: " + secretKey;
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
        std::cerr << "Error performing request: " << e.what() << std::endl;
        return "";
    }
}

void AlpacaAdapter::on_message(websocket_client* c, connection_hdl hdl, websocket_client::message_ptr msg) {
    try {
        std::string payload = msg->get_payload();
        auto json_msg = nlohmann::json::parse(payload);

        std::cout << "< Received message: " << payload << std::endl;  // Log the full payload

        if (json_msg.contains("T")) {
            std::string messageType = json_msg["T"];

            if (messageType == "success" && json_msg["msg"] == "authenticated") {
                std::cout << "[INFO] WebSocket connection authenticated." << std::endl;
                isAuthenticated = true;

                // Send subscription message
                std::string subscription_message = buildSubscriptionMessage({"FAKEPACA"});
                std::cout << "> Subscribing to FAKEPACA in test mode." << std::endl;
                c->send(hdl, subscription_message, websocketpp::frame::opcode::text);
                std::cout << "Subscription message sent" << std::endl;

            } else if (messageType == "subscription") {
                std::cout << "[INFO] Subscription Update: " << json_msg.dump(4) << std::endl;
            } else if (messageType == "error") {
                std::cout << "[ERROR] Error Message: " << json_msg.dump(4) << std::endl;
            } else if (messageType == "q") {
                std::cout << "[QUOTE] Quote Update for Symbol: " << json_msg["S"]
                          << " | Bid: " << json_msg["bp"] << " | Ask: " << json_msg["ap"]
                          << " | Timestamp: " << json_msg["t"] << std::endl;
            } else if (messageType == "t") {
                std::cout << "[TRADE] Trade Update for Symbol: " << json_msg["S"]
                          << " | Price: " << json_msg["p"] << " | Size: " << json_msg["s"]
                          << " | Timestamp: " << json_msg["t"] << std::endl;
            } else {
                std::cout << "[UNKNOWN] Unlabeled message: " << json_msg.dump(4) << std::endl;
            }
        } else {
            std::cerr << "[WARNING] Received a message without a type field: " << json_msg.dump(4) << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "[EXCEPTION] Error processing WebSocket message: " << e.what() << std::endl;
    }
}

void AlpacaAdapter::sendMessage(websocket_client* c, connection_hdl hdl, const nlohmann::json& message) {
    try {
        std::string payload = message.dump();
        c->send(hdl, payload, websocketpp::frame::opcode::text);
    } catch (const std::exception &e) {
        std::cerr << "Error sending WebSocket message: " << e.what() << std::endl;
    }
}

std::string AlpacaAdapter::buildAuthMessage() {
    nlohmann::json auth_message = {
        {"action", "auth"},
        {"key", apiKey},
        {"secret", secretKey}
    };
    return auth_message.dump();
}

std::string AlpacaAdapter::buildSubscriptionMessage(const std::vector<std::string>& tickers) {
    nlohmann::json subscribe_message = {
        {"action", "subscribe"},
        {"bars", tickers},  // Subscribe to bars
        {"quotes", tickers}   // Optionally subscribe to quotes too
    };
    return subscribe_message.dump();
}

void AlpacaAdapter::initializeWebSocketConnection(websocket_client& c, const std::string& url) {
    try {
        c.init_asio();

        // Set TLS initialization callback
        c.set_tls_init_handler([](websocketpp::connection_hdl) -> websocketpp::lib::shared_ptr<boost::asio::ssl::context> {
            try {
                auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
                ctx->set_default_verify_paths();
                return ctx;
            } catch (const std::exception &e) {
                std::cerr << "TLS initialization error: " << e.what() << std::endl;
                return nullptr;  // Return null on error
            }
        });

    } catch (const std::exception &e) {
        std::cerr << "Error initializing WebSocket connection: " << e.what() << std::endl;
    }
}
