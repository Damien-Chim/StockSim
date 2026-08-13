#pragma once
#include <string>

enum class Side {
	BUY,
	SELL
};

enum class Status {
	OPEN, 
	FILLED,
	PARTIALLY_FILLED,
	CANCELED
};

class Order {

private:
	std::string order_id;
	std::string user_id;
	std::string stock_id;
	Side side;
	int quantity;
	int limit_price;
	long long reserved_cash;
	Status status;

public:
	Order(std::string order_id, std::string user_id, std::string stock_id, Side side, int quantity, int limit_price, Status status);
	const std::string& get_order_id() const;
	const std::string& get_user_id() const;
	const std::string& get_stock_id() const;
	Side get_side() const;
	int get_quantity() const;
	int get_limit_price() const;
	long long get_reserved_cash() const;
	Status get_status() const;
	// const std::unordered_map<std::string, int>& get_reserved_stocks() const;
	void set_quantity(int new_quantity);
	void set_reserved_cash(long long new_reserved_cash);
	// void add_reserved_stocks(const std::unordered_map<std::string, int>& new_reserved_stocks);
	// void remove_reserved_stocks(const std::unordered_map<std::string, int>& stocks_to_remove);
	void set_status(Status new_status);
};