cmake_minimum_required(VERSION 3.20)

project(StockSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(StockSim
    StockSim.cpp
    BotManager.cpp
    Client.cpp
    Exchange.cpp
    HttpServer.cpp
    Order.cpp
    OrderBook.cpp
    Server.cpp
    Stock.cpp
    Time.cpp
    Trade.cpp
    User.cpp
)

target_link_libraries(
    StockSim
    PRIVATE
        Threads::Threads
        Boost::boost
	nlohmann_json::nlohmann_json
)