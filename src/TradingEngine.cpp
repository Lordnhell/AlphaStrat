//
// Created by Admin on 3/9/2024.
//
#include "../include/TradingEngine.h"
#include <iostream>

TradingEngine::TradingEngine() {}

void TradingEngine::executeTradingStrategy(const std::string& aggregatedData) {
    // Example strategy implementation
    std::cout << "Executing strategy on aggregated data: " << aggregatedData << std::endl;
}

void TradingEngine::sendOrderRequest(const std::string& order) {
    std::cout << "Sending order request: " << order << std::endl;
}

void TradingEngine::receiveStatusUpdate(const std::string& update) {
    std::cout << "Order update received: " << update << std::endl;
}
