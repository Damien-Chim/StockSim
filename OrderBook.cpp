#include "OrderBook.hpp"
#include "Order.hpp"
#include "Exchange.hpp"
#include "Trade.hpp"
#include "User.hpp"
#include "Time.hpp"
#include "Stock.hpp"
#include <queue>
#include <algorithm>

void OrderBook::update_orders_after_execution(Order* buy_order, Order* sell_order, int traded_quantity, long long reserved_cash_spent) {
	buy_order->set_quantity(buy_order->get_quantity() - traded_quantity);
	buy_order->set_reserved_cash(buy_order->get_reserved_cash() - reserved_cash_spent);
	sell_order->set_quantity(sell_order->get_quantity() - traded_quantity);
}

void OrderBook::swap_assets(User* buyer, User* seller, long long reserved_cash_spent, int traded_quantity, const std::string& stock_id) {
	buyer->set_reserved_cash(buyer->get_reserved_cash() - reserved_cash_spent);  // buyer uses up reserved_cash
	buyer->add_available_stocks({ {stock_id, traded_quantity} });  // in exchange for new stocks

	seller->remove_reserved_stocks({ {stock_id, traded_quantity} });  // seller uses up reserved_stock
	seller->set_available_cash(seller->get_available_cash() + reserved_cash_spent);    // in exchange for money
}

void OrderBook::refund_price_improvement(User* buyer, Order* buy_order) {
	buyer->set_available_cash(buyer->get_available_cash() + buy_order->get_reserved_cash());
	buyer->set_reserved_cash(buyer->get_reserved_cash() - buy_order->get_reserved_cash());
}

void OrderBook::make_trade(const std::string& stock_id, User* buyer, User* seller, int traded_quantity, long long execution_price) {
	std::string trade_id = Exchange::generate_trade_id();
	Timestamp executed_timestamp = current_timestamp();
	Trade trade(trade_id, stock_id, buyer->get_user_id(), seller->get_user_id(), traded_quantity, execution_price, executed_timestamp);
	Stock* stock = Exchange::get_stock(stock_id);
	if (stock != nullptr) {
		stock->add_trade(trade);
		stock->update_candle(trade);
		stock->set_market_price(execution_price);
	}
}

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
		if (order == nullptr) {
			buy_orders.begin()->second.pop();

			if (buy_orders.begin()->second.empty()) {
				buy_orders.erase(buy_orders.begin());
			}

			continue;
		}

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
		if (order == nullptr) {
			sell_orders.begin()->second.pop();

			if (sell_orders.begin()->second.empty()) {
				sell_orders.erase(sell_orders.begin());
			}

			continue;
		}

		const Status order_status = order->get_status();
		if (order_status == Status::OPEN || order_status == Status::PARTIALLY_FILLED) { return; }
		sell_orders.begin()->second.pop();
		if (sell_orders.begin()->second.empty()) { sell_orders.erase(sell_orders.begin()); }
	}
}

