#include <cassert>
#include <iostream>
#include <string>
#include "Exchange.hpp"
#include "User.hpp"
#include "Stock.hpp"
#include "Order.hpp"
#include "Trade.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "HttpServer.hpp"
#include "BotManager.hpp"
#include "Simulation.hpp"

BotManager bots;
static void initialise() {
    Exchange::add_stock(Stock("Intel Corporation", "INTC", 103));
    Exchange::add_stock(Stock("NVIDIA Corporation", "NVDA", 225));
    Exchange::add_stock(Stock("Tesla, Inc.", "TSLA", 342));
    Exchange::add_stock(Stock("Ford Motor Company", "F", 14));
    Exchange::add_stock(Stock("McDonald's Corporation", "MCD", 273));
    Exchange::add_stock(Stock("JPMorgan Chase & Co.", "JPM", 363));
    Exchange::add_stock(Stock("Wallmart Inc.", "WMT", 115));
    Exchange::add_stock(Stock("Boeing", "BOE", 148));
    Exchange::add_user(User("Damien"));
    User* user = Exchange::get_user("USER_0");
    user->deposit_cash(10000);
    user->add_available_stocks(
        {
            {"STOCK_0", 10},
            {"STOCK_3", 20},
            {"STOCK_6", 15}
        }
    );
}

void reset_simulation() {
    bots.stop();

    Exchange::reset();

    initialise();

    bots.create_bots(20);
    bots.start();
}

int main(int argc, char* argv[]) {
    initialise();

    bots.create_bots(20);
    bots.start();
    if (argc != 2) {
        std::cout << "Invalid number of parameters" << std::endl; return 1;
    }

    std::string mode = argv[1];
    if (mode == "1") {
        std::cout << "Server" << std::endl;
        run_http_server();
    }
    
    else if (mode == "2") {
        std::cout << "Client" << std::endl;
        Client client;
        client.run();
    }

    else {
        std::cout << "Invalid code" << std::endl; return 1;
    }

    return 0;
}