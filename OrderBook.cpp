#include "OrderBook.hpp"
#include "Order.hpp"

void OrderBook::place_order(Order order, Side side) {
	if (side == Side::BUY) {
		buy_orders[order.get_limit_price()].push(order);
	}

	else if (side == Side::SELL) {
		sell_orders[order.get_limit_price()].push(order);
	}
}