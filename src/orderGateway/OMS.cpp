//
// Created by Admin on 20/10/2024.
//

#include "../include/orderGateway/OMS.h"
#include "../include/Adapters/AlpacaAdapter.h"


// Initialize available adapters
void OrderManagementSystem::initializeAdapters() {
    // Initialize AlpacaAdapter
    std::string alpacaConfigFile = "../config/config.json";
    AlpacaAdapter* alpacaAdapter = new AlpacaAdapter(alpacaConfigFile);
    OMSadapters[AdapterEnum::Alpaca] = alpacaAdapter;  // Store with enum key

    // // Initialize IBKRAdapter (example)
    // std::string ibkrConfigFile = "../config/ibkr_config.json";
    // IBKRAdapter* ibkrAdapter = new IBKRAdapter(ibkrConfigFile);
    // OMSadapters[AdapterEnum::IBKR] = ibkrAdapter;  // Store with enum key

    std::cout << "Adapters registered with OMS." << std::endl;
}

// Helper method to get the correct adapter based on the AdapterEnum
GenericAdapter* OrderManagementSystem::getAdapter(AdapterEnum adapterType) {
    auto it = OMSadapters.find(adapterType);  // Find by AdapterEnum key
    if (it != OMSadapters.end()) {
        return it->second;  // Return the found adapter
    } else {
        std::cerr << "Error: Adapter not found for the given type!" << std::endl;
        return nullptr;
    }
}

// Submit an order through the selected adapter
void OrderManagementSystem::submitOrder(const Order& order, AdapterEnum adapterType) {
    std::cout << "OMS submit order called" << std::endl;

    // Use the getAdapter method to get the appropriate adapter
    GenericAdapter* adapter = getAdapter(adapterType);
    if (adapter) {
        adapter->createOrder(order);  // Call createOrder on the adapter
        std::cout << "Order submitted successfully." << std::endl;
    } else {
        std::cerr << "Error: Could not submit order, adapter not found or invalid." << std::endl;
    }
}
