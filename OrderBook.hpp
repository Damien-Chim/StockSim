#pragma once
#include <map>
#include <unordered_map>
#include <queue>
#include <string>
#include "Order.hpp"

class OrderBook {
private:
	// for buy orders, highest bid price is the best
	std::map<int, std::queue<std::string>, std::greater<int>> buy_orders;

	// for sell orders, lowest ask price is the best
	std::map<int, std::queue<std::string>> sell_orders;
	
	void clean_buy_level();
	void clean_sell_level();
	
public:
	void place_order(Order& order);
	void match_orders();
};