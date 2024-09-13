//
// Created by Admin on 3/9/2024.
//
#include "../include/MarketDataGateway.h"
#include <fstream>
#include <iostream>
#include "../include/json.hpp"
#include <cpr/cpr.h>

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
        apiKey = config["alpaca_api_key"];
        secretKey = config["alpaca_secret_key"];
        baseUrl = config["alpaca_base_url"];
    } else {
        std::cerr << "Unable to open config file" << std::endl;
    }
}

// Get market data for a given symbol
std::string MarketDataGateway::getMarketData(const std::string& symbol) {
    auto response = cpr::Get(cpr::Url{baseUrl + "/v2/stocks/" + symbol + "/quote"},
                             cpr::Header{{"APCA-API-KEY-ID", apiKey},
                                         {"APCA-API-SECRET-KEY", secretKey}});
    if (response.status_code == 200) {
        return response.text;
    } else {
        return "Error fetching market data";
    }
}
