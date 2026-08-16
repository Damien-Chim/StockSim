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

static int stock_qty(const User& user, const std::string& stock_id) {
    const auto& stocks = user.get_available_stocks();
    auto it = stocks.find(stock_id);
    return it == stocks.end() ? 0 : it->second;
}

static int reserved_stock_qty(const User& user, const std::string& stock_id) {
    const auto& stocks = user.get_reserved_stocks();
    auto it = stocks.find(stock_id);
    return it == stocks.end() ? 0 : it->second;
}

static void test_full_fill() {
    Exchange::add_stock(Stock("STOCK_FULL", "Full Fill Co", "FULL", 100));
    Exchange::add_user(User("buyer_full", "Buyer Full", 10000));
    Exchange::add_user(User("seller_full", "Seller Full", 0));

    User* buyer = Exchange::get_user("buyer_full");
    User* seller = Exchange::get_user("seller_full");
    assert(buyer && seller);

    seller->add_available_stocks({ {"STOCK_FULL", 10} });

    assert(buyer->buy_stock("STOCK_FULL", 10, 100));
    assert(seller->sell_stock("STOCK_FULL", 10, 100));

    Order* buy = Exchange::get_order("ORDER_0");
    Order* sell = Exchange::get_order("ORDER_1");
    Trade* trade = Exchange::get_trade("TRADE_0");

    assert(buy && sell && trade);
    assert(buy->get_status() == Status::FILLED);
    assert(sell->get_status() == Status::FILLED);
    assert(buy->get_quantity() == 0);
    assert(sell->get_quantity() == 0);
    assert(trade->get_quantity() == 10);
    assert(trade->get_price() == 100);
    assert(stock_qty(*buyer, "STOCK_FULL") == 10);
    assert(stock_qty(*seller, "STOCK_FULL") == 0);
    assert(reserved_stock_qty(*seller, "STOCK_FULL") == 0);
    assert(buyer->get_available_cash() == 9000);
    assert(buyer->get_reserved_cash() == 0);
    assert(seller->get_available_cash() == 1000);

    std::cout << "[PASS] full fill\n";
}

static void test_partial_buy_then_cancel() {
    Exchange::add_stock(Stock("STOCK_PARTIAL_BUY", "Partial Buy Co", "PBUY", 100));
    Exchange::add_user(User("buyer_partial", "Buyer Partial", 10000));
    Exchange::add_user(User("seller_partial", "Seller Partial", 0));

    User* buyer = Exchange::get_user("buyer_partial");
    User* seller = Exchange::get_user("seller_partial");
    assert(buyer && seller);

    seller->add_available_stocks({ {"STOCK_PARTIAL_BUY", 4} });

    // Buyer reserves 10 * 105 = 1050.
    assert(buyer->buy_stock("STOCK_PARTIAL_BUY", 10, 105));
    assert(seller->sell_stock("STOCK_PARTIAL_BUY", 4, 100));

    Order* buy = Exchange::get_order("ORDER_2");
    Order* sell = Exchange::get_order("ORDER_3");
    Trade* trade = Exchange::get_trade("TRADE_1");

    assert(buy && sell && trade);
    assert(buy->get_status() == Status::PARTIALLY_FILLED);
    assert(buy->get_quantity() == 6);
    assert(sell->get_status() == Status::FILLED);
    assert(trade->get_quantity() == 4);
    assert(trade->get_price() == 100);
    assert(stock_qty(*buyer, "STOCK_PARTIAL_BUY") == 4);

    // Your current accounting leaves 650 reserved on the order/user:
    // 1050 initially reserved - 400 actually spent = 650.
    assert(buy->get_reserved_cash() == 650);
    assert(buyer->get_reserved_cash() == 650);
    assert(buyer->get_available_cash() == 8950);

    assert(buyer->cancel_trade("ORDER_2"));
    assert(buy->get_status() == Status::CANCELED);
    assert(buyer->get_reserved_cash() == 0);
    assert(buyer->get_available_cash() == 9600);

    // A second cancellation must not refund twice.
    assert(!buyer->cancel_trade("ORDER_2"));
    assert(buyer->get_available_cash() == 9600);

    std::cout << "[PASS] partial buy + cancel\n";
}

