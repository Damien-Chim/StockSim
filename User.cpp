#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Stock.hpp"
#include "User.hpp"
#include "Order.hpp"
#include "Exchange.hpp"


User::User(std::string user_id, std::string username, long long cash) :
	user_id{ user_id }, username{ username }, cash{ cash } {

}

//bool User::write_to_owned_stocks(Stock stock, int quantity) {
//	auto it = owned_stocks.find(stock);
//	if (it == owned_stocks.end()) {
//		owned_stocks[stock] = quantity;
//	}
//
//	else {
//		it->second += quantity;
//	}
//
//	return true;
//}

//bool User::add_stock(Stock stock, int quantity) {
//	return write_to_owned_stocks(stock, quantity);
//}
//
//bool User::add_stock(std::vector<std::pair<Stock, int>> stocks) {
//	for (auto [stock, quantity] : stocks) {
//		if (User::write_to_owned_stocks(stock, quantity) == false) { return false; }
//	}
//	return true;
//}

bool User::deposit_cash(long long amount) {
	if (amount < 0) { return false; }
	cash += amount;
	return true;
}

bool User::buy_stock(std::string stock_id, int quantity, int limit_price) {
	if (quantity < 0 || limit_price < 0) { return false; }
	if (quantity * limit_price > cash) { return false; }
	// Order order("temporary_order_id", this->user_id, stock_id, Side::BUY, quantity, limit_price);
	Exchange::buy_request(this->user_id, stock_id, quantity, limit_price);
	return true;
}

bool User::sell_stock(std::string stock_id, int quantity, int limit_price) {
	if (quantity < 0 || limit_price < 0) { return false; }
	auto stock_it = owned_stocks.find(stock_id);
	if (stock_it == owned_stocks.end()) { return false; }
	if (quantity > stock_it->second) { return false; }
	Exchange::sell_request(this->user_id, stock_id, quantity, limit_price);
	return true;
}