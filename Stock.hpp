#pragma once
#include <string>
#include <unordered_map>
#include "OrderBook.hpp"
#include "Order.hpp"
#include "Time.hpp"
#include "Candle.hpp"
#include "Trade.hpp"

class Stock {
private:
	std::string stock_id;
	std::string stock_name;
	std::string stock_symbol;
	int market_price;
	OrderBook order_book;
	std::vector<Trade> trade_history;
	std::map<Timestamp, Candle> candles;

public:
	Stock(std::string stock_id, std::string stock_name, std::string stock_symbol, int market_price);
	Stock(std::string stock_name, std::string stock_symbol, int market_price);
	const std::string& get_stock_id() const;
	const std::string& get_stock_name() const;
	const std::string& get_stock_symbol() const;
	int get_market_price() const;
	OrderBook& get_order_book();
	void add_trade(const Trade& new_trade);
	void update_candle(const Trade& new_trade);
};