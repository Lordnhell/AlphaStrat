//
// Created by Admin on 3/3/2025.
//

#ifndef ALPHASTRATDTOS_H
#define ALPHASTRATDTOS_H

#endif //ALPHASTRATDTOS_H
#pragma once
#include <string>
#include <optional>
#include <chrono>

/**
 * DTO struct for Market Data messages.
 * This is what we'll publish on Solace (as JSON or otherwise).
 */
struct MarketDataDTO {
    std::string symbol;         // e.g. "AAPL"
    double price = 0.0;         // last trade or quote price
    double size = 0.0;          // last trade or quote size
    std::string feed;           // e.g. "iex", "SMART", "Alpaca", etc.
    long long timestamp = 0;    // epoch millis or micros
    // Add more fields as needed (e.g. bid/ask, exchange, etc.)
};

/**
 * DTO struct for Order requests or updates.
 * This might be a lighter-weight struct than your Order class,
 * focusing only on fields needed for transport across your system.
 */
struct OrderDTO {
    std::string orderId;        // Unique ID, e.g. "clientOrderID"
    std::string symbol;         // "AAPL"
    std::string side;           // "BUY" or "SELL"
    double quantity = 0.0;
    std::string orderType;      // "MARKET", "LIMIT", etc.
    double limitPrice = 0.0;    // if applicable
    double stopPrice = 0.0;     // if applicable
    bool extendedHours = false;
    // etc. for additional fields
};
