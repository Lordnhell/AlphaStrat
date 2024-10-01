//
// Created by Admin on 26/9/2024.
//
#include "../../include/MarketDataGateway/MarketDataParser.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

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
std::string MarketDataParser::printQuote(const nlohmann::json& quote) {
    if (quote.contains("bp")
        && quote.contains("ap")
        && quote.contains("bs")
        && quote.contains("as")
        && quote.contains("t")) {

        std::ostringstream oss;
        oss << "Bid Price: " << quote["bp"] << ", Bid Size: " << quote["bs"]
            << ", Ask Price: " << quote["ap"] << ", Ask Size: " << quote["as"] <<", Date and Time: "<< convertISO8601(quote["t"]);
        return oss.str();
    } else {
        std::cerr << "Invalid Quote Data!" << std::endl;
        return "";
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
std::string MarketDataParser::parseQuoteData(const std::string& jsonResponse) {
    try {
        std::cout << jsonResponse << std::endl;
        nlohmann::json data = nlohmann::json::parse(jsonResponse);

        // To collect all output in case there are multiple quotes
        std::string result;

        if (data.contains("quote")) {
            // If there's a single quote
            result = printQuote(data["quote"]);
        } else if (data.contains("quotes")) {
            // If there are multiple quotes, iterate through each
            for (const auto& [symbol, quote] : data["quotes"].items()) {
                result += "Symbol: " + symbol + "\n";
                result += printQuote(quote) + "\n";
            }
        } else {
            std::cerr << "No quote data found!" << std::endl;
            return "";
        }

        return result;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return "";
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

std::string MarketDataParser::convertISO8601(const std::string &date) {
    // Extract the date and time part from the ISO 8601 string
    std::tm tm = {};
    std::istringstream ss(date.substr(0, 19)); // Take only the first part, excluding fractional seconds and 'Z'

    // Parse the string to fill the tm structure
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    if (ss.fail()) {
        return "Failed to parse time";
    }

    // Convert to time_t (time since epoch)
    std::time_t time = std::mktime(&tm);

    // Format the time to a readable string
    std::ostringstream result;
    result << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return result.str();
}

