#include <iostream>
#include <fstream>

//#include "../include/Aggregator.h"
//#include "../include/MarketDataGateway.h"
//#include "../include/OrderGateway.h"
//#include "../include/OrderManagementSystem.h"
//#include "../include/TradingEngine.h"
#include "../include/json.hpp"
#include "../include/MarketDataGateway.h"

using namespace std;

int main() {
    // Initialize components
    MarketDataGateway dataGateway;

    dataGateway.initialize("../config/config.json");

    // string response1 = dataGateway.getMarketData("tsla","iex");
    string response2 = dataGateway.getLatestQuotes("tsla","iex");
    // cout << response1 << endl;
    cout << "" << endl;
    cout << response2 << endl;
    return 0;
}
