#pragma once
#include <unordered_map>
#include <optional>
#include "Stock.hpp"
#include "User.hpp"
#include "Order.hpp"
#include "Trade.hpp"

class Exchange {
private:
	static std::unordered_map<std::string, Stock> stock_map;
	static std::unordered_map<std::string, User> user_map;
	static std::unordered_map<std::string, Order> order_map;
	static std::unordered_map<std::string, Trade> trade_map;
	static int next_order_id;
	static int next_trade_id;
public:
	static std::string generate_order_id();
	static std::string generate_trade_id();
	static bool buy_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price);
	static bool sell_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price);
	static bool cancel_request(const std::string& user_id, const std::string& order_id);
	static User* get_user(const std::string& user_id);
	static Order* get_order(const std::string& order_id);
	static void add_trade(Trade& trade);
	static void add_user(User& user);
	static void add_stock(Stock& stock);
	static const std::unordered_map<std::string, Trade>& get_trades();
	static Trade* get_trade(const std::string& trade_id);
};