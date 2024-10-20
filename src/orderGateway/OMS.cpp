//
// Created by Admin on 20/10/2024.
//

#include "../include/orderGatway/OMS.h"
#include "../include/Adapters/AlpacaAdapter.h"

// Migrate to genereic adapter
void OrderManagementSystem::initializeAdapters() {
    // Initialize AlpacaAdapter
    std::string alpacaConfigFile = "../config/config.json";  // Example config file
    AlpacaAdapter* alpacaAdapter = new AlpacaAdapter(alpacaConfigFile); //Config will be loaded as constructor of adapter
    // alpacaAdapter->initialize(alpacaConfigFile);  // Initialize the Alpaca adapter
    adapters["Alpaca"] = alpacaAdapter;

    std::cout << "Adapters registered with OMS." << std::endl;
}


