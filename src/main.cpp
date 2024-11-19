#include <winsock2.h>
#include <solClient.h>
#include <solClientMsg.h>
#include <iostream>
#include <fstream>

//#include "../include/Aggregator.h"
//#include "../include/MarketDataGateway.h"
//#include "../include/OrderGateway.h"
#include "../include/orderGateway/OMS.h"
//#include "../include/TradingEngine.h"
#include <nlohmann/json.hpp>
#include "../include/MarketDataGateway/MarketDataGateway.h"
#include "../include/solace/solaceLib.h"
#include "Adapters/AlpacaAdapter.h"
#include "orderGateway/Order.h"
#include "interface/AdapterEnum.h"
#include "../src/MarketDataGateway/EventListener.h"

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

    try {
        // Create EventListener objects and start listening
        EventListener liveEventListener("../config/config.json");
        EventListener historicalEventListener("../config/config.json");
        EventListener orderEventListener("../config/config.json");

        liveEventListener.startListening();
        historicalEventListener.startListening();
        orderEventListener.startListening();

        solaceLib solaceClient("../config/config.json");

        // Setup subscriber threads for each topic
        std::thread liveSubscriberThread([&]() {
            solaceClient.subscribeToTopic("marketdata/request/live");
        });

        std::thread historicalSubscriberThread([&]() {
            solaceClient.subscribeToTopic("marketdata/request/historical");
        });

        std::thread orderSubscriberThread([&]() {
            solaceClient.subscribeToTopic("order/request");
        });

        // Start threads
        liveSubscriberThread.detach();
        historicalSubscriberThread.detach();
        orderSubscriberThread.detach();

        // Loop to continuously publish messages to the subscribed topics
        int messageCount = 0;
        while (messageCount < 100) {
            // Create test messages
            std::string testLiveMessage = "{\"event\": \"live_data\", \"message\": \"Live data sample " + std::to_string(messageCount) + "\"}";
            std::string testHistoricalMessage = "{\"event\": \"historical_data\", \"message\": \"Historical data sample " + std::to_string(messageCount) + "\"}";
            std::string testOrderMessage = "{\"event\": \"order_request\", \"message\": \"Order request sample " + std::to_string(messageCount) + "\"}";

            // Publish the messages
            solaceClient.publishMessage("marketdata/request/live", testLiveMessage);
            solaceClient.publishMessage("marketdata/request/historical", testHistoricalMessage);
            solaceClient.publishMessage("order/request", testOrderMessage);

            std::cout << "Published messages with count: " << messageCount << std::endl;

            // Wait for a short period before sending the next set of messages
            std::this_thread::sleep_for(std::chrono::seconds(1));
            messageCount++;
        }

        // Keep the program running to receive messages (subscriber)
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    // OrderManagementSystem oms;
    // oms.initializeAdapters();
    // Order example_order("AlpacaOrder001",
    //     "AAPL",
    //     Order::Side::Buy,
    //     100,
    //     Order::OrderType::Limit,
    //     Order::TimeInForce::Day,
    //     "NYSE");
    //
    // // Setting optional parameters
    // example_order.setLimitPrice(145.50);
    // // example_order.setClientOrderID("AlpacaOrder001");
    // example_order.setExtendedHours(true);
    //
    // oms.submitOrder(example_order, AdapterEnum::Alpaca);

    // unsigned int numThreads = std::thread::hardware_concurrency();
    // std::cout << "Number of hardware threads: " << numThreads << std::endl;

    /*
      eventlistener{
        listen for order from solace
        have an on message router (example, cld be sending order or cancelling order get pnl ETC)
        deserialize orders back order object using boost object
        send to create order method
        inside create order method, call order router method
        order router wil route according to nature

    }/*

    /*
This block of code demonstrates the use of the solaceLib client for connecting to a Solace messaging platform, subscribing to a specific topic, and publishing JSON-encoded messages to that topic.

The solaceLib instance is initialized using a configuration file path, and the client subscribes to the topic `"example/topic"`.

Inside a loop, it publishes 100 messages, each prefixed with "Hello from Solace!" and an incremented message count. After sending each message, it simulates a delay of 1 second. Once the loop is complete, the program continues running to allow receiving messages.

If any exception occurs during execution, it is caught, and the error is printed to the console.
*/

    // try {
    //     // Create an instance of solaceLib with the path to your config file
    //     solaceLib solaceClient("../config/config.json");
    //
    //     // Define the topic
    //     std::string topic = "example/topic";
    //
    //     // Subscribe to the topic
    //     solaceClient.subscribeToTopic(topic);
    //
    //     // Run a loop to continuously publish messages and receive them
    //     int messageCount = 0;
    //     nlohmann::json jsonObj = {
    //         {"key1", "value1"},
    //         {"key2", 42},
    //         {"key3", true}
    //     };
    //
    //     // Convert the JSON object to a string
    //     std::string jsonString = jsonObj.dump();
    //
    //     while (messageCount < 100) {  // You can adjust the condition for the loop
    //         // Create a new message
    //         std::string message = "Hello from Solace! Message #" + std::to_string(messageCount);
    //
    //         // Publish the message
    //         solaceClient.publishMessage(topic, message);
    //
    //         // Simulate a delay between message sends (e.g., 1 second)
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //
    //         // Increment the message count
    //         messageCount++;
    //     }
    //
    //     std::cout << "Finished sending messages." << std::endl;
    //
    //     // Keep the program running to receive messages (optional)
    //     while (true) {
    //         std::this_thread::sleep_for(std::chrono::seconds(10));
    //     }
    //
    // } catch (const std::exception &e) {
    //     std::cerr << "Exception: " << e.what() << std::endl;
    // }

    /*
 Code demonstrates how to run multiple threads, each initializing and running a
 separate adapter for a set of tickers.The configuration files and tickers for
 each thread are stored in vectors. Based on a user-defined mode
 (test mode or paper trading mode), each thread starts an adapter instance
 with its respective configuration and ticker list. The threads are then joined to
 ensure the main program waits for all threads to finish execution.
 Additionally, an example is shown using an AlpacaAdapter to fetch the latest
 stock tick for "TSLA" from the "iex" data source.

    //add thread pool for handling many adapters, doing a manual approach for now
    //(alpaca only allows one subscription (free), this will be used for other adapters)
*/

    // // Register signal handler
    // signal(SIGINT, signalHandler);
    // signal(SIGTERM, signalHandler);

    // // Example for running multiples threads
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
    // alpaca_adapter.initialize("../config/config.json");
    // //
    // string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;
    // //
    // cout << "" << endl;
    // cout << response2 << endl;

    return 0;
}