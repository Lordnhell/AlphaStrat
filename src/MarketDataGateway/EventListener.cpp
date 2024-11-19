//
// Created by Admin on 13/11/2024.
//

#include "EventListener.h"

// Constructor that initializes Solace session and subscribes to topics
EventListener::EventListener(const std::string& configFilePath) : solaceClient(configFilePath) {
    loadConfig(configFilePath);
    subscribeToTopics();
}

// Start listening for incoming messages
void EventListener::startListening() {
    std::thread listeningThread([this]() {
        while (true) {
            // Keep the program running to receive messages
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    listeningThread.detach();
}

// Load config to retrieve the topics to subscribe to
void EventListener::loadConfig(const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configFilePath);
    }

    nlohmann::json configJson;
    configFile >> configJson;

    for (const auto& topic : configJson["topics"]) {
        topics.push_back(topic.get<std::string>());
    }
    std::cout << "Topics loaded successfully." << std::endl;
}

// Subscribe to each topic specified in the config
void EventListener::subscribeToTopics() {
    for (const auto& topic : topics) {
        solaceClient.subscribeToTopic(topic);
        std::cout << "Subscribed to topic: " << topic << std::endl;
    }
}