cmake_minimum_required(VERSION 3.29)
project(AlphaStrat)

set(CMAKE_CXX_STANDARD 20)

add_executable(AlphaStrat src/main.cpp
        src/MarketDataGateway.cpp
        src/Aggregator.cpp
        src/TradingEngine.cpp
        src/OrderManagementSystem.cpp
        src/OrderGateway.cpp
        include/MarketDataGateway.h
        include/Aggregator.h
        include/TradingEngine.h
        include/OrderManagementSystem.h
        include/OrderGateway.h
        tests/test_MarketDataGateway.cpp
        tests/test_Aggregator.cpp
        tests/test_TradingEngine.cpp
        tests/test_OrderManagementSystem.cpp
        tests/test_OrderGateway.cpp)
