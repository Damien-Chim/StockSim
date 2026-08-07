
#include "Exchange.hpp"
#include "Order.hpp"
#include <optional>
#include <string>
int Exchange::next_order_id = 0;

std::string Exchange::generate_order_id() {
	return "ORDER_" + std::to_string(next_order_id);
	next_order_id += 1;
}

bool Exchange::buy_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	if (quantity < 0 || limit_price < 0) { return false; }

	Stock& stock = stock_it->second;
	stock.get_order_book().place_order(user_id, stock_id, quantity, Side::BUY, limit_price);
	stock.get_order_book().match_orders();

	return true;
}

bool Exchange::sell_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	if (quantity < 0 || limit_price < 0) { return false; }
	
	Stock& stock = stock_it->second;
	stock.get_order_book().place_order(user_id, stock_id, quantity, Side::SELL, limit_price);
	stock.get_order_book().match_orders();

	return true;
}

User& Exchange::get_user(const std::string& user_id) {
	return user_map[user_id];
}