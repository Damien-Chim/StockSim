
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

User::User(std::string username) : 
	user_id{ Exchange::generate_user_id() }, 
	username{ username }, 
	available_cash{ 0 }, 
	reserved_cash{ 0 } {

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

const std::unordered_map<std::string, int>& User::get_available_stocks() const {
	return available_stocks;
}

const std::unordered_map<std::string, int>& User::get_reserved_stocks() const {
	return reserved_stocks;
}

bool User::deposit_cash(long long amount) {
	if (amount < 0) { return false; }
	available_cash += amount;
	return true;
}

bool User::buy_stock(const std::string& stock_id, int quantity, int limit_price) {
	return Exchange::buy_request(user_id, stock_id, quantity, limit_price);
}

bool User::sell_stock(const std::string& stock_id, int quantity, int limit_price) {
	return Exchange::sell_request(user_id, stock_id, quantity, limit_price);
}

bool User::cancel_trade(const std::string& order_id) {
	return Exchange::cancel_request(user_id, order_id);
}

void User::set_available_cash(long long new_cash_balance) {
	available_cash = new_cash_balance;
}

void User::set_reserved_cash(long long new_reserved_cash) {
	reserved_cash = new_reserved_cash;
}

void User::add_available_stocks(const std::unordered_map<std::string, int>& new_available_stocks) {
	for (auto& [stock_id, quantity] : new_available_stocks) {
		available_stocks[stock_id] += quantity;
	}
}

void User::add_reserved_stocks(const std::unordered_map<std::string, int>& new_reserved_stocks) {
	for (auto& [stock_id, quantity] : new_reserved_stocks) {
		reserved_stocks[stock_id] += quantity;
	}
}

void User::remove_available_stocks(const std::unordered_map<std::string, int>& stocks_to_remove) {
	for (auto& [stock_id, quantity] : stocks_to_remove) {
		auto it = available_stocks.find(stock_id);
		if (it != available_stocks.end()) {
			it->second -= quantity;
			if (it->second == 0) { available_stocks.erase(it); }
		}
	}
}

void User::remove_reserved_stocks(const std::unordered_map<std::string, int>& stocks_to_remove) {
	for (auto& [stock_id, quantity] : stocks_to_remove) {
		auto it = reserved_stocks.find(stock_id);
		if (it != reserved_stocks.end()) {
			it->second -= quantity;
			if (it->second == 0) { reserved_stocks.erase(it); }
		}
	}
}

void User::add_order(std::string order_id) {
	orders.push_back(order_id);
}

const std::vector<std::string> User::get_orders() const {
	return orders;
}