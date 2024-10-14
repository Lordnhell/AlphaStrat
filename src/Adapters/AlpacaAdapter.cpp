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

AlpacaAdapter::AlpacaAdapter(const std::string& configFile) {
    try {
        loadConfig(configFile);
        std::cout << "AlpacaAdapter initialized successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error initializing AlpacaAdapter: " << e.what() << std::endl;
    }
}

AlpacaAdapter::~AlpacaAdapter() {
    gracefulDisconnect();  // Ensure graceful shutdown on object destruction
}

void AlpacaAdapter::gracefulDisconnect() {
    try {
        std::lock_guard<std::mutex> guard(mtx);
        if (con->get_state() == websocketpp::session::state::open) {
            c.close(con->get_handle(), websocketpp::close::status::going_away, "Client disconnect");
        }
        std::cout << "WebSocket connection closed gracefully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error during graceful disconnect: " << e.what() << std::endl;
    }
}

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
    // websocket_client c;
    c.get_alog().clear_channels(websocketpp::log::alevel::frame_header |
        websocketpp::log::alevel::frame_payload |
        websocketpp::log::alevel::control);

    std::string url = testMode ? "wss://stream.data.alpaca.markets/v2/test" : "wss://stream.data.alpaca.markets/v2/stocks";

    // Initialize WebSocket connection


    try {
        // websocketpp::lib::error_code ec;
        // websocket_client::connection_ptr con = c.get_connection(url, ec);
        try {
            initializeWebSocketConnection(c, url);
        } catch (std::exception &e)
        {
            std::cerr << "Error initializing websocket connection: " << e.what() << std::endl;
        }

        // Set message handler to handle incoming messages (only once)
        c.set_open_handler(std::bind(&AlpacaAdapter::on_open, this, &c, std::placeholders::_1));
        c.set_message_handler(std::bind(&AlpacaAdapter::on_message, this, &c, std::placeholders::_1, std::placeholders::_2));
        c.set_close_handler(std::bind(&AlpacaAdapter::on_close, this, &c, std::placeholders::_1));
        c.set_fail_handler(std::bind(&AlpacaAdapter::on_fail, this, &c, std::placeholders::_1));

        websocketpp::lib::error_code ec;
        websocket_client::connection_ptr con = c.get_connection(url, ec);

        if (ec) {
            std::cout << "Connection error: " << ec.message() << std::endl;
        }

        c.connect(con);

        // WebSocket run in a separate thread
        std::thread websocket_thread([this]() {
            try {
                c.run();
            } catch (const std::exception &e) {
                std::cerr << "[THREAD ERROR] WebSocket thread error: " << e.what() << std::endl;
            }
        });

        // Main thread waits for WebSocket messages and handles subscription
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Sending subscription message
            send_subscription(&c, con->get_handle());
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

            // Load Alpaca config from the nested structure
            apiKey = config["marketAdapter"]["alpacaAdapter"]["api_key"];
            secretKey = config["marketAdapter"]["alpacaAdapter"]["api_secret"];
            baseUrl = config["marketAdapter"]["alpacaAdapter"]["base_url"];
            dataUrl = config["marketAdapter"]["alpacaAdapter"]["data_url"];

            std::cout << "Alpaca config loaded successfully." << std::endl;
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

void AlpacaAdapter::on_message(websocket_client* c, websocketpp::connection_hdl hdl, websocket_client::message_ptr msg) {
    std::lock_guard<std::mutex> guard(mtx);
    std::cout << "< Received message: " << msg->get_payload() << std::endl;
    std::cout << "" << std::endl;
}

void AlpacaAdapter::on_open(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connected to WebSocket server!" << std::endl;
    AlpacaAdapter::send_auth(c, hdl);
}

void AlpacaAdapter::on_close(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed!" << std::endl;
}

void AlpacaAdapter::on_fail(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connection failed!" << std::endl;
}

void AlpacaAdapter::sendMessage(websocket_client* c, connection_hdl hdl, const nlohmann::json& message) {
    try {
        std::string payload = message.dump();
        c->send(hdl, payload, websocketpp::frame::opcode::text);
    } catch (const std::exception &e) {
        std::cerr << "Error sending WebSocket message: " << e.what() << std::endl;
    }
}

void AlpacaAdapter::send_auth(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::string auth_msg = R"({"action":"auth","key":")" + apiKey + R"(","secret":")" + secretKey + R"("})";
    c->send(hdl, auth_msg, websocketpp::frame::opcode::text);
    // std::cout << "> Auth message: " << auth_msg << std::endl;
}

void AlpacaAdapter::send_subscription(websocket_client* c, websocketpp::connection_hdl hdl) {
    std::string sub_msg = R"({"action":"subscribe","bars":["FAKEPACA"],"quotes":["FAKEPACA"]})";
    c->send(hdl, sub_msg, websocketpp::frame::opcode::text);
    std::cout << "> Subscription message: " << sub_msg << std::endl;
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
}
