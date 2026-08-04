#include <string>
#include "Stock.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"

Stock::Stock(std::string stock_id, std::string stock_name, std::string stock_symbol, int market_price) :
	stock_id{ stock_id }, stock_name{ stock_name }, stock_symbol{ stock_symbol }, market_price{ market_price } {

}
bool Stock::add_order(Order order, Side side) {
	if (side == Side::BUY) {
		order_book.place_order(order, side);
		return true;
	}

	else if (order.get_side() == Side::SELL) {
		order_book.place_order(order, side);
		return true;
	}

	return false;
}
int Stock::get_market_price() {
	return market_price;
}