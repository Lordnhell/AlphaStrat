//
// Created by Admin on 1/10/2024.
//
#include <fstream>
#include <iostream>
#include <curl/curl.h>

#include "../include/Adapters/AlpacaAdapter.h"
#include "../include/MarketDataGateway/MarketDataParser.h"
#include <nlohmann/json.hpp>

#ifdef _WIN32
    // #define NOMINMAX
    #include <windows.h>
#endif

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
