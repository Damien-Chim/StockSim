#include "Stock.hpp"
#include "Exchange.hpp"
#include "Order.hpp"
#include <optional>
#include <string>

bool Exchange::buy_request(std::string user_id, std::string stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	Order order("temp_order_id", user_id, stock_id, Side::BUY, quantity, limit_price);
	stock_it->second.add_order(order, Side::BUY);

	return true;
}

bool Exchange::sell_request(std::string user_id, std::string stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	Order order("temp_order_id", user_id, stock_id, Side::SELL, quantity, limit_price);
	stock_it->second.add_order(order, Side::SELL);

	return true;
}

void Exchange::add_stock(std::string stock_id, Stock stock) {
	stock_map[stock_id] = stock;
}

std::optional<Stock> Exchange::get_stock(std::string stock_id) {
	auto it = stock_map.find(stock_id);
	if (it == stock_map.end()) { return std::nullopt; }
	return it->second;
}

void Exchange::add_user(std::string user_id, User user) {
	user_map[user_id] = user;
}
std::optional<User> Exchange::get_user(std::string user_id) {
	auto it = user_map.find(user_id);
	if (it == user_map.end()) { return std::nullopt; }
	return it->second;
}