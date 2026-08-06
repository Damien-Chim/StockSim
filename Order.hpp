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
	int reserved_cash;

public:
	Order(std::string order_id, std::string user_id, std::string stock_id, Side side, int quantity, int limit_price);
	const std::string& get_order_id() const;
	const std::string& get_user_id() const;
	const std::string& get_stock_id() const;
	Side get_side() const;
	int get_quantity() const;
	int get_limit_price() const;
	int get_reserved_cash() const;
	void set_quantity(int new_quantity);
	void set_reserved_cash(int new_reserved_cash);
};