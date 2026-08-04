#pragma once
#include <string>
#include "User.hpp"
#include "Stock.hpp"

enum class Side {
	BUY,
	SELL
};

class Order {

private:
	std::string order_id;
	std::string user_id;
	std::string stock_id;
	Side side;
	int quantity;
	int limit_price;

public:
	Order(std::string order_id, std::string user_id, std::string stock_id, Side side, int quantity, int limit_price);
	Side get_side();
	int get_limit_price();

};