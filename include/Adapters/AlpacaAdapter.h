// Created by Admin on 1/10/2024.
//

#ifndef ALPACAADAPTER_H
#define ALPACAADAPTER_H
#include <string>
#include <vector>

#include "../interface/GenericAdapter.h"
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>


typedef websocketpp::client<websocketpp::config::asio_client> websocket_client;
typedef websocketpp::connection_hdl connection_hdl;

class AlpacaAdapter : public GenericAdapter {
public:
    // Config parameters
    void initialize(const std::string& configFile) override;
    //Single json request
    std::string getLatestTick(const std::string &symbol, const std::string &feed) override;
    // Websocket live data
    void subscribeLiveData(const std::vector<std::string>& tickers) override;

private:
    // Config parameters
    void loadConfig(const std::string& configFile);
    std::string apiKey;
    std::string secretKey;
    std::string baseUrl;
    std::string dataUrl;

    //Single json request
    std::string performRequest(const std::string& url);

    // Websocket live data
    void on_message(websocket_client* c, websocketpp::connection_hdl hdl, websocket_client::message_ptr msg);
    void sendMessage(websocket_client* c, websocketpp::connection_hdl hdl, const nlohmann::json& message);
    // helper methods for building msgs
    std::string buildAuthMessage();
    std::string buildSubscriptionMessage(const std::vector<std::string>& tickers);
    void initializeWebSocketConnection(websocket_client& c, const std::string& url);
};


#endif //ALPACAADAPTER_H