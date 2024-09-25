//
// Created by Admin on 3/9/2024.
//
// Standard and external library includes
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include "../include/json.hpp"

// Your project's includes
#include "../include/MarketDataGateway.h"

// Windows headers
#ifdef _WIN32
    // #define NOMINMAX
    #include <windows.h>
#endif

// Constructor
MarketDataGateway::MarketDataGateway() {}

// Initialize the gateway with API keys from the config file
void MarketDataGateway::initialize(const std::string& configFile) {
    loadConfig(configFile);
}

// Load configuration from JSON file
void MarketDataGateway::loadConfig(const std::string& configFile) {
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

// Callback function to capture CURL response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Get market data for a given symbol
std::string MarketDataGateway::getMarketData(const std::string& symbol,const string& feed) {
    CURL *hnd = curl_easy_init();
    if (!hnd) {
        return "CURL initialization failed";
    }

    std::string readBuffer;
    std::string url = dataUrl + "v2/stocks/quotes/latest?symbols=" + symbol + "&feed=" + feed;
    cout << url << endl;

    curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);  // Set callback function
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &readBuffer);  // Pass the string to store data

    // Prepare headers
    struct curl_slist *headers = NULL;
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

    return readBuffer;  // Return the result
}
