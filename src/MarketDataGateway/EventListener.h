#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include "../include/solace/solaceLib.h"
#include <thread>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class EventListener {
public:
    // Enum for event listener types
    enum class Type {
        Live,
        Historical,
        Order
    };

    // Constructor
    EventListener(const std::string& configFilePath, Type type);

    // Start listening for messages in a separate thread
    void startListening();

private:
    solaceLib solaceClient;               // Instance of solaceLib
    std::vector<std::string> topics;      // Topics to subscribe to
    std::thread listenerThread;           // Thread for listening

    // Load topics from the config JSON based on listener type
    void loadTopics(const std::string& configFilePath, Type type);

    // Callback function for processing received messages
    static void messageCallback(const std::string& topic, const std::string& message);
};

#endif // EVENTLISTENER_H
