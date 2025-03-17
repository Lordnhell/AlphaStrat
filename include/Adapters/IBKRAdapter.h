//
// Created by Admin on 3/3/2025.
//

#ifndef IBKRADAPTER_H
#define IBKRADAPTER_H
#pragma once

#include "../interface/GenericAdapter.h"
#include <vector>
#include <string>
#include <mutex>
#include <thread>
// #include <orderGateway/Order.h>

class IBKRAdapter : public GenericAdapter {
public:
    IBKRAdapter(const std::string& configFile);
    ~IBKRAdapter();

    // Overridden from GenericAdapter
    void initialize(const std::string& configFile) override;
    std::string getLatestTick(const std::string& symbol, const std::string& feed) override;
    void subscribeLiveData(const std::vector<std::string>& tickers, bool testMode) override;
    std::string createOrder(const Order& order) override; // optional

    // Graceful disconnect
    void disconnect();


private:
    void loadConfig(const std::string& configFile);

};



#endif //IBKRADAPTER_H
