//
// Created by Admin on 13/11/2024.
//

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include "../include/solace/solaceLib.h"  // Assuming solaceLib is your wrapper for Solace messaging
#include <thread>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

// EventListener class handles both subscribing and reacting to messages received
class EventListener {
public:
    // Constructor that initializes Solace session and subscribes to topics
    EventListener(const std::string& configFilePath);

    // Start listening for incoming messages
    void startListening();

private:
    solaceLib solaceClient;
    std::vector<std::string> topics;

    // Load config to retrieve the topics to subscribe to
    void loadConfig(const std::string& configFilePath);

    // Subscribe to each topic specified in the config
    void subscribeToTopics();
};

#endif // EVENTLISTENER_H