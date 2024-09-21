//
// Created by Admin on 3/9/2024.
//

#ifndef MARKETDATA_GATEWAY_H
#define MARKETDATA_GATEWAY_H

#include <string>

using namespace std;

class MarketDataGateway {
public:
    MarketDataGateway();
    void initialize(const std::string& configFile);
    std::string getMarketData(const std::string& symbol, const string& feed);

private:
    string apiKey;
    string secretKey;
    string baseUrl;
    string dataUrl;

    void loadConfig(const std::string& configFile);
};

#endif // MARKETDATA_GATEWAY_H
