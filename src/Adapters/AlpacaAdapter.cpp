//
// Created by Admin on 1/10/2024.
//
#include <fstream>
#include <iostream>
#include <curl/curl.h>
// #include <iostream>
#include <vector>
#include <string>

#include "../include/Adapters/AlpacaAdapter.h"
#include "../include/MarketDataGateway/MarketDataParser.h"


using namespace std;

void AlpacaAdapter::initialize(const std::string& configFile) {
    loadConfig(configFile);
}

string AlpacaAdapter::getLatestTick(const std::string &symbol, const std::string &feed) {
    std::string url = dataUrl + "v2/stocks/quotes/latest?symbols=" + symbol + "&feed=" + feed;
    std::cout << "Fetching market data for " << symbol << " from: " << url << std::endl;
    return MarketDataParser::parseQuoteData(performRequest(url));
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Public method to subscribe to live data for multiple tickers
void AlpacaAdapter::subscribeLiveData(const std::vector<std::string>& tickers) {
    websocket_client c;
    std::string url = "wss://paper-api.alpaca.markets/stream";  // WebSocket URL for live trading data

    // Initialize WebSocket connection
    initializeWebSocketConnection(c, url);

    try {
        websocketpp::lib::error_code ec;
        websocket_client::connection_ptr con = c.get_connection(url, ec);

        if (ec) {
            std::cerr << "Failed to initialize connection: " << ec.message() << std::endl;
            return;
        }

        // Set message handler to handle incoming messages
        c.set_message_handler(std::bind(&AlpacaAdapter::on_message, this, &c, std::placeholders::_1, std::placeholders::_2));

        // Connect and start the WebSocket event loop
        c.connect(con);

        // WebSocket run in a separate thread
        std::thread websocket_thread([&c]() {
            c.run();
        });

        // Authenticate with API key and secret
        std::string auth_message = buildAuthMessage();
        c.send(con->get_handle(), auth_message, websocketpp::frame::opcode::text);

        // Subscribe to the streams after authentication
        std::string subscription_message = buildSubscriptionMessage(tickers);
        c.send(con->get_handle(), subscription_message, websocketpp::frame::opcode::text);

        // Join the WebSocket thread back to the main thread
        websocket_thread.join();

    } catch (const websocketpp::exception& e) {
        std::cerr << "WebSocket exception: " << e.what() << std::endl;
    }
}

void AlpacaAdapter::loadConfig(const std::string &configFile) {
    std::ifstream file(configFile);
    if (file.is_open()) {
        nlohmann::json config;
        file >> config;
        apiKey = config["api_key"];
        // cout << "API Key: " << apiKey << endl;
        secretKey = config["api_secret"];
        // cout << "API Secret: " << secretKey << endl;
        baseUrl = config["base_url"];
        // cout << "Base URL: " << baseUrl << endl;
        dataUrl = config["data_url"];
        // cout << "Data URL: " << dataUrl << endl;
    } else {
        std::cerr << "Unable to open config file" << std::endl;
    }
}

string AlpacaAdapter::performRequest(const std::string &url) {
    CURL* hnd = curl_easy_init();
    if (!hnd) {
        return "CURL initialization failed";
    }

    std::string readBuffer;
    curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);  // Set callback function
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &readBuffer);  // Pass the string to store data

    // Prepare headers
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    std::string apiKeyHeader = "APCA-API-KEY-ID: " + apiKey;
    headers = curl_slist_append(headers, apiKeyHeader.c_str());
    std::string apiSecretHeader = "APCA-API-SECRET-KEY: " + secretKey;
    headers = curl_slist_append(headers, apiSecretHeader.c_str());

    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    CURLcode res = curl_easy_perform(hnd);

    // Check for errors
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);

    return readBuffer;  // Return the response
}

// Message handler for incoming WebSocket messages
void AlpacaAdapter::on_message(websocket_client* c, connection_hdl hdl, websocket_client::message_ptr msg) {
    std::string payload = msg->get_payload();
    auto json_msg = nlohmann::json::parse(payload);

    if (json_msg["stream"] == "trade_updates") {
        std::cout << "Received trade update: " << json_msg.dump(4) << std::endl;
    } else if (json_msg["stream"] == "authorization") {
        if (json_msg["data"]["status"] == "authorized") {
            std::cout << "WebSocket connection authorized." << std::endl;
        } else {
            std::cerr << "WebSocket authorization failed." << std::endl;
        }
    }
}

// Send WebSocket message
void AlpacaAdapter::sendMessage(websocket_client* c, connection_hdl hdl, const nlohmann::json& message) {
    std::string payload = message.dump();
    c->send(hdl, payload, websocketpp::frame::opcode::text);
}

// Build authentication message
std::string AlpacaAdapter::buildAuthMessage() {
    nlohmann::json auth_message = {
        {"action", "auth"},
        {"key", apiKey},
        {"secret", secretKey}
    };
    return auth_message.dump();
}

// Build subscription message for trade updates
std::string AlpacaAdapter::buildSubscriptionMessage(const std::vector<std::string>& tickers) {
    nlohmann::json subscribe_message = {
        {"action", "listen"},
        {"data", {
                {"streams", {"trade_updates"}}
        }}
    };
    return subscribe_message.dump();
}


// Helper function to initialize the WebSocket connection
void AlpacaAdapter::initializeWebSocketConnection(websocket_client& c, const std::string& url) {
    c.init_asio();  // ASIO initialization for WebSocket++
}