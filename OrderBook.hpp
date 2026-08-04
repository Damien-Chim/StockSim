#pragma once
#include <map>
#include <queue>
#include "Order.hpp"

class OrderBook {
private:
	// for buy orders, highest bid price is the best
	std::map<int, std::queue<Order>, std::greater<int>> buy_orders;

	// for sell orders, lowest ask price is the best
	std::map<int, std::queue<Order>> sell_orders;

public:
	void place_order(Order order, Side side);
};