#ifndef ORDERMANAGEMENTSYSTEM_H
#define ORDERMANAGEMENTSYSTEM_H

#include <string>
#include <map>
#include <vector>
#include <thread>
#include <functional>
#include "../include/orderGatway/OMS.h"
#include "../include/orderGatway/Order.h"
#include "../interface/GenericAdapter.h" // Base class for different adapters (IBKRAdapter, AlpacaAdapter, etc.)

class OrderManagementSystem {
public:
    // Constructor and Destructor
    // OrderManagementSystem();
    // ~OrderManagementSystem();

    // Function to initialize and register adapters
    void initializeAdapters();
    //
    // // Event Listener for orders (listens to Solace topics)
    // void listenForOrders();
    //
    // // Function to process incoming orders concurrently
    // void processOrderConcurrently(const Order& order);

    // Function to generate a unique order ID
    // std::string generateOrderID(const std::string& instrumentSymbol, const std::string& orderType, const std::string& teId);
    //
    // // Function to submit a new order (calls the relevant adapter)
    // void submitOrder(const Order& order);
    //
    // // Function to cancel an existing order
    // void cancelOrder(const std::string& orderId);

    // // Function to modify an existing order
    // void modifyOrder(const std::string& orderId, const Order& modifiedOrder);
    //
    // // Function to handle partial fills and track remaining quantity
    // void handlePartialFill(const std::string& orderId, double filledQuantity);
    //
    // // Function to select the optimal adapter based on the order details
    // Adapter* selectOptimalAdapter(const Order& order);
    //
    // // Function to handle real-time portfolio updates
    // void updatePortfolio(const std::string& instrument, double quantity, double price);
    //
    // // Function to calculate real-time PnL for the current portfolio
    // double calculatePnL(const std::string& instrument);
    //
    // // Function to fetch real-time or historical market data
    // std::vector<double> fetchMarketData(const std::string& instrument, const std::string& timeframe, const std::string& dataType);
    //
    // // Function to retrieve current portfolio details
    // std::map<std::string, double> getPortfolio();
    //
    // // Function to set and check risk management rules
    // void setRiskManagement(double maxExposure, double maxLoss);
    // bool checkRiskLimits(const Order& order);
    //
    // // Function to reject excess orders if risk limits or quantities are exceeded
    // bool rejectExcessOrders(const Order& order);
    //
    // // Function to log order, trade, and cancellation details for compliance and audit
    // void logTradeData(const Order& order, const std::string& status);
    //
    // // Smart Order Routing (SOR) - Function to optimize order routing
    // void optimizeOrderRouting(const Order& order);
    //
    // // Function to maintain audit trails for compliance
    // void auditTrail(const Order& order, const std::string& action);
    //
    // // Real-time risk management function
    // void performRealTimeRiskCheck(const Order& order);
    //
    // // Mass cancel all active orders (kill switch)
    // void cancelAllOrders();
    //
    // // Handle execution reports (update order status, send to trading engine)
    // void handleExecutionReport(const std::string& orderId, const std::string& status, double filledQuantity, double remainingQuantity);
    //
    // // Notify Trading Engine of portfolio changes
    // void notifyTEPortfolioUpdate();

private:
    // Map to store multiple adapters for different brokers
    std::map<std::string, GenericAdapter*> adapters;

    // Portfolio details: instrument -> quantity
    // std::map<std::string, double> portfolio;

    // PnL details: instrument -> PnL value
    // std::map<std::string, double> pnl;

    // Risk management parameters
    // double maxExposure;
    // double maxLoss;

    // Helper function to route orders to the correct adapter
    // void routeOrderToAdapter(const Order& order, Adapter* adapter);

    // Map to track order status
    std::map<std::string, Order> activeOrders;

    // Function to update order status (e.g., Partially Filled, Filled, Cancelled)
    // void updateOrderStatus(const std::string& orderId, const std::string& status);
};

#endif // ORDERMANAGEMENTSYSTEM_H
