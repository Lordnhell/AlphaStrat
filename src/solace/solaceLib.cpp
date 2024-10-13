// // SolaceLib.cpp
// #include <winsock2.h>
// #include "../include/solace/solaceLib.h"
// #include <iostream>
// #include <solClient.h>
//
// SolaceLib::SolaceLib() : session(nullptr) {
//     solClient_initialize(SOLCLIENT_LOG_DEFAULT_FILTER, nullptr);
// }
//
// SolaceLib::~SolaceLib() {
//     if (session) {
//         solClient_session_disconnect(session);
//         solClient_session_destroy(&session);
//     }
//     solClient_cleanup();
// }
//
// bool SolaceLib::connect(const std::string& brokerUrl, const std::string& clientId, const std::string& username, const std::string& password) {
//     try {
//         setupSession(brokerUrl, clientId, username, password);
//         solClient_session_connect(session);
//         std::cout << "Connected to Solace broker at " << brokerUrl << std::endl;
//         return true;
//     } catch (const std::exception& e) {
//         std::cerr << "Connection failed: " << e.what() << std::endl;
//         return false;
//     }
// }
//
// void SolaceLib::setupSession(const std::string& brokerUrl,
//                              const std::string& clientId,
//                              const std::string& username,
//                              const std::string& password) {
//     solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;
//     solClient_opaqueContext_pt context;
//     solClient_context_create(SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD, &context, &contextFuncInfo, sizeof(contextFuncInfo));
//
//     solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;
//     sessionFuncInfo.rxMsgInfo.callback_p = [](solClient_opaqueSession_pt, solClient_opaqueMsg_pt msg, void*) -> solClient_rxMsgCallback_returnCode_t {
//         solClient_msg_dump(msg, nullptr, 0);
//         return SOLCLIENT_CALLBACK_OK;
//     };
//     sessionFuncInfo.eventInfo.callback_p = [](solClient_opaqueSession_pt, solClient_session_eventCallbackInfo_pt eventInfo, void*) {
//         std::cout << "Session Event: " << solClient_session_eventToString(eventInfo->sessionEvent) << std::endl;
//     };
//
//     const char* sessionProps[] = {
//         SOLCLIENT_SESSION_PROP_HOST, brokerUrl.c_str(),
//         SOLCLIENT_SESSION_PROP_VPN_NAME, "default",
//         SOLCLIENT_SESSION_PROP_USERNAME, username.c_str(),
//         SOLCLIENT_SESSION_PROP_PASSWORD, password.c_str(),
//         nullptr
//     };
//
//     solClient_returnCode_t rc = solClient_session_create(
//         const_cast<char**>(sessionProps),
//         context,
//         &session,
//         &sessionFuncInfo,
//         sizeof(sessionFuncInfo)
//     );
//     if (rc != SOLCLIENT_OK) {
//         throw std::runtime_error("Failed to create Solace session");
//     }
// }
//
// bool SolaceLib::publish(const std::string& topic, const std::string& message) {
//     solClient_opaqueMsg_pt msg;
//     solClient_msg_alloc(&msg);
//     solClient_msg_setDeliveryMode(msg, SOLCLIENT_DELIVERY_MODE_DIRECT);
//
//     solClient_destination_t destination = { SOLCLIENT_TOPIC_DESTINATION, topic.c_str() };
//     solClient_msg_setDestination(msg, &destination);
//     solClient_msg_setBinaryAttachmentString(msg, message.c_str());
//
//     solClient_returnCode_t rc = solClient_session_sendMsg(session, msg);
//     solClient_msg_free(&msg);
//
//     if (rc == SOLCLIENT_OK) {
//         std::cout << "Message published to " << topic << ": " << message << std::endl;
//         return true;
//     } else {
//         std::cerr << "Failed to publish message to " << topic << std::endl;
//         return false;
//     }
// }
//
// bool SolaceLib::subscribe(const std::string& topic, std::function<void(const std::string&)> messageHandler) {
//     solClient_returnCode_t rc = solClient_session_topicSubscribeExt(session, SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM, topic.c_str());
//     if (rc != SOLCLIENT_OK) {
//         std::cerr << "Failed to subscribe to topic: " << topic << std::endl;
//         return false;
//     }
//
//     std::cout << "Subscribed to topic: " << topic << std::endl;
//
//     // The message processing would be handled in the callback defined in setupSession.
//     return true;
// }
//
// void SolaceLib::disconnect() {
//     solClient_session_disconnect(session);
//     std::cout << "Disconnected from Solace broker." << std::endl;
// }
