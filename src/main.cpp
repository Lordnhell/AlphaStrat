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

    string response = dataGateway.getMarketData("tsla","iex");
    cout << response << endl;
    return 0;
}
