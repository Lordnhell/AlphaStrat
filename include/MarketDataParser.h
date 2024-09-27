#ifndef MARKETDATA_PARSER_H
#define MARKETDATA_PARSER_H

#include <string>
#include "../include/json.hpp"

class MarketDataParser {
public:
    // Static methods for parsing different response types
    static void parseBarData(const std::string& jsonResponse);
    static void parseQuoteData(const std::string& jsonResponse);
    static void parseTradeData(const std::string& jsonResponse);
    static void parseSnapshotData(const std::string& jsonResponse);

private:
    static void printBar(const nlohmann::json& bar);
    static void printQuote(const nlohmann::json& quote);
    static void printTrade(const nlohmann::json& trade);
    static void printSnapshot(const nlohmann::json& snapshot);
};

#endif // MARKETDATA_PARSER_H
