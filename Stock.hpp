#pragma once
#include <string>
#include <unordered_map>
#include "OrderBook.hpp"
#include "Order.hpp"

class Stock {
private:
	std::string stock_id;
	std::string stock_name;
	std::string stock_symbol;
	int market_price;
	OrderBook order_book;

public:
	Stock(std::string stock_id, std::string stock_name, std::string stock_symbol, int market_price);
	bool add_order(Order order, Side side);
	int get_market_price();
};