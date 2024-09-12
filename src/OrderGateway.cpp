//
// Created by Admin on 3/9/2024.
//
#include "../include/OrderGateway.h"
#include <iostream>

OrderGateway::OrderGateway() {}

void OrderGateway::sendOrderToExchange(const std::string& order) {
    std::cout << "Sending order to exchange: " << order << std::endl;
}

void OrderGateway::receiveExecutionReport(const std::string& report) {
    std::cout << "Execution report received: " << report << std::endl;
}

void OrderGateway::implementCircuitBreaker() {
    std::cout << "Implementing circuit breaker" << std::endl;
}
