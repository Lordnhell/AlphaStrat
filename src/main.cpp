#include <iostream>
#include <fstream>

//#include "../include/Aggregator.h"
//#include "../include/MarketDataGateway.h"
//#include "../include/OrderGateway.h"
//#include "../include/OrderManagementSystem.h"
//#include "../include/TradingEngine.h"
#include "../include/json.hpp"
#include "../include/MarketDataGateway/MarketDataGateway.h"
#include "Adapters/AlpacaAdapter.h"

using namespace std;

std::atomic<bool> running(true);  // Use atomic flag to control the main loop

// Signal handler to stop the program gracefully
void signalHandler(int signal) {
    std::cout << "\nSignal (" << signal << ") received. Stopping the program..." << std::endl;
    running = false;
}

int main() {

    signal(SIGINT, signalHandler);

    AlpacaAdapter alpaca_adapter;
    alpaca_adapter.initialize("../config/config.json");

    string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;

    cout << "" << endl;
    cout << response2 << endl;

    // Create an instance of the AlpacaAdapter
    AlpacaAdapter adapter;

    // Initialize with a configuration file (make sure to replace with the actual path)
    adapter.initialize("../config/config.json");

    // Tickers to subscribe for live data (e.g., Apple and Google)
    std::vector<std::string> tickers = {"AAPL", "GOOG"};

    // User selects mode
    bool testMode = true;
    cout << "Running in " << (testMode ? "Test Mode" : "Paper Trading Mode") << endl;

    // Subscribe to live data for the tickers
    adapter.subscribeLiveData(tickers, testMode);

    return 0;
}