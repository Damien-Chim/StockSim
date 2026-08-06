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
	const std::string& get_stock_id() const;
	const std::string& get_stock_name() const;
	const std::string& get_stock_symbol() const;
	int get_market_price() const;
	OrderBook& get_order_book();
};