static void test_partial_sell_then_cancel() {
    Exchange::add_stock(Stock("STOCK_PARTIAL_SELL", "Partial Sell Co", "PSELL", 100));
    Exchange::add_user(User("buyer_small", "Buyer Small", 10000));
    Exchange::add_user(User("seller_large", "Seller Large", 0));

    User* buyer = Exchange::get_user("buyer_small");
    User* seller = Exchange::get_user("seller_large");
    assert(buyer && seller);

    seller->add_available_stocks({ {"STOCK_PARTIAL_SELL", 10} });

    assert(seller->sell_stock("STOCK_PARTIAL_SELL", 10, 100));
    assert(buyer->buy_stock("STOCK_PARTIAL_SELL", 4, 105));

    Order* sell = Exchange::get_order("ORDER_4");
    Order* buy = Exchange::get_order("ORDER_5");
    Trade* trade = Exchange::get_trade("TRADE_2");

    assert(sell && buy && trade);
    assert(sell->get_status() == Status::PARTIALLY_FILLED);
    assert(sell->get_quantity() == 6);
    assert(buy->get_status() == Status::FILLED);
    assert(trade->get_quantity() == 4);
    assert(trade->get_price() == 100);
    assert(stock_qty(*seller, "STOCK_PARTIAL_SELL") == 0);
    assert(reserved_stock_qty(*seller, "STOCK_PARTIAL_SELL") == 6);

    assert(seller->cancel_trade("ORDER_4"));
    assert(sell->get_status() == Status::CANCELED);
    assert(stock_qty(*seller, "STOCK_PARTIAL_SELL") == 6);
    assert(reserved_stock_qty(*seller, "STOCK_PARTIAL_SELL") == 0);

    std::cout << "[PASS] partial sell + cancel\n";
}

static void test_no_match() {
    Exchange::add_stock(Stock("STOCK_NOMATCH", "No Match Co", "NONE", 95));
    Exchange::add_user(User("buyer_nomatch", "Buyer No Match", 10000));
    Exchange::add_user(User("seller_nomatch", "Seller No Match", 0));

    User* buyer = Exchange::get_user("buyer_nomatch");
    User* seller = Exchange::get_user("seller_nomatch");
    assert(buyer && seller);
    seller->add_available_stocks({ {"STOCK_NOMATCH", 5} });

    assert(buyer->buy_stock("STOCK_NOMATCH", 5, 90));
    assert(seller->sell_stock("STOCK_NOMATCH", 5, 100));

    Order* buy = Exchange::get_order("ORDER_6");
    Order* sell = Exchange::get_order("ORDER_7");

    assert(buy && sell);
    assert(buy->get_status() == Status::OPEN);
    assert(sell->get_status() == Status::OPEN);
    assert(buy->get_quantity() == 5);
    assert(sell->get_quantity() == 5);
    assert(stock_qty(*buyer, "STOCK_NOMATCH") == 0);
    assert(buyer->get_reserved_cash() == 450);
    assert(reserved_stock_qty(*seller, "STOCK_NOMATCH") == 5);

    std::cout << "[PASS] no match\n";
}

static void test_wrong_user_cannot_cancel() {
    // ORDER_6 belongs to buyer_nomatch.
    User* seller = Exchange::get_user("seller_nomatch");
    assert(seller);
    assert(!seller->cancel_trade("ORDER_6"));

    Order* buy = Exchange::get_order("ORDER_6");
    assert(buy && buy->get_status() == Status::OPEN);

    std::cout << "[PASS] ownership check on cancellation\n";
}

static void initialise() {
    Exchange::add_stock(Stock("Intel Corporation", "INTC", 103));
    Exchange::add_stock(Stock("NVIDIA Corporation", "NVDA", 225));
    Exchange::add_stock(Stock("Tesla, Inc.", "TSLA", 342));
    Exchange::add_stock(Stock("Ford Motor Company", "F", 14));
    Exchange::add_stock(Stock("McDonald's Corporation", "MCD", 273));
    Exchange::add_stock(Stock("JPMorgan Chase & Co.", "JPM", 363));
    Exchange::add_stock(Stock("Wallmart Inc.", "WMT", 115));
}

int main(int argc, char* argv[]) {
    initialise();

    if (argc != 2) { std::cout << "Invalid number of parameters" << std::endl; return 1; }
    std::string mode = argv[1];
    if (mode == "1") {
        std::cout << "Server" << std::endl;
        Server server(54000);
        server.run();
    }
    
    else if (mode == "2") {
        std::cout << "Client" << std::endl;
        Client client;
        client.run();
        
    }

    else { std::cout << "Invalid code" << std::endl; return 1; }


    //test_full_fill();
    //test_partial_buy_then_cancel();
    //test_partial_sell_then_cancel();
    //test_no_match();
    //test_wrong_user_cannot_cancel();

    //std::cout << "\nAll integration tests passed.\n";
    return 0;
}