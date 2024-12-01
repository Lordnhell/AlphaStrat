#include "EventListener.h"
#include <fstream>
#include <iostream>

// Constructor
EventListener::EventListener(const std::string& configFilePath, Type type)
    : solaceClient("../config/config.json") {
    loadTopics(configFilePath, type);
}

// Load topics from the config file
void EventListener::loadTopics(const std::string& configFilePath, Type type) {
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configFilePath);
    }

    nlohmann::json configJson;
    file >> configJson;

    std::string key;
    switch (type) {
        case Type::Live: key = "live"; break;
        case Type::Historical: key = "historical"; break;
        case Type::Order: key = "order"; break;
    }

    for (const auto& topic : configJson[key]["topics"]) {
        topics.push_back(topic.get<std::string>());
    }
    std::cout << "Loaded topics: " << topics.size() << std::endl;
}

// Start listening for messages
void EventListener::startListening() {
    listenerThread = std::thread([this]() {
        try {
            for (const auto& topic : topics) {
                solaceClient.subscribeToTopic(topic);
                std::cout << "Subscribed to topic: " << topic << std::endl;
            }
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in EventListener: " << e.what() << std::endl;
        }
    });
    listenerThread.detach(); // Detach the thread to keep listening
}

// Static message callback
void EventListener::messageCallback(const std::string& topic, const std::string& message) {
    std::cout << "Message received on topic: " << topic << std::endl;
    std::cout << "Message content: " << message << std::endl;
}
