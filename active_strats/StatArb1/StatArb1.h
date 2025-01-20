#ifndef STATARB1_H
#define STATARB1_H

#include "../src/strategy/Strategy.h"

class StatArb1 : public Strategy {
public:
    StatArb1(const std::string& solaceConfig);
    void run() override;

protected:
    void onLiveUpdate(const nlohmann::json& data) override;
};

#endif // STATARB1_H