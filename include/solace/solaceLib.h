#ifndef SOLACELIB_H
#define SOLACELIB_H

#include <winsock2.h>
#include <solClient.h>
#include <solClientMsg.h>
#include <string>
#include <nlohmann/json.hpp>

class solaceLib {
public:
    // Constructor
    solaceLib(const std::string& configFilePath);

    // Destructor
    ~solaceLib();

    // Method to publish a message to a topic
    void publishMessage(const std::string& topic, const std::string& message);

    // Method to subscribe to a topic
    void subscribeToTopic(const std::string& topic);

private:
    // Configuration loaded from config.json
    std::string host;
    std::string vpn;
    std::string username;
    std::string password;
    std::string trustStoreDir;

    // Solace session and context
    solClient_opaqueSession_pt session;
    solClient_opaqueContext_pt context;

    // Callback for receiving messages
    static solClient_rxMsgCallback_returnCode_t messageReceiveCallback(solClient_opaqueSession_pt session_p, solClient_opaqueMsg_pt msg_p, void *user_p);

    // Callback for session events (e.g., connection status)
    static void sessionEventCallback(solClient_opaqueSession_pt session_p, solClient_session_eventCallbackInfo_pt eventInfo_p, void *user_p);

    // Method to load configuration from JSON
    void loadConfig(const std::string& configFilePath);

    // Method to create and connect the Solace session
    void createSession();
};


#endif // SOLACELIB_H
