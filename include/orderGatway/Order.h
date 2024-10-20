//
// Created by Admin on 20/10/2024.
//

#ifndef ORDER_H
#define ORDER_H

#include <string>

class Order {
public:
    // Constructor
    Order(const std::string& instrument, const std::string& orderType, double quantity, double price, const std::string& exchange);

    // Getters
    std::string getOrderID() const;
    std::string getInstrument() const;
    std::string getOrderType() const;
    double getQuantity() const;
    double getPrice() const;
    std::string getStatus() const;
    std::string getExchange() const;
    std::string getStrategyID() const;
    std::string getTimeInForce() const;

    // Setters
    void setStatus(const std::string& status);
    void setFilledQuantity(double filledQuantity);
    void setOrderID(const std::string& orderId);

private:
    std::string orderID;
    std::string instrument;  // e.g., "AAPL"
    std::string orderType;   // "Buy", "Sell", etc.
    double quantity;         // Number of shares/contracts
    double price;            // Price for limit orders
    std::string status;      // "New", "Filled", "Cancelled"
    std::string exchange;    // e.g., "NYSE"
    std::string strategyID;  // ID of the originating strategy
    std::string timeInForce; // e.g., "GTC", "IOC"
    double filledQuantity;   // Tracks how much has been filled
};

#endif // ORDER_H
