#include "Order.hpp"
#include "User.hpp"
#include <string>
Order::Order(std::string order_id, std::string user_id, std::string stock_id, Side side, int quantity, int limit_price) :
	order_id{ order_id }, user_id{ user_id }, stock_id{ stock_id }, 
	side{ side }, quantity{ quantity }, limit_price{ limit_price } {
}

Side Order::get_side() {
	return side;
}

int Order::get_limit_price() {
	return limit_price;
}