void OrderBook::match_orders(std::string order_id) {
	clean_buy_level();
	clean_sell_level();
	Order* order = Exchange::get_order(order_id);
	if (order == nullptr) { return; }

	if (order->get_side() == Side::BUY) {
		Order* buy_order = order;
		while (true) {
			if (buy_order->get_quantity() == 0) {
				return;
			}

			else if (sell_orders.size() == 0) {
				place_order(*buy_order);
				return;
			}

			else if (sell_orders.begin()->first > buy_order->get_limit_price()) {
				place_order(*buy_order);
				return;
			}

			std::string sell_order_id = sell_orders.begin()->second.front();
			Order* sell_order = Exchange::get_order(sell_order_id);
			if (sell_order == nullptr) {
				clean_sell_level();
				continue;
			}

			std::string stock_id = buy_order->get_stock_id();
			int execution_price = sell_order->get_limit_price();
			int traded_quantity = std::min(buy_order->get_quantity(), sell_order->get_quantity());
			long long reserved_cash_spent = static_cast<long long>(execution_price) * static_cast<long long>(traded_quantity);
			
			update_orders_after_execution(buy_order, sell_order, traded_quantity, reserved_cash_spent);

			User* buyer = Exchange::get_user(buy_order->get_user_id());
			User* seller = Exchange::get_user(sell_order->get_user_id());
			swap_assets(buyer, seller, reserved_cash_spent, traded_quantity, stock_id);

			// buyer order is fulfiled
			if (buy_order->get_quantity() == 0) {
				refund_price_improvement(buyer, buy_order);
				buy_order->set_status(Status::FILLED);
				buy_order->set_reserved_cash(0);
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
			
			make_trade(stock_id, buyer, seller, traded_quantity, execution_price);
		}
	}

	else if (order->get_side() == Side::SELL) {
		Order* sell_order = order;
		while (true) {
			if (sell_order->get_quantity() == 0) {
				return;
			}

			else if (buy_orders.size() == 0) {
				place_order(*sell_order);
				return;
			}

			else if (buy_orders.begin()->first < sell_order->get_limit_price()) {
				place_order(*sell_order);
				return;
			}

			std::string buy_order_id = buy_orders.begin()->second.front();

			Order* buy_order = Exchange::get_order(buy_order_id);
			if (buy_order == nullptr) {
				clean_buy_level();
				continue;
			}
			
			std::string stock_id = buy_order->get_stock_id();
			int execution_price = buy_order->get_limit_price();
			int traded_quantity = std::min(buy_order->get_quantity(), sell_order->get_quantity());
			long long reserved_cash_spent = static_cast<long long>(execution_price) * static_cast<long long>(traded_quantity);

			update_orders_after_execution(buy_order, sell_order, traded_quantity, reserved_cash_spent);

			User* buyer = Exchange::get_user(buy_order->get_user_id());
			User* seller = Exchange::get_user(sell_order->get_user_id());

			swap_assets(buyer, seller, reserved_cash_spent, traded_quantity, stock_id);

			// buyer order is fulfiled
			if (buy_order->get_quantity() == 0) {
				refund_price_improvement(buyer, buy_order);
				buy_order->set_status(Status::FILLED);
				buy_order->set_reserved_cash(0);
				clean_buy_level();
			}

			else {
				buy_order->set_status(Status::PARTIALLY_FILLED);
			}

			// seller order is fullfiled
			if (sell_order->get_quantity() == 0) {
				sell_order->set_status(Status::FILLED);
			}

			else {
				sell_order->set_status(Status::PARTIALLY_FILLED);
			}

			make_trade(stock_id, buyer, seller, traded_quantity, execution_price);
		}
	}
}

//void OrderBook::match_orders() {
//	clean_buy_level();
//	clean_sell_level();
//	while (true) {
//		if (buy_orders.size() == 0 || sell_orders.size() == 0) { return; }
//		if (buy_orders.begin()->first < sell_orders.begin()->first) { return; }
//
//		std::string buy_order_id = buy_orders.begin()->second.front();
//		std::string sell_order_id = sell_orders.begin()->second.front();
//
//		Order* buy_order = Exchange::get_order(buy_order_id);
//		Order* sell_order = Exchange::get_order(sell_order_id);
//
//		int execution_price = sell_order->get_limit_price();
//		int traded_quantity = std::min(buy_order->get_quantity(), sell_order->get_quantity());
//		int reserved_cash_spent = execution_price * traded_quantity;
//
//		buy_order->set_quantity(buy_order->get_quantity() - traded_quantity);
//		buy_order->set_reserved_cash(buy_order->get_reserved_cash() - reserved_cash_spent);
//		sell_order->set_quantity(sell_order->get_quantity() - traded_quantity);
//
//		User* buyer = Exchange::get_user(buy_order->get_user_id());
//		buyer->set_reserved_cash(buyer->get_reserved_cash() - reserved_cash_spent);  // buyer uses up reserved_cash
//		buyer->add_available_stocks({ {buy_order->get_stock_id(), traded_quantity} });  // in exchange for new stocks
//
//		User* seller = Exchange::get_user(sell_order->get_user_id());
//		seller->remove_reserved_stocks({ {sell_order->get_stock_id(), traded_quantity} });  // seller uses up reserved_stock
//		seller->set_available_cash(seller->get_available_cash() + reserved_cash_spent);    // in exchange for money
//
//		// buyer order is fulfiled
//		if (buy_order->get_quantity() == 0) {
//			// refund on price improvement
//			buyer->set_available_cash(buyer->get_available_cash() + buy_order->get_reserved_cash());
//			buyer->set_reserved_cash(buyer->get_reserved_cash() - buy_order->get_reserved_cash());
//			buy_order->set_status(Status::FILLED);
//			clean_buy_level();
//		}
//
//		else {
//			buy_order->set_status(Status::PARTIALLY_FILLED);
//		}
//
//		// seller order is fullfiled
//		if (sell_order->get_quantity() == 0) {
//			sell_order->set_status(Status::FILLED);
//			clean_sell_level();
//		}
//
//		else {
//			sell_order->set_status(Status::PARTIALLY_FILLED);
//		}
//
//		std::string trade_id = Exchange::generate_trade_id();
//		std::string stock_id = buy_order->get_stock_id();
//		Timestamp executed_timestamp = current_timestamp();
//		Trade trade(trade_id, stock_id, buyer->get_user_id(), seller->get_user_id(), traded_quantity, execution_price, executed_timestamp);
//		Stock* stock = Exchange::get_stock(stock_id);
//		if (stock != nullptr) {
//			stock->add_trade(trade);
//			stock->update_candle(trade);
//			stock->set_market_price(execution_price);
//		}
//	}
//}