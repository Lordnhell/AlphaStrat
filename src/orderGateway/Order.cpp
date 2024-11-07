#include "../include/orderGateway/Order.h"
#include <iostream>
#include <stdexcept>
#include <map>

// Constructor implementation
Order::Order(const std::string& clientOrderID,
    const std::string& instrument,
    Side side, double quantity,
    OrderType type,
    TimeInForce timeInForce,
    const std::string& exchange)
    : clientOrderID(clientOrderID),
instrument(instrument),
side(side),
quantity(quantity),
type(type),
timeInForce(timeInForce),
exchange(exchange) {

    // Validation for required parameters
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be greater than zero.");
    }
    if (!isValidOrderType(type)) {
        throw std::invalid_argument("Invalid order type.");
    }
    if (!isValidSide(side)) {
        throw std::invalid_argument("Invalid side.");
    }
    if (!isValidTimeInForce(timeInForce)) {
        throw std::invalid_argument("Invalid time-in-force.");
    }
}

// Getters
std::string Order::getInstrument() const {
    return instrument;
}

std::string Order::getSide() const {
    return side == Side::Buy ? "buy" : "sell";
}

std::string Order::getOrderType() const {
    static std::map<OrderType, std::string> orderTypeMap = {
        { OrderType::Market, "market" },
        { OrderType::Limit, "limit" },
        { OrderType::Stop, "stop" },
        { OrderType::StopLimit, "stop_limit" },
        { OrderType::TrailingStop, "trailing_stop" }
    };
    return orderTypeMap[type];
}

double Order::getQuantity() const {
    return quantity;
}

std::string Order::getExchange() const {
    return exchange;
}

std::string Order::getTimeInForce() const {
    static std::map<TimeInForce, std::string> tifMap = {
        { TimeInForce::Day, "day" },
        { TimeInForce::GTC, "gtc" },
        { TimeInForce::OPG, "opg" },
        { TimeInForce::CLS, "cls" },
        { TimeInForce::IOC, "ioc" },
        { TimeInForce::FOK, "fok" }
    };
    return tifMap[timeInForce];
}

std::optional<double> Order::getLimitPrice() const {
    return limitPrice;
}

std::optional<double> Order::getStopPrice() const {
    return stopPrice;
}

std::optional<double> Order::getTrailPrice() const {
    return trailPrice;
}

std::optional<double> Order::getTrailPercent() const {
    return trailPercent;
}

bool Order::isExtendedHours() const {
    return extendedHours;
}

std::string Order::getClientOrderID() const {
    return clientOrderID;
}

std::string Order::getOrderClass() const {
    return orderClass;
}

// Setters for optional fields
void Order::setLimitPrice(double price) {
    limitPrice = price;
}

void Order::setStopPrice(double price) {
    stopPrice = price;
}

void Order::setTrailPrice(double price) {
    trailPrice = price;
}

void Order::setTrailPercent(double percent) {
    trailPercent = percent;
}

void Order::setExtendedHours(bool extended) {
    extendedHours = extended;
}

// void Order::setClientOrderID(const std::string& orderId) {
//     clientOrderID = orderId;
// }

void Order::setOrderClass(const std::string& orderClass) {
    this->orderClass = orderClass;
}

// Validation functions
bool Order::isValidOrderType(const OrderType type) {
    return type == OrderType::Market || type == OrderType::Limit || type == OrderType::Stop || type == OrderType::StopLimit || type == OrderType::TrailingStop;
}

bool Order::isValidSide(const Side side) {
    return side == Side::Buy || side == Side::Sell;
}

bool Order::isValidTimeInForce(const TimeInForce timeInForce) {
    return timeInForce == TimeInForce::Day || timeInForce == TimeInForce::GTC || timeInForce == TimeInForce::OPG || timeInForce == TimeInForce::CLS || timeInForce == TimeInForce::IOC || timeInForce == TimeInForce::FOK;
}
