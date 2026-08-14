
#include "Exchange.hpp"
#include "Order.hpp"
#include "Trade.hpp"
#include <optional>
#include <string>
int Exchange::next_order_id = 0;
int Exchange::next_trade_id = 0;
int Exchange::next_user_id = 0;
int Exchange::next_stock_id = 0;
std::unordered_map<std::string, Stock> Exchange::stock_map;
std::unordered_map<std::string, User> Exchange::user_map;
std::unordered_map<std::string, Order> Exchange::order_map;
std::unordered_map<std::string, Trade> Exchange::trade_map;

std::string Exchange::generate_order_id() {
	std::string id = "ORDER_" + std::to_string(next_order_id);
	next_order_id += 1;
	return id;
}

std::string Exchange::generate_trade_id() {
	std::string id = "TRADE_" + std::to_string(next_trade_id);
	next_trade_id += 1;
	return id;
}

std::string Exchange::generate_user_id() {
	std::string id = "USER_" + std::to_string(next_user_id);
	next_user_id += 1;
	return id;
}

std::string Exchange::generate_stock_id() {
	std::string id = "STOCK_" + std::to_string(next_stock_id);
	next_stock_id += 1;
	return id;
}

bool Exchange::buy_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	if (quantity <= 0 || limit_price <= 0) { return false; }

	std::string order_id = generate_order_id();
	Order order(order_id, user_id, stock_id, Side::BUY, quantity, limit_price, Status::OPEN);
	order_map.emplace(order_id, order);
	
	Stock& stock = stock_it->second;
	stock.get_order_book().place_order(order);
	stock.get_order_book().match_orders();

	return true;
}

bool Exchange::sell_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	if (quantity <= 0 || limit_price <= 0) { return false; }
	
	std::string order_id = generate_order_id();
	Order order(order_id, user_id, stock_id, Side::SELL, quantity, limit_price, Status::OPEN);
	order_map.emplace(order_id, order);

	Stock& stock = stock_it->second;
	stock.get_order_book().place_order(order);
	stock.get_order_book().match_orders();

	return true;
}

bool Exchange::cancel_request(const std::string& user_id, const std::string& order_id) {
	User* user = get_user(user_id);
	if (user == nullptr) { return false; }
	Order* order = get_order(order_id);
	if (order == nullptr) { return false; }
	if (order->get_user_id() != user_id) { return false; }
	if (order->get_status() != Status::OPEN && order->get_status() != Status::PARTIALLY_FILLED) { return false; }
	// refund stocks/cash to user
	const Side side = order->get_side();
	const std::string stock_id = order->get_stock_id();
	const int remaining_stock_quantity = order->get_quantity();
	const int remaining_cash = order->get_reserved_cash();

	if (side == Side::BUY) {
		user->set_available_cash(user->get_available_cash() + remaining_cash);
		user->set_reserved_cash(user->get_reserved_cash() - remaining_cash);
	}

	else if (side == Side::SELL) {
		user->add_available_stocks({ {stock_id, remaining_stock_quantity} });
		user->remove_reserved_stocks({ {stock_id, remaining_stock_quantity} });
	}

	order->set_status(Status::CANCELED);
	return true;
}

User* Exchange::get_user(const std::string& user_id) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return nullptr; }
	return &user_it->second;
}

std::unordered_map<std::string, User> Exchange::get_users() {
	return user_map;
}

Order* Exchange::get_order(const std::string& order_id) {
	auto order_it = order_map.find(order_id);
	if (order_it == order_map.end()) { return nullptr; }
	return &order_it->second;
}

void Exchange::add_trade(Trade& trade) {
	const std::string trade_id = trade.get_trade_id();
	trade_map.emplace(trade_id, trade);
}

void Exchange::add_user(User user) {
	const std::string user_id = user.get_user_id();
	user_map.emplace(user_id, user);
}

void Exchange::add_stock(Stock stock) {
	const std::string stock_id = stock.get_stock_id();
	stock_map.emplace(stock_id, stock);
}

const std::unordered_map<std::string, Trade>& Exchange::get_trades() {
	return trade_map;
}

Trade* Exchange::get_trade(const std::string& trade_id) {
	auto it = trade_map.find(trade_id);
	if (it == trade_map.end()) { return nullptr; }
	return &it->second;
}