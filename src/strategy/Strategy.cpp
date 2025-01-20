#include "Strategy.h"
#include <iostream>

Strategy::Strategy(const std::string& strategyName, const std::string& solaceConfig)
    : name(strategyName), solaceClient(solaceConfig), running(false) {}

Strategy::~Strategy() {
    stop();
}

void Strategy::start() {
    running = true;
    strategyThread = std::thread(&Strategy::eventLoop, this);
}

void Strategy::stop() {
    running = false;
    if (strategyThread.joinable()) {
        strategyThread.join();
    }
}

void Strategy::eventLoop() {
    while (running) {
        // Simulate event listening (replace with Solace-based message handling)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Strategy::requestLiveData(const std::string& ticker) {
    std::string topic = "trading/marketdata/live/request/" + ticker;
    solaceClient.publishMessage(topic, "{}"); // Example empty payload
}

void Strategy::requestHistoricalData(const std::string& ticker, const std::string& dateRange) {
    std::string topic = "trading/marketdata/historical/request/" + ticker + "/" + dateRange;
    solaceClient.publishMessage(topic, "{}"); // Example empty payload
}

void Strategy::sendOrder(const Order& order) {
    nlohmann::json orderJson = {
        {"clientOrderID", order.getClientOrderID()},
        {"instrument", order.getInstrument()},
        {"side", order.getSide()},
        {"quantity", order.getQuantity()}
    };
    solaceClient.publishMessage("trading/order/request/anyBroker/action/" + name, orderJson.dump());
}