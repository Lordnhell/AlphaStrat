#include <winsock2.h>
#include <solClient.h>
#include <solClientMsg.h>
#include <iostream>
#include <fstream>

//#include "../include/Aggregator.h"
//#include "../include/MarketDataGateway.h"
//#include "../include/OrderGateway.h"
//#include "../include/OrderManagementSystem.h"
//#include "../include/TradingEngine.h"
#include <nlohmann/json.hpp>
#include "../include/MarketDataGateway/MarketDataGateway.h"
#include "../include/solace/solaceLib.h"
#include "Adapters/AlpacaAdapter.h"

using namespace std;

// Signal handler
AlpacaAdapter* adapter_ptr = nullptr;

void signalHandler(int signal) {
    std::cout << "Interrupt signal (" << signal << ") received. Performing graceful shutdown..." << std::endl;
    if (adapter_ptr) {
        adapter_ptr->gracefulDisconnect();
    }
    exit(signal);
}

// Function to run an adapter and subscribe to live data
void runAdapter(const std::string& configFile, const std::vector<std::string>& tickers, bool testMode) {
    AlpacaAdapter adapter(configFile);
    adapter.subscribeLiveData(tickers, testMode);
}

int main() {

    //add thread pool for handling many adapters, doing a manual approach for now
    //(alpaca only allows one subscription (free), this will be used for other adapters)

    try {
        // Create an instance of solaceLib with the path to your config file
        solaceLib solaceClient("../config/config.json");

        // Define the topic
        std::string topic = "example/topic";

        // Subscribe to the topic
        solaceClient.subscribeToTopic(topic);

        // Run a loop to continuously publish messages and receive them
        int messageCount = 0;
        nlohmann::json jsonObj = {
            {"key1", "value1"},
            {"key2", 42},
            {"key3", true}
        };

        // Convert the JSON object to a string
        std::string jsonString = jsonObj.dump();

        while (messageCount < 100) {  // You can adjust the condition for the loop
            // Create a new message
            std::string message = "Hello from Solace! Message #" + std::to_string(messageCount);

            // Publish the message
            solaceClient.publishMessage(topic, message);

            // Simulate a delay between message sends (e.g., 1 second)
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Increment the message count
            messageCount++;
        }

        std::cout << "Finished sending messages." << std::endl;

        // Keep the program running to receive messages (optional)
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // Register signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Example for running multiples threads
    // std::vector<std::string> configFiles = {
    //     "../config/config.json",
    //     "../config/config.json"// Add as many as you want
    // };
    //
    // std::vector<std::vector<std::string>> tickersList = {
    //     {"AAPL", "GOOG"},
    //     {"MSFT", "TSLA"}
    // };

    // std::vector<std::string> configFiles = {
    //     "../config/config.json"
    // };
    //
    // std::vector<std::vector<std::string>> tickersList = {
    //     {"AAPL", "GOOG"}
    // };
    //
    // std::vector<std::thread> threads;
    // // User selects mode
    // bool testMode = true;
    // cout << "Running in " << (testMode ? "Test Mode" : "Paper Trading Mode") << endl;
    //
    // // Create and start adapters in separate threads
    // for (size_t i = 0; i < configFiles.size(); ++i) {
    //     // Start each adapter in a separate thread
    //     threads.emplace_back(runAdapter, configFiles[i], tickersList[i], testMode);
    // }
    //
    // // Wait for all threads to finish (join them)
    // for (auto& th : threads) {
    //     if (th.joinable()) {
    //         th.join();
    //     }
    // }

    // AlpacaAdapter alpaca_adapter("../config/config.json");
    // // alpaca_adapter.initialize("../config/config.json");
    //
    // string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;
    //
    // cout << "" << endl;
    // cout << response2 << endl;

    return 0;
}