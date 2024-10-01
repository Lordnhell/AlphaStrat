#include "gtest/gtest.h"
#include "../include/MarketDataGateway/MarketDataGateway.h"
#include <fstream>
#include "../include/json.hpp"
#include <string>
using namespace std;

class MarketDataGatewayTest : public ::testing::Test {
protected:
    MarketDataGateway mdg;
};

// Test loading from actual config
// TEST_F(MarketDataGatewayTest, LoadActualConfig) {
//     mdg.initialize("../config/config.json");  // Test with actual config file
//
//     // Access the baseUrl directly since we're using a friend class
//     std::string expectedBaseUrl = "https://paper-api.alpaca.markets/";  // Update this if necessary
//     EXPECT_EQ(mdg.baseUrl, expectedBaseUrl);  // Direct access to private member
// }

// Test the getMarketData function (mocking the CURL request)
// TEST_F(MarketDataGatewayTest, getMarketData) {
//     // Assuming that calling `getMarketData` will at least return something non-empty if successful
//     std::string result = mdg.getMarketData("AAPL", "IEX");
//     EXPECT_NE(result, "");  // We assume a valid request returns non-empty data
// }
