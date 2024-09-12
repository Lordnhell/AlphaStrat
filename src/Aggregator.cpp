//
// Created by Admin on 3/9/2024.
//
#include "../include/Aggregator.h"

Aggregator::Aggregator() {}

void Aggregator::addOrderBookData(const std::string& data) {
    orderBooks.push_back(data);
}

std::string Aggregator::getAggregatedData() {
    std::string aggregatedData;
    for (const auto& book : orderBooks) {
        aggregatedData += book + "\n";
    }
    return aggregatedData;
}
