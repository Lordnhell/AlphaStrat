//
// Created by Admin on 3/9/2024.
//

#ifndef MARKETDATA_GATEWAY_H
#define MARKETDATA_GATEWAY_H

#include <string>

class MarketDataGateway {
public:
    MarketDataGateway();
    void initialize(const std::string& configFile);
    std::string getMarketData(const std::string& symbol);

private:
    std::string apiKey;
    std::string secretKey;
    std::string baseUrl;

    void loadConfig(const std::string& configFile);
};

#endif // MARKETDATA_GATEWAY_H
