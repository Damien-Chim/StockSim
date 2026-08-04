#pragma once
#include <unordered_map>
#include <optional>
#include "Stock.hpp"
#include "User.hpp"
#include "OrderBook.hpp"

class Exchange {
private:
	static std::unordered_map<std::string, Stock> stock_map;
	static std::unordered_map<std::string, User> user_map;
	static std::unordered_map<std::string, OrderBook> order_book_map;

public:
	static bool buy_request(std::string user_id, std::string stock_id, int quantity, int limit_price);
	static bool sell_request(std::string user_id, std::string stock_id, int quantity, int limit_price);
	static void add_stock(std::string stock_id, Stock stock);
	static std::optional<Stock> get_stock(std::string stock_id);
	static void add_user(std::string user_id, User user);
	static std::optional<User> get_user(std::string user_id);
};