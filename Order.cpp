#include "Order.hpp"
#include "User.hpp"
#include <string>
Order::Order(std::string order_id, std::string user_id, std::string stock_id, Side side, int quantity, int limit_price, Status status) :
	order_id{ order_id }, 
	user_id{ user_id }, 
	stock_id{ stock_id }, 
	side{ side }, 
	quantity{ quantity }, 
	limit_price{ limit_price },
	status{ status } {
	if (side == Side::BUY) {
		reserved_cash = static_cast<long long>(quantity * limit_price);
	}

	else if (side == Side::SELL) {
		reserved_cash = 0;
	}
}

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

long long Order::get_reserved_cash() const {
	return reserved_cash;
}

Status Order::get_status() const {
	return status;
}

void Order::set_quantity(int new_quantity) {
	quantity = new_quantity;
}

void Order::set_reserved_cash(long long new_reserved_cash) {
	reserved_cash = new_reserved_cash;
}

void Order::set_status(Status new_status) {
	status = new_status;
}

/*

Order status types
- Pending: The system received your order, but the market is closed or waiting for your target price.
- Open: Your order is active in the market book, waiting for a match.
- Filled: The trade finished completely at the set price.
- Partially Filled: Only some shares traded; the rest remain open.
- Canceled: You or the system stopped the order before it finished.
- Rejected: The broker or exchange declined the order due to an error or low funds.

*/