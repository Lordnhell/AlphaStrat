//
// Created by Admin on 1/10/2024.
//

#ifndef GENERICADAPTER_H
#define GENERICADAPTER_H

#include <string>

class GenericAdapter {
public:
    virtual ~GenericAdapter() = default;
    virtual void initialize(const std::string& configFile) = 0;
    virtual std::string getLatestTick (const std::string& symbol,const std::string& feed) = 0;
};


#endif //GENERICADAPTER_H