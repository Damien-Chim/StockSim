#include "OrderBook.hpp"
#include "Order.hpp"
#include "Exchange.hpp"
#include "User.hpp"
#include <queue>
#include <algorithm>
void OrderBook::place_order(const std::string& user_id, const std::string stock_id, int quantity, Side side, int limit_price) {
	std::string order_id = Exchange::generate_order_id();
	Order order(order_id, user_id, stock_id, Side::SELL, quantity, limit_price);
	active_orders[order_id] = order;
	if (side == Side::BUY) {
		buy_orders[limit_price].push(order_id);
	}

	else if (side == Side::SELL) {
		buy_orders[limit_price].push(order_id);
	}
}

void OrderBook::match_orders() {
	clean_buy_level();
	clean_sell_level();
	while (true) {
		if (buy_orders.size() == 0 || sell_orders.size() == 0) { return; }
		if (buy_orders.begin()->first < sell_orders.begin()->first) { return; }

		std::string buy_order_id = buy_orders.begin()->second.front();
		std::string sell_order_id = sell_orders.begin()->second.front();

		Order& buy_order = active_orders[buy_order_id];
		Order& sell_order = active_orders[sell_order_id];

		int execution_price = sell_order.get_limit_price();
		int traded_quantity = std::min(buy_order.get_quantity(), sell_order.get_quantity());
		int spent = execution_price * traded_quantity;

		buy_order.set_quantity(buy_order.get_quantity() - traded_quantity);
		buy_order.set_reserved_cash(buy_order.get_reserved_cash() - spent);
		sell_order.set_quantity(sell_order.get_quantity() - traded_quantity);

		User& user = Exchange::get_user(buy_order.get_user_id());
		user.set_reserved_cash(user.get_reserved_cash() - spent);
		user.add_owned_stock(buy_order.get_stock_id(), traded_quantity);

		if (buy_order.get_quantity() == 0) {
			user.set_available_cash(user.get_available_cash() + buy_order.get_reserved_cash());
			user.set_reserved_cash(user.get_reserved_cash() - buy_order.get_reserved_cash());
			active_orders.erase(buy_order_id);
			clean_buy_level();
		}

		if (sell_order.get_quantity() == 0) {
			active_orders.erase(sell_order_id);
			clean_sell_level();
		}
	}
}