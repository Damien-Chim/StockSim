#include "Order.hpp"
#include "User.hpp"
#include <string>
Order::Order(std::string order_id, std::string user_id, std::string stock_id, Side side, int quantity, int limit_price) :
	order_id{ order_id }, 
	user_id{ user_id }, 
	stock_id{ stock_id }, 
	side{ side }, 
	quantity{ quantity }, 
	limit_price{ limit_price }, 
	reserved_cash{ quantity * limit_price } {
}

/*
	const std::string& get_order_id() const;
	const std::string& get_user_id() const;
	const std::string& get_stock_id() const;
	Side get_side() const;
	int get_quantity() const;
	int get_limit_price() const;
*/

const std::string& Order::get_order_id() const {
	return order_id;
}

const std::string& Order::get_user_id() const {
	return user_id;
}

const std::string& Order::get_stock_id() const {
	return stock_id;
}

Side Order::get_side() const {
	return side;
}

int Order::get_quantity() const {
	return quantity;
}

int Order::get_limit_price() const {
	return limit_price;
}

int Order::get_reserved_cash() const {
	return reserved_cash;
}

void Order::set_quantity(int new_quantity) {
	quantity = new_quantity;
}

void Order::set_reserved_cash(int new_reserved_cash) {
	reserved_cash = new_reserved_cash;
}
