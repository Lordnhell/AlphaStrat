//
// Created by Admin on 26/9/2024.
//
#include "../include/MarketDataParser.h"
#include <iostream>

// Helper function to print bars
void MarketDataParser::printBar(const nlohmann::json& bar) {
    if (bar.contains("open") && bar.contains("high") && bar.contains("low") && bar.contains("close") && bar.contains("volume")) {
        std::cout << "Open: " << bar["open"] << ", High: " << bar["high"]
                  << ", Low: " << bar["low"] << ", Close: " << bar["close"]
                  << ", Volume: " << bar["volume"] << std::endl;
    } else {
        std::cerr << "Invalid Bar Data!" << std::endl;
    }
}

// Helper function to print quotes
void MarketDataParser::printQuote(const nlohmann::json& quote) {
    if (quote.contains("bp") && quote.contains("ap") && quote.contains("bs") && quote.contains("as")) {
        std::cout << "Bid Price: " << quote["bp"] << ", Bid Size: " << quote["bs"]
                  << ", Ask Price: " << quote["ap"] << ", Ask Size: " << quote["as"] << std::endl;
    } else {
        std::cerr << "Invalid Quote Data!" << std::endl;
    }
}

// Helper function to print trades
void MarketDataParser::printTrade(const nlohmann::json& trade) {
    if (trade.contains("price") && trade.contains("size") && trade.contains("timestamp")) {
        std::cout << "Trade Price: " << trade["price"] << ", Size: " << trade["size"]
                  << ", Timestamp: " << trade["timestamp"] << std::endl;
    } else {
        std::cerr << "Invalid Trade Data!" << std::endl;
    }
}

// Helper function to print snapshots (combination of trades and quotes)
void MarketDataParser::printSnapshot(const nlohmann::json& snapshot) {
    if (snapshot.contains("latest_trade") && snapshot.contains("latest_quote")) {
        std::cout << "Snapshot - Latest Trade and Quote:" << std::endl;
        std::cout << "Latest Trade: ";
        printTrade(snapshot["latest_trade"]);
        std::cout << "Latest Quote: ";
        printQuote(snapshot["latest_quote"]);
    } else {
        std::cerr << "Invalid Snapshot Data!" << std::endl;
    }
}

// Parse Bar Data (OHLCV)
void MarketDataParser::parseBarData(const std::string& jsonResponse) {
    try {
        nlohmann::json data = nlohmann::json::parse(jsonResponse);
        if (data.contains("bar")) {
            printBar(data["bar"]);
        } else if (data.contains("bars")) {
            for (const auto& bar : data["bars"]) {
                printBar(bar);
            }
        } else {
            std::cerr << "No bar data found!" << std::endl;
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}

// Parse Quote Data (Bid/Ask)
void MarketDataParser::parseQuoteData(const std::string& jsonResponse) {
    try {
        nlohmann::json data = nlohmann::json::parse(jsonResponse);
        if (data.contains("quote")) {
            printQuote(data["quote"]);
        } else if (data.contains("quotes")) {
            for (const auto& [symbol, quote] : data["quotes"].items()) {
                std::cout << "Symbol: " << symbol << std::endl;
                printQuote(quote);
            }
        } else {
            std::cerr << "No quote data found!" << std::endl;
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}

// Parse Trade Data
void MarketDataParser::parseTradeData(const std::string& jsonResponse) {
    try {
        nlohmann::json data = nlohmann::json::parse(jsonResponse);
        if (data.contains("trade")) {
            printTrade(data["trade"]);
        } else if (data.contains("trades")) {
            for (const auto& [symbol, trade] : data["trades"].items()) {
                std::cout << "Symbol: " << symbol << std::endl;
                printTrade(trade);
            }
        } else {
            std::cerr << "No trade data found!" << std::endl;
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}

// Parse Snapshot Data
void MarketDataParser::parseSnapshotData(const std::string& jsonResponse) {
    try {
        nlohmann::json data = nlohmann::json::parse(jsonResponse);
        if (data.contains("snapshot")) {
            printSnapshot(data["snapshot"]);
        } else if (data.contains("snapshots")) {
            for (const auto& [symbol, snapshot] : data["snapshots"].items()) {
                std::cout << "Symbol: " << symbol << std::endl;
                printSnapshot(snapshot);
            }
        } else {
            std::cerr << "No snapshot data found!" << std::endl;
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}
