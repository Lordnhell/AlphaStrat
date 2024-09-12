#include <iostream>

#include "../include/Aggregator.h"
#include "../include/MarketDataGateway.h"
#include "../include/OrderGateway.h"
#include "../include/OrderManagementSystem.h"
#include "../include/TradingEngine.h"

int main() {
    // Initialize components
    MarketDataGateway marketDataGateway;
    Aggregator aggregator;
    TradingEngine tradingEngine;
    OrderManagementSystem orderManagementSystem;
    OrderGateway orderGateway;

    // Initialize with config
    marketDataGateway.initialize("../config/config.json");

    // Fetch market data
    std::string marketData = marketDataGateway.getMarketData("AAPL");
    aggregator.addOrderBookData(marketData);

    // Execute trading strategy
    std::string aggregatedData = aggregator.getAggregatedData();
    tradingEngine.executeTradingStrategy(aggregatedData);

    // Simulate order and lifecycle management
    std::string order = "Buy AAPL 10 @ 150.00";
    tradingEngine.sendOrderRequest(order);
    orderManagementSystem.manageOrderLifecycle("Order1", "Submitted");

    // Send and receive execution report
    orderGateway.sendOrderToExchange(order);
    orderGateway.receiveExecutionReport("Order executed");

    // Implement circuit breaker
    orderGateway.implementCircuitBreaker();

    return 0;
}
