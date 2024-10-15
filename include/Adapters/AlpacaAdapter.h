// Created by Admin on 1/10/2024.
//

#ifndef ALPACAADAPTER_H
#define ALPACAADAPTER_H
#include <string>
#include <vector>

#include "../interface/GenericAdapter.h"
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_client.hpp>  // for TLS connection
#include <websocketpp/client.hpp>


typedef websocketpp::client<websocketpp::config::asio_tls_client> websocket_client;
typedef websocketpp::connection_hdl connection_hdl;

class AlpacaAdapter : public GenericAdapter {
public:
    // Constructor
    AlpacaAdapter(const std::string& configFile);

    // Destructor
    ~AlpacaAdapter();

    // Config parameters
    void initialize(const std::string& configFile) override;

    //Single json request
    std::string getLatestTick(const std::string &symbol, const std::string &feed) override;

    // Websocket live data
    void subscribeLiveData(const std::vector<std::string>& tickers, bool testMode) override;

    // Graceful disconnect method
    void gracefulDisconnect();

private:
    // Config parameters
    void loadConfig(const std::string& configFile);
    std::string apiKey;
    std::string secretKey;
    std::string baseUrl;
    std::string dataUrl;
    std::string mode;
    std::string mode_options;
    std::mutex mtx;

    bool isAuthenticated = false;

    //Single json request
    std::string performRequest(const std::string& url);

    // Websocket live data
    websocket_client c;
    websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr con;

    void initializeWebSocketConnection(websocket_client& c, const std::string& url);

    std::string buildAuthMessage();
    std::string buildSubscriptionMessage(const std::vector<std::string>& tickers);
    void sendMessage(websocket_client* c, websocketpp::connection_hdl hdl, const nlohmann::json& message);
    void send_auth(websocket_client* c, websocketpp::connection_hdl hdl);
    void send_subscription(websocket_client* c, websocketpp::connection_hdl hdl);

    void on_message(websocket_client* c, websocketpp::connection_hdl hdl, websocket_client::message_ptr msg);
    void on_open(websocket_client* c, websocketpp::connection_hdl hdl);
    void on_close(websocket_client* c, websocketpp::connection_hdl hdl);
    void on_fail(websocket_client* c, websocketpp::connection_hdl hdl);
};


#endif //ALPACAADAPTER_H