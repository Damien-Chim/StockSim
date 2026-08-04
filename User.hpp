#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Stock.hpp"

class User {
private:
	std::string user_id;
	std::string username;
	long long cash;

	// Stock : Quantity
	std::unordered_map<std::string, int> owned_stocks{};

	//bool write_to_owned_stocks(Stock stock, int quantity);

public:
	User(std::string user_id, std::string username, long long cash);

	//bool add_stock(Stock stock, int quantity);

	//bool add_stock(std::vector<std::pair<Stock, int>> stocks);

	bool deposit_cash(long long amount);

	bool buy_stock(std::string stock_id, int quantity, int limit_price);

	bool sell_stock(std::string stock_id, int quantity, int limit_price);
};
