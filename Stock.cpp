#include <string>
#include "Stock.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"

Stock::Stock(std::string stock_id, std::string stock_name, std::string stock_symbol, int market_price) :
	stock_id{ stock_id }, stock_name{ stock_name }, stock_symbol{ stock_symbol }, market_price{ market_price } {

}

const std::string& Stock::get_stock_id() const {
	return stock_id;
}

const std::string& Stock::get_stock_name() const {
	return stock_name;
}

const std::string& Stock::get_stock_symbol() const {
	return stock_symbol;
}

int Stock::get_market_price() const {
	return market_price;
}

OrderBook& Stock::get_order_book() {
	return order_book;
}