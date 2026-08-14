#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Stock.hpp"

class User {
private:
	std::string user_id;
	std::string username;
	long long available_cash{ 0 };
	long long reserved_cash{ 0 };
	std::unordered_map<std::string, int> available_stocks{};
	std::unordered_map<std::string, int> reserved_stocks{};

public:
	User(std::string user_id, std::string username, long long cash_balance);

	User(std::string username);

	const std::string& get_user_id() const;

	const std::string& get_username() const;

	long long get_available_cash() const;

	long long get_reserved_cash() const;

	const std::unordered_map<std::string, int>& get_available_stocks() const;

	const std::unordered_map<std::string, int>& get_reserved_stocks() const;

	bool deposit_cash(long long amount);

	bool buy_stock(const std::string& stock_id, int quantity, int limit_price);

	bool sell_stock(const std::string& stock_id, int quantity, int limit_price);

	bool cancel_trade(const std::string& order_id);

	void set_available_cash(long long new_cash_balance);

	void set_reserved_cash(long long new_reserved_cash);

	void add_available_stocks(const std::unordered_map<std::string, int>& new_available_stocks);

	void add_reserved_stocks(const std::unordered_map<std::string, int>& new_reserved_stocks);

	void remove_available_stocks(const std::unordered_map<std::string, int>& stocks_to_remove);

	void remove_reserved_stocks(const std::unordered_map<std::string, int>& stocks_to_remove);


};
