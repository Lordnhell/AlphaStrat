#include <iostream>
#include <fstream>

//#include "../include/Aggregator.h"
//#include "../include/MarketDataGateway.h"
//#include "../include/OrderGateway.h"
//#include "../include/OrderManagementSystem.h"
//#include "../include/TradingEngine.h"
#include "../include/json.hpp"
#include "../include/MarketDataGateway/MarketDataGateway.h"
#include "../include/solace/solaceLib.h"
#include "Adapters/AlpacaAdapter.h"

using namespace std;

// Signal handler
AlpacaAdapter* adapter_ptr = nullptr;

void signalHandler(int signal) {
    std::cout << "Interrupt signal (" << signal << ") received. Performing graceful shutdown..." << std::endl;
    if (adapter_ptr) {
        adapter_ptr->gracefulDisconnect();
    }
    exit(signal);
}

// Function to run an adapter and subscribe to live data
void runAdapter(const std::string& configFile, const std::vector<std::string>& tickers, bool testMode) {
    AlpacaAdapter adapter(configFile);
    adapter.subscribeLiveData(tickers, testMode);
}

int main() {

    //add thread pool for handling many adapters, doing a manual approach for now
    //(alpaca only allows one subscription (free), this will be used for other adapters)

    // Register signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Example for running multiples threads
    // std::vector<std::string> configFiles = {
    //     "../config/config.json",
    //     "../config/config.json"// Add as many as you want
    // };
    //
    // std::vector<std::vector<std::string>> tickersList = {
    //     {"AAPL", "GOOG"},
    //     {"MSFT", "TSLA"}
    // };

    std::vector<std::string> configFiles = {
        "../config/config.json"
    };

    std::vector<std::vector<std::string>> tickersList = {
        {"AAPL", "GOOG"}
    };

    std::vector<std::thread> threads;
    // User selects mode
    bool testMode = true;
    cout << "Running in " << (testMode ? "Test Mode" : "Paper Trading Mode") << endl;

    // Create and start adapters in separate threads
    for (size_t i = 0; i < configFiles.size(); ++i) {
        // Start each adapter in a separate thread
        threads.emplace_back(runAdapter, configFiles[i], tickersList[i], testMode);
    }

    // Wait for all threads to finish (join them)
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    // AlpacaAdapter alpaca_adapter("../config/config.json");
    // // alpaca_adapter.initialize("../config/config.json");
    //
    // string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;
    //
    // cout << "" << endl;
    // cout << response2 << endl;

    return 0;
}