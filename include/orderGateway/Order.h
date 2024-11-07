//
// Created by Admin on 20/10/2024.
//

#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <optional>

class Order {
public:
    // Enumerations for validation of acceptable values
    enum class Side { Buy, Sell };
    enum class OrderType { Market, Limit, Stop, StopLimit, TrailingStop };
    enum class TimeInForce { Day, GTC, OPG, CLS, IOC, FOK };

    // Constructor
    Order(const std::string& clientOrderID,
        const std::string& instrument,
        Side side, double quantity,
        OrderType type,
        TimeInForce timeInForce,
        const std::string& exchange);

    // Getters
    std::string getInstrument() const;
    std::string getOrderType() const;
    std::string getSide() const;
    double getQuantity() const;
    std::string getExchange() const;
    std::string getTimeInForce() const;
    std::optional<double> getLimitPrice() const;
    std::optional<double> getStopPrice() const;
    std::optional<double> getTrailPrice() const;
    std::optional<double> getTrailPercent() const;
    bool isExtendedHours() const;
    std::string getClientOrderID() const;
    std::string getOrderClass() const;

    // Setters (for optional fields)
    void setLimitPrice(double price);
    void setStopPrice(double price);
    void setTrailPrice(double price);
    void setTrailPercent(double percent);
    void setExtendedHours(bool extended);
    // void setClientOrderID(const std::string& orderId);
    void setOrderClass(const std::string& orderClass);

    // Validation function (static to avoid object creation for validation)
    static bool isValidOrderType(const OrderType type);
    static bool isValidSide(const Side side);
    static bool isValidTimeInForce(const TimeInForce timeInForce);

private:
    std::string clientOrderID;        // client order ID
    std::string instrument;
    Side side;                       // Buy, Sell
    double quantity;                 // Required, number of shares
    OrderType type;                  // Required, market, limit, stop, stop_limit, trailing_stop
    TimeInForce timeInForce;         // Required, e.g., day, gtc, opg, cls, ioc, fok
    std::string exchange;            // Exchange, e.g., "NYSE"

    // Optional fields
    std::optional<double> limitPrice;  // Only for limit and stop_limit orders
    std::optional<double> stopPrice;   // Only for stop or stop_limit orders
    std::optional<double> trailPrice;  // Only for trailing_stop orders
    std::optional<double> trailPercent;  // Only for trailing_stop orders
    bool extendedHours = false;       // Default is false

    std::string orderClass;           // Optional, default empty: "", bracket, oco, oto
};

#endif // ORDER_H

