#include <iostream>
#include <fstream>

//#include "../include/Aggregator.h"
//#include "../include/MarketDataGateway.h"
//#include "../include/OrderGateway.h"
//#include "../include/OrderManagementSystem.h"
//#include "../include/TradingEngine.h"
#include "../include/json.hpp"
#include "../include/MarketDataGateway/MarketDataGateway.h"
#include "Adapters/AlpacaAdapter.h"

using namespace std;

std::atomic<bool> running(true);  // Use atomic flag to control the main loop

// Signal handler to stop the program gracefully
void signalHandler(int signal) {
    std::cout << "\nSignal (" << signal << ") received. Stopping the program..." << std::endl;
    running = false;
}

int main() {
    // // // Initialize components
    // MarketDataGateway dataGateway;
    //
    // dataGateway.initialize("../config/config.json");
    //
    // // string response1 = dataGateway.getMarketData("tsla","iex");
    // string response2 = dataGateway.getLatestQuotes("tsla","iex");
    // cout << response1 << endl;

    signal(SIGINT, signalHandler);

    AlpacaAdapter alpaca_adapter;
    alpaca_adapter.initialize("../config/config.json");

    string response2 = alpaca_adapter.getLatestTick("tsla", "iex") ;

    cout << "" << endl;
    cout << response2 << endl;

    // Create an instance of the AlpacaAdapter
    AlpacaAdapter adapter;

    // Initialize with a configuration file (make sure to replace with the actual path)
    adapter.initialize("../config/config.json");

    // Tickers to subscribe for live data (e.g., Apple and Google)
    std::vector<std::string> tickers = {"AAPL", "GOOG"};

    // User selects mode
    bool testMode = true;
    cout << "Running in " << (testMode ? "Test Mode" : "Paper Trading Mode") << endl;

    // Subscribe to live data for the tickers
    adapter.subscribeLiveData(tickers, testMode);

    return 0;
}

// code split placeholder

// main.cpp
// #include <websocketpp/config/asio_client.hpp>
// #include <websocketpp/client.hpp>
// #include <iostream>
// #include <string>
// #include <thread>
// #include <functional>
// #include <mutex>
//
// typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
//
// std::string api_key = "AK8QCPTP9R8X1WFFW4U0";  // Replace with your Alpaca API key
// std::string api_secret = "QVg2JUHHfb6P74UW2eVeYnyw1lzf9wxrLZ2oxZ0P";  // Replace with your Alpaca API secret
//
// std::mutex mtx;  // Mutex for printing thread-safe
//
// // Callback for receiving WebSocket messages
// void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
//     std::lock_guard<std::mutex> guard(mtx);
//     std::cout << "< " << msg->get_payload() << std::endl;
// }
//
// // Send authentication data
// void send_auth(client* c, websocketpp::connection_hdl hdl) {
//     std::string auth_msg = R"({"action":"auth","key":")" + api_key + R"(","secret":")" + api_secret + R"("})";
//     c->send(hdl, auth_msg, websocketpp::frame::opcode::text);
//     std::cout << "> " << auth_msg << std::endl;
// }
//
// // Send subscription message
// void send_subscription(client* c, websocketpp::connection_hdl hdl) {
//     std::string sub_msg = R"({"action":"subscribe","bars":["FAKEPACA"],"quotes":["FAKEPACA"]})";
//     c->send(hdl, sub_msg, websocketpp::frame::opcode::text);
//     std::cout << "> " << sub_msg << std::endl;
// }
//
// // Handle successful connection
// void on_open(client* c, websocketpp::connection_hdl hdl) {
//     std::cout << "Connected to WebSocket server!" << std::endl;
//
//     // Send authentication message
//     send_auth(c, hdl);
// }
//
// // Handle close event
// void on_close(client* c, websocketpp::connection_hdl hdl) {
//     std::cout << "Connection closed!" << std::endl;
// }
//
// // Handle connection failure
// void on_fail(client* c, websocketpp::connection_hdl hdl) {
//     std::cout << "Connection failed!" << std::endl;
// }
//
// int main() {
//     client c;
//     c.get_alog().clear_channels(websocketpp::log::alevel::frame_header |
//         websocketpp::log::alevel::frame_payload |
//         websocketpp::log::alevel::control);
//
//     try {
//         // Initialize ASIO Transport
//         try {
//             c.init_asio();
//
//             // Set TLS initialization callback
//             c.set_tls_init_handler([](websocketpp::connection_hdl) -> websocketpp::lib::shared_ptr<boost::asio::ssl::context> {
//                 try {
//                     auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
//                     ctx->set_default_verify_paths();
//                     return ctx;
//                 } catch (const std::exception &e) {
//                     std::cerr << "TLS initialization error: " << e.what() << std::endl;
//                     return nullptr;  // Return null on error
//                 }
//             });
//
//         } catch (const std::exception &e) {
//             std::cerr << "Error initializing WebSocket connection: " << e.what() << std::endl;
//         }
//
//         // Register WebSocket event handlers
//         c.set_open_handler(std::bind(&on_open, &c, std::placeholders::_1));
//         c.set_message_handler(std::bind(&on_message, &c, std::placeholders::_1, std::placeholders::_2));
//         c.set_close_handler(std::bind(&on_close, &c, std::placeholders::_1));
//         c.set_fail_handler(std::bind(&on_fail, &c, std::placeholders::_1));
//
//         // Create a connection to the test endpoint
//         websocketpp::lib::error_code ec;
//         client::connection_ptr con = c.get_connection("wss://stream.data.alpaca.markets/v2/test", ec);
//
//         if (ec) {
//             std::cout << "Connection error: " << ec.message() << std::endl;
//             return -1;
//         }
//
//         // Connect to the server
//         c.connect(con);
//
//         // Start the ASIO io_service run loop
//         std::thread ws_thread([&c]() {
//             c.run();
//         });
//
//         // Main thread waits for WebSocket messages and handles subscription
//         while (true) {
//             std::this_thread::sleep_for(std::chrono::seconds(5));
//             // Sending subscription message
//             send_subscription(&c, con->get_handle());
//         }
//
//         // Clean up
//         ws_thread.join();
//     } catch (const std::exception &e) {
//         std::cout << "Exception: " << e.what() << std::endl;
//     }
//
//     return 0;
// }

