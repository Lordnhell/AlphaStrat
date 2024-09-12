//
// Created by Admin on 3/9/2024.
//

#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <vector>
#include <string>

class Aggregator {
public:
    Aggregator();
    void addOrderBookData(const std::string& data);
    std::string getAggregatedData();

private:
    std::vector<std::string> orderBooks;
};

#endif // AGGREGATOR_H

