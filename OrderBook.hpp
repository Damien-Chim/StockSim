#pragma once
#include <map>
#include <unordered_map>
#include <queue>
#include <string>

class Order;
class User;

class OrderBook {
private:
	// for buy orders, highest bid price is the best
	std::map<int, std::queue<std::string>, std::greater<int>> buy_orders;

	// for sell orders, lowest ask price is the best
	std::map<int, std::queue<std::string>> sell_orders;
	
	void clean_buy_level();
	void clean_sell_level();
	void update_orders_after_execution(Order* buy_order, Order* sell_order, int traded_quantity, long long reserved_cash_spent);
	void swap_assets(User* buyer, User* seller, long long reserved_cash_spent, int traded_quantity, const std::string& stock_id);
	void refund_price_improvement(User* buyer, Order* buy_order);
	void make_trade(const std::string& stock_id, User* buyer, User* seller, int traded_quantity, long long execution_price);
	
public:

	void place_order(Order& order);
	// void match_orders();
	void match_orders(std::string order_id);
};