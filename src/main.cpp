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
// #include "../include/solace/solaceLib.h"
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

    // Initialize Solace API with default logging.
    solClient_initialize(SOLCLIENT_LOG_DEFAULT_FILTER, nullptr);

    // Create a Solace context.
    solClient_context_createFuncInfo_t ctxInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;
    solClient_opaqueContext_pt context = nullptr;
    solClient_returnCode_t rc = solClient_context_create(SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD, &context, &ctxInfo, sizeof(ctxInfo));
    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to create Solace context: " << solClient_returnCodeToString(rc) << std::endl;
        return 1;
    }

    // Session properties for connecting to Solace PubSub.
    const char* host = "tcp://localhost:55554";
    const char* vpnName = "default";
    const char* username = "admin";
    const char* password = "admin";

    const char* sessionProps[] = {
        SOLCLIENT_SESSION_PROP_HOST, host,
        SOLCLIENT_SESSION_PROP_VPN_NAME, vpnName,
        SOLCLIENT_SESSION_PROP_USERNAME, username,
        SOLCLIENT_SESSION_PROP_PASSWORD, password,
        nullptr
    };

    // Create a Solace session.
    solClient_opaqueSession_pt session = nullptr;
    solClient_session_createFuncInfo_t sessionInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;
    sessionInfo.rxMsgInfo.callback_p = nullptr;
    sessionInfo.eventInfo.callback_p = nullptr;

    rc = solClient_session_create(const_cast<solClient_propertyArray_pt>(sessionProps), context, &session, &sessionInfo, sizeof(sessionInfo));
    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to create Solace session: " << solClient_returnCodeToString(rc) << std::endl;
        solClient_cleanup();
        return 1;
    }

    // Connect the Solace session.
    rc = solClient_session_connect(session);
    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to connect Solace session: " << solClient_returnCodeToString(rc) << std::endl;
        solClient_cleanup();
        return 1;
    }

    // Allocate a Solace message.
    solClient_opaqueMsg_pt message = nullptr;
    rc = solClient_msg_alloc(&message);
    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to allocate Solace message: " << solClient_returnCodeToString(rc) << std::endl;
        solClient_cleanup();
        return 1;
    }

    // Set the message delivery mode to direct.
    solClient_msg_setDeliveryMode(message, SOLCLIENT_DELIVERY_MODE_DIRECT);

    // Set the message payload.
    const char* payload = "hello solace";
    solClient_msg_setBinaryAttachmentPtr(message, (void*)payload, strlen(payload));

    // Set the message destination to a topic.
    solClient_destination_t destination;
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = "tutorial/topic";
    solClient_msg_setDestination(message, &destination, sizeof(destination));

    // Send the message.
    rc = solClient_session_sendMsg(session, message);
    if (rc == SOLCLIENT_OK) {
        std::cout << "Message published successfully!" << std::endl;
    } else {
        std::cerr << "Failed to publish message: " << solClient_returnCodeToString(rc) << std::endl;
    }

    // Free the message and clean up.
    solClient_msg_free(&message);
    solClient_session_disconnect(session);
    solClient_context_destroy(&context);
    solClient_cleanup();

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

    std::vector<std::string> configFiles = {
        "../config/config.json"
    };

    std::vector<std::vector<std::string>> tickersList = {
        {"AAPL", "GOOG"}
    };

    std::vector<std::thread> threads;
    // User selects mode
    bool testMode = true;
    cout << "Running in " << (testMode ? "Test Mode" : "Paper Trading Mode") << endl;

    // Create and start adapters in separate threads
    for (size_t i = 0; i < configFiles.size(); ++i) {
        // Start each adapter in a separate thread
        threads.emplace_back(runAdapter, configFiles[i], tickersList[i], testMode);
    }

    // Wait for all threads to finish (join them)
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    // AlpacaAdapter alpaca_adapter("../config/config.json");
    // // alpaca_adapter.initialize("../config/config.json");
    //
    // string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;
    //
    // cout << "" << endl;
    // cout << response2 << endl;

    return 0;
}