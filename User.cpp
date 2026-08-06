#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Stock.hpp"
#include "User.hpp"
#include "Order.hpp"
#include "Exchange.hpp"


User::User(std::string user_id, std::string username, long long available_cash) :
	user_id{ user_id }, username{ username }, available_cash{ available_cash }, reserved_cash{ 0 } {
}

const std::string& User::get_user_id() const {
	return user_id;
}

const std::string& User::get_username() const {
	return username;
}

long long User::get_available_cash() const {
	return available_cash;
}

long long User::get_reserved_cash() const {
	return reserved_cash;
}

const std::unordered_map<std::string, int>& User::get_owned_stocks() const {
	return owned_stocks;
}

bool User::deposit_cash(long long amount) {
	if (amount < 0) { return false; }
	available_cash += amount;
	return true;
}

bool User::buy_stock(const std::string& stock_id, int quantity, int limit_price) {
	if (quantity < 0 || limit_price < 0) { return false; }
	if (quantity * limit_price > available_cash) { return false; }
	long long locked = quantity * limit_price;
	reserved_cash += locked;
	available_cash -= locked;
	return Exchange::buy_request(user_id, stock_id, quantity, limit_price);
}

bool User::sell_stock(const std::string& stock_id, int quantity, int limit_price) const {
	if (quantity < 0 || limit_price < 0) { return false; }
	auto stock_it = owned_stocks.find(stock_id);
	if (stock_it == owned_stocks.end()) { return false; }
	if (quantity > stock_it->second) { return false; }
	return Exchange::sell_request(user_id, stock_id, quantity, limit_price);
}

void User::set_available_cash(int new_cash_balance) {
	available_cash = new_cash_balance;
}

void User::set_reserved_cash(int new_reserved_cash) {
	reserved_cash = new_reserved_cash;
}

void User::add_owned_stock(const std::string& stock_id, int quantity) {
	owned_stocks[stock_id] += quantity;
}