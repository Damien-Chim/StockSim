#include "OrderBook.hpp"
#include "Order.hpp"
#include "Exchange.hpp"
#include "Trade.hpp"
#include "User.hpp"
#include <queue>
#include <algorithm>
void OrderBook::place_order(Order& order) {
	Side side = order.get_side();
	int limit_price = order.get_limit_price();
	std::string order_id = order.get_order_id();

	if (side == Side::BUY) {
		buy_orders[limit_price].push(order_id);
	}

	else if (side == Side::SELL) {
		sell_orders[limit_price].push(order_id);
	}
}

void OrderBook::clean_buy_level() {
	while (true) {
		if (buy_orders.empty()) { return; }
		const std::string order_id = buy_orders.begin()->second.front();
		const Order* order = Exchange::get_order(order_id);
		const Status order_status = order->get_status();
		if (order_status == Status::OPEN || order_status == Status::PARTIALLY_FILLED) { return; }
		buy_orders.begin()->second.pop();
		if (buy_orders.begin()->second.empty()) { buy_orders.erase(buy_orders.begin()); }
	}
}

void OrderBook::clean_sell_level() {
	while (true) {
		if (sell_orders.empty()) { return; }
		const std::string order_id = sell_orders.begin()->second.front();
		const Order* order = Exchange::get_order(order_id);
		const Status order_status = order->get_status();
		if (order_status == Status::OPEN || order_status == Status::PARTIALLY_FILLED) { return; }
		sell_orders.begin()->second.pop();
		if (sell_orders.begin()->second.empty()) { sell_orders.erase(sell_orders.begin()); }
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

		Order* buy_order = Exchange::get_order(buy_order_id);
		Order* sell_order = Exchange::get_order(sell_order_id);

		int execution_price = sell_order->get_limit_price();
		int traded_quantity = std::min(buy_order->get_quantity(), sell_order->get_quantity());
		int reserved_cash_spent = execution_price * traded_quantity;

		buy_order->set_quantity(buy_order->get_quantity() - traded_quantity);
		buy_order->set_reserved_cash(buy_order->get_reserved_cash() - reserved_cash_spent);
		sell_order->set_quantity(sell_order->get_quantity() - traded_quantity);

		User* buyer = Exchange::get_user(buy_order->get_user_id());
		buyer->set_reserved_cash(buyer->get_reserved_cash() - reserved_cash_spent);  // buyer uses up reserved_cash
		buyer->add_available_stocks({ {buy_order->get_stock_id(), traded_quantity} });  // in exchange for new stocks

		User* seller = Exchange::get_user(sell_order->get_user_id());
		seller->remove_reserved_stocks({ {sell_order->get_stock_id(), traded_quantity} });  // seller uses up reserved_stock
		seller->set_available_cash(seller->get_available_cash() + reserved_cash_spent);    // in exchange for money

		// buyer order is fulfiled
		if (buy_order->get_quantity() == 0) {
			// refund on price improvement
			buyer->set_available_cash(buyer->get_available_cash() + buy_order->get_reserved_cash());
			buyer->set_reserved_cash(buyer->get_reserved_cash() - buy_order->get_reserved_cash());
			buy_order->set_status(Status::FILLED);
			clean_buy_level();
		}

		else {
			buy_order->set_status(Status::PARTIALLY_FILLED);
		}

		// seller order is fullfiled
		if (sell_order->get_quantity() == 0) {
			sell_order->set_status(Status::FILLED);
			clean_sell_level();
		}

		else {
			sell_order->set_status(Status::PARTIALLY_FILLED);
		}

		std::string trade_id = Exchange::generate_trade_id();
		std::string stock_id = buy_order->get_stock_id();
		Trade trade(trade_id, stock_id, buyer->get_user_id(), seller->get_user_id(), traded_quantity, execution_price);
		Exchange::add_trade(trade);
	}
}