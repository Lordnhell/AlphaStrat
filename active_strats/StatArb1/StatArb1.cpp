#include "StatArb1.h"
#include <iostream>

StatArb1::StatArb1(const std::string& solaceConfig) : Strategy("StatArb1", solaceConfig) {}

void StatArb1::run() {
    requestLiveData("AAPL");
    requestHistoricalData("AAPL", "2022-01-01_2023-01-01");

    while (running) {
        // Main strategy logic
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "StatArb1 running...\n";
    }
}

void StatArb1::onLiveUpdate(const nlohmann::json& data) {
    std::cout << "Received live update: " << data.dump() << "\n";
}