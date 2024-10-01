// Created by Admin on 1/10/2024.
//

#ifndef ALPACAADAPTER_H
#define ALPACAADAPTER_H
#include <string>
#include "../interface/GenericAdapter.h"

class AlpacaAdapter : public GenericAdapter {
public:
    void initialize(const std::string& configFile) override;
    std::string getLatestTick(const std::string &symbol, const std::string &feed) override;

private:
    std::string apiKey;
    std::string secretKey;
    std::string baseUrl;
    std::string dataUrl;

    void loadConfig(const std::string& configFile);
    std::string performRequest(const std::string& url);
};


#endif //ALPACAADAPTER_H