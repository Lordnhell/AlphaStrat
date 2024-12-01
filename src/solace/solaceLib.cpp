#include "../include/solace/solaceLib.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

solaceLib::solaceLib(const std::string& configFilePath) {
    // Initialize Solace API
    solClient_initialize(SOLCLIENT_LOG_DEFAULT_FILTER, NULL);

    // Load configuration from the provided config file
    loadConfig(configFilePath);

    // Create and connect the Solace session
    createSession();
}

solaceLib::~solaceLib() {
    // Clean up the Solace session and context
    if (session) {
        solClient_session_disconnect(session);
        solClient_session_destroy(&session);
    }
    if (context) {
        solClient_context_destroy(&context);
    }
    solClient_cleanup();
}

void solaceLib::loadConfig(const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configFilePath);
    }

    json configJson;
    configFile >> configJson;

    // Navigate through the nested structure to load Solace configurations
    host = configJson["solace"]["solaceLib"]["host"].get<std::string>();
    vpn = configJson["solace"]["solaceLib"]["vpn"].get<std::string>();
    username = configJson["solace"]["solaceLib"]["username"].get<std::string>();
    password = configJson["solace"]["solaceLib"]["password"].get<std::string>();
    trustStoreDir = configJson["solace"]["solaceLib"]["trustStoreDir"].get<std::string>();
}

void solaceLib::createSession() {
    // Define session properties (connection details from loaded config)
    const char* sessionProps[] = {
        SOLCLIENT_SESSION_PROP_HOST, host.c_str(),
        SOLCLIENT_SESSION_PROP_VPN_NAME, vpn.c_str(),
        SOLCLIENT_SESSION_PROP_USERNAME, username.c_str(),
        SOLCLIENT_SESSION_PROP_PASSWORD, password.c_str(),
        SOLCLIENT_SESSION_PROP_SSL_TRUST_STORE_DIR, trustStoreDir.c_str(),
        SOLCLIENT_SESSION_PROP_CONNECT_BLOCKING, SOLCLIENT_PROP_ENABLE_VAL,
        NULL
    };

    // Initialize the context function information
    solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

    // Create a context with function info
    solClient_returnCode_t rc = solClient_context_create(SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD, &context, &contextFuncInfo, sizeof(contextFuncInfo));

    if (rc != SOLCLIENT_OK) {
        throw std::runtime_error("Failed to create Solace context: " + std::string(solClient_returnCodeToString(rc)));
    }

    // Set up session callback information
    solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;
    sessionFuncInfo.rxMsgInfo.callback_p = messageReceiveCallback;
    sessionFuncInfo.rxMsgInfo.user_p = this;
    sessionFuncInfo.eventInfo.callback_p = sessionEventCallback;  // This will be corrected to return void
    sessionFuncInfo.eventInfo.user_p = this;

    // Cast sessionProps to match the expected property array type
    rc = solClient_session_create((solClient_propertyArray_pt)sessionProps, context, &session, &sessionFuncInfo, sizeof(sessionFuncInfo));

    if (rc != SOLCLIENT_OK) {
        throw std::runtime_error("Failed to create Solace session: " + std::string(solClient_returnCodeToString(rc)));
    }

    // Connect the session
    rc = solClient_session_connect(session);
    if (rc != SOLCLIENT_OK) {
        throw std::runtime_error("Failed to connect to Solace broker.");
    }
}


void solaceLib::publishMessage(const std::string& topic, const std::string& message) {
    // Create a message
    solClient_opaqueMsg_pt msg;
    solClient_msg_alloc(&msg);
    solClient_msg_setDeliveryMode(msg, SOLCLIENT_DELIVERY_MODE_DIRECT);
    solClient_msg_setBinaryAttachmentString(msg, message.c_str());

    // Set the destination (topic)
    solClient_destination_t destination;
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = topic.c_str();
    solClient_msg_setDestination(msg, &destination, sizeof(destination));

    // Publish the message
    solClient_returnCode_t rc = solClient_session_sendMsg(session, msg);
    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to send message." << std::endl;
    } else {
        std::cout << "Message sent successfully to topic: " << topic << std::endl;
    }

    // Cleanup
    solClient_msg_free(&msg);
}

void solaceLib::subscribeToTopic(const std::string& topic) {
    solClient_returnCode_t rc = solClient_session_topicSubscribeExt(session, SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM, topic.c_str());
    if (rc != SOLCLIENT_OK) {
        throw std::runtime_error("Failed to subscribe to topic: " + topic);
    }
    std::cout << "Subscribed to topic: " << topic << std::endl;
}

// Callback modification
solClient_rxMsgCallback_returnCode_t solaceLib::messageReceiveCallback(solClient_opaqueSession_pt session_p, solClient_opaqueMsg_pt msg_p, void* user_p) {
    void* msgText;
    solClient_uint32_t msgSize;
    solClient_msg_getBinaryAttachmentPtr(msg_p, (solClient_opaquePointer_pt)&msgText, &msgSize);
    std::string message((char*)msgText, msgSize);

    std::cout << "Received message: " << message << std::endl;
    return SOLCLIENT_CALLBACK_OK;
}

// Fix: Changed the return type of sessionEventCallback to void
void solaceLib::sessionEventCallback(solClient_opaqueSession_pt session_p, solClient_session_eventCallbackInfo_pt eventInfo_p, void* user_p) {
    std::cout << "Received session event: " << solClient_session_eventToString(eventInfo_p->sessionEvent) << std::endl;
}