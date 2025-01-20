#ifndef STRATEGY_H
#define STRATEGY_H

#include <string>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>
#include "../include/solace/solaceLib.h"
#include "../include/orderGateway/Order.h"

class Strategy {
public:
    Strategy(const std::string& strategyName, const std::string& solaceConfig);
    virtual ~Strategy();

    void start(); // Start the strategy thread
    void stop();  // Stop the strategy thread
    virtual void run() = 0; // Main logic to be implemented by derived classes

protected:
    void requestLiveData(const std::string& ticker);
    void requestHistoricalData(const std::string& ticker, const std::string& dateRange);
    void sendOrder(const Order& order);

    std::atomic<bool> running;
    std::string name;
    solaceLib solaceClient;

    // Derived classes can override these for custom event handling
    virtual void onLiveUpdate(const nlohmann::json& data) {}
    virtual void onHistoricalResponse(const nlohmann::json& data) {}

private:
    std::thread strategyThread;
    void eventLoop(); // Internal event loop
};

#endif // STRATEGY_H