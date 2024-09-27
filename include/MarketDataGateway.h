//
// Created by Admin on 3/9/2024.
//

#ifndef MARKETDATA_GATEWAY_H
#define MARKETDATA_GATEWAY_H

#include <string>
#include "MarketDataParser.h"

using namespace std;
// class MarketDataGatewayTest;

class MarketDataGateway {
public:
    MarketDataGateway();
    void initialize(const std::string& configFile);
    string getMarketData(const std::string& symbol, const string& feed);
    string getLatestQuotes(const std::string& symbol,const string& feed);


private:
    std::string apiKey;
    std::string secretKey;
    std::string baseUrl;
    std::string dataUrl;

    void loadConfig(const std::string& configFile);
    // friend class MarketDataGatewayTest;
};

#endif // MARKETDATA_GATEWAY_H
