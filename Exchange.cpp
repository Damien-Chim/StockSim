
#include "Exchange.hpp"
#include "Order.hpp"
#include <optional>
#include <string>
int Exchange::next_order_id = 0;

std::string Exchange::generate_order_id() {
	return "ORDER_" + std::to_string(next_order_id);
	next_order_id += 1;
}

std::string Exchange::generate_trade_id() {
	return "TRADE_" + std::to_string(next_trade_id);
	next_trade_id += 1;
}

bool Exchange::buy_request(const std::string& user_id, const std::string& stock_id, int quantity, int limit_price) {
	auto user_it = user_map.find(user_id);
	if (user_it == user_map.end()) { return false; }

	auto stock_it = stock_map.find(stock_id);
	if (stock_it == stock_map.end()) { return false; }

	if (quantity < 0 || limit_price < 0) { return false; }

	std::string order_id = generate_order_id();
	Order order(order_id, user_id, stock_id, Side::BUY, quantity, limit_price, Status::OPEN);
	order_map[order_id];
	
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

	if (quantity < 0 || limit_price < 0) { return false; }
	
	std::string order_id = generate_order_id();
	Order order(order_id, user_id, stock_id, Side::SELL, quantity, limit_price, Status::OPEN);
	order_map[order_id];

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

Order* Exchange::get_order(const std::string& order_id) {
	auto order_it = order_map.find(order_id);
	if (order_it == order_map.end()) { return nullptr; }
	return &order_it->second;
}