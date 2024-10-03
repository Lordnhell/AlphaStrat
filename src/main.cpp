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

int main() {
    // // Initialize components
    MarketDataGateway dataGateway;
    //
    // dataGateway.initialize("../config/config.json");
    //
    // // string response1 = dataGateway.getMarketData("tsla","iex");
    // string response2 = dataGateway.getLatestQuotes("tsla","iex");
    // cout << response1 << endl;

    AlpacaAdapter alpaca_adapter;
    alpaca_adapter.initialize("../config/config.json");

    string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;

    cout << "" << endl;
    cout << response2 << endl;

    // // Create an instance of the AlpacaAdapter
    // AlpacaAdapter adapter;
    //
    // // Initialize with a configuration file (make sure to replace with the actual path)
    // adapter.initialize("config.json");
    //
    // // Tickers to subscribe for live data (e.g., Apple and Google)
    // std::vector<std::string> tickers = {"AAPL", "GOOG"};
    //
    // // Subscribe to live data for the tickers
    // adapter.subscribeLiveData(tickers);

    return 0;
}
