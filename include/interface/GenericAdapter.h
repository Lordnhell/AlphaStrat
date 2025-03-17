//
// Created by Admin on 1/10/2024.
//

#ifndef GENERICADAPTER_H
#define GENERICADAPTER_H

#include <string>
#include <vector>
#include "../include/orderGateway/Order.h"
#include "../include/DTOs/AlphaStratDTOs.h"
#include <iostream>

class GenericAdapter {
public:
    // Virtual destructor to allow proper cleanup
    virtual ~GenericAdapter() = default;

    // Initialization (pure virtual, as all adapters need this)
    virtual void initialize(const std::string& configFile) = 0;

    // Get latest tick data (optional, may not be implemented by all adapters)
    virtual std::string getLatestTick(const std::string& symbol, const std::string& feed) {
        return "getLatestTick() is not supported or not implemented by this adapter.";
    }

    // Subscribe to live market data (optional, may not be implemented by all adapters)
    virtual void subscribeLiveData(const std::vector<std::string>& tickers, bool testMode) {
        std::cerr << "subscribeLiveData() is not supported or not implemented by this adapter." << std::endl;
    }

    // Order creation (optional, may not be implemented by all adapters)
    virtual std::string createOrder(const Order& order) {
        std::cerr << "createOrder() is not supported or not implemented by this adapter." << std::endl;
        return "";
    }

    // Historical data retrieval (optional, may not be implemented by all adapters)
    virtual void getHistoricalData(const std::string& symbol, int startTimeDate, int endStartDate) {
        std::cerr << "getHistoricalData() is not supported or not implemented by this adapter." << std::endl;
    }

    // In include/interface/GenericAdapter.h
    virtual void publishMarketDataDTO(const MarketDataDTO& dto) {
        // Default: do nothing or log a warning
        std::cerr << "[GenericAdapter] publishMarketDataDTO() not implemented.\n";
    }

    // Similarly, if you want a method for OrderDTO:
    virtual void publishOrderDTO(const OrderDTO& dto) {
        std::cerr << "[GenericAdapter] publishOrderDTO() not implemented.\n";
    }

    // // Sending orders (optional, may not be implemented by all adapters)
    // virtual void sendOrder() {
    //     std::cerr << "sendOrder() is not supported or not implemented by this adapter." << std::endl;
    // }
    //
    // // Order management methods (optional, may not be implemented by all adapters)
    // virtual void orderManagementMethod() {
    //     std::cerr << "orderManagementMethod() is not supported or not implemented by this adapter." << std::endl;
    // }
    //
    // // Solace-related methods (optional, may not be implemented by all adapters)
    // virtual void publisherToTopic() {
    //     std::cerr << "publisherToTopic() is not supported or not implemented by this adapter." << std::endl;
    // }
    //
    // virtual void subscribeToTopic() {
    //     std::cerr << "subscribeToTopic() is not supported or not implemented by this adapter." << std::endl;
    // }
};

#endif //GENERICADAPTER_H
