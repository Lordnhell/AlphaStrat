//
// Created by Admin on 1/10/2024.
//

#ifndef GENERICADAPTER_H
#define GENERICADAPTER_H

#include <string>

class GenericAdapter {
public:
    virtual ~GenericAdapter() = default;
    virtual void initialize(const std::string& configFile) = 0;

    virtual std::string getLatestTick (const std::string& symbol,const std::string& feed) = 0;
    virtual void subscribeLiveData (const std::vector<std::string>& tickers, bool testMode) = 0;

    // historical
    // virtual void getHistoricalData(const std::string symbol, const int startTimeDate,, const int endStartDate) = 0;

    // fill these up
    // virtual void sendOrder() = 0;
    // virtual void orderManagementMethod() = 0;

    // solace related
    // virtual void publisherToSolace() = 0;

};


#endif //GENERICADAPTER_H