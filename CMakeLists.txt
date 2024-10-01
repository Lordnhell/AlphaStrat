cmake_minimum_required(VERSION 3.28.1)
project(AlphaStrat)

set(CMAKE_CXX_STANDARD 20)
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")

# Include the vcpkg toolchain
#set(CMAKE_TOOLCHAIN_FILE "[path to vcpkg]/scripts/buildsystems/vcpkg.cmake")

# Find necessary packages
find_package(CURL CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
#target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Gather all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE TEST_FILES "tests/*.cpp")
file(GLOB_RECURSE INCLUDE_FILES "include/*.h")



# Main executable
add_executable(AlphaStrat ${SRC_FILES} ${INCLUDE_FILES}
        include/initialize.h
        include/MarketDataGateway/MarketDataParser.h
        src/MarketDataGateway/MarketDataParser.cpp
        src/interface/GenericAdapter.cpp
        include/interface/GenericAdapter.h
        include/Adapters/AlpacaAdapter.h
        src/Adapters/AlpacaAdapter.cpp
)

# Link necessary libraries to the main executable
target_link_libraries(AlphaStrat PRIVATE CURL::libcurl)

# Test executable (make sure to include all source files)
add_executable(AlphaStratTests ${TEST_FILES} ${SRC_FILES} ${INCLUDE_FILES}
        include/MarketDataGateway/MarketDataParser.h
        src/MarketDataGateway/MarketDataParser.cpp
)
target_link_libraries(AlphaStratTests PRIVATE GTest::gtest_main CURL::libcurl)
enable_testing()

# Add a custom command to run tests before building the main program
add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS AlphaStratTests
)

# Make the main executable dependent on running tests
add_dependencies(AlphaStrat run_tests)

# Add a custom test for CTest
#add_test(NAME RunAllTests COMMAND AlphaStratTests)
