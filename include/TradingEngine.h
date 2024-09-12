//
// Created by Admin on 3/9/2024.
//

#ifndef TRADING_ENGINE_H
#define TRADING_ENGINE_H

#include <string>

class TradingEngine {
public:
    TradingEngine();
    void executeTradingStrategy(const std::string& aggregatedData);
    void sendOrderRequest(const std::string& order);
    void receiveStatusUpdate(const std::string& update);
};

#endif // TRADING_ENGINE_H
