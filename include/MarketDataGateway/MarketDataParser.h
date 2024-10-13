#ifndef MARKETDATA_PARSER_H
#define MARKETDATA_PARSER_H

#include <string>
#include <nlohmann/json.hpp>

class MarketDataParser {
public:
    // Static methods for parsing different response types
    static void parseBarData(const std::string& jsonResponse);
    static std::string parseQuoteData(const std::string& jsonResponse);
    static void parseTradeData(const std::string& jsonResponse);
    static void parseSnapshotData(const std::string& jsonResponse);

private:
    static void printBar(const nlohmann::json& bar);
    static std::string printQuote(const nlohmann::json& quote);
    static void printTrade(const nlohmann::json& trade);
    static void printSnapshot(const nlohmann::json& snapshot);
    static std::string convertISO8601(const std::string& date);
};

#endif // MARKETDATA_PARSER_H
