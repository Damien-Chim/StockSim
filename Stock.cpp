#include <string>
#include "Stock.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"
#include "Exchange.hpp"

Stock::Stock(std::string stock_id, std::string stock_name, std::string stock_symbol, int market_price) :
	stock_id{ stock_id }, stock_name{ stock_name }, stock_symbol{ stock_symbol }, market_price{ market_price } {
}

Stock::Stock(std::string stock_name, std::string stock_symbol, int market_price) :
	stock_id{ Exchange::generate_stock_id() }, stock_name{ stock_name }, stock_symbol{ stock_symbol }, market_price{ market_price } {

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

void Stock::add_trade(const Trade& new_trade) {
	trade_history.push_back(new_trade);
}

void Stock::update_candle(const Trade& new_trade) {
	Timestamp executed_timestamp = new_trade.get_executed_timestamp();
	int price = new_trade.get_price();
	int quantity = new_trade.get_quantity();

	Timestamp start_time = (executed_timestamp / 10) * 10;
	auto it = candles.find(start_time);
	if (it == candles.end()) {
		// candle hasn't existed yet
		Candle new_candle{ .start_time = start_time, .open = price, .high = price, .low = price, .close = price, .volume = quantity, .colour = Colour::GREY };
		auto [new_it, inserted] = candles.emplace(start_time, new_candle);
		it = new_it;
	}

	else {
		// candle already existedd
		it->second.high = std::max(it->second.high, price);
		it->second.low = std::min(it->second.low, price);
		it->second.close = price;
		it->second.volume += quantity;
	}

	// update the colour
	int open = it->second.open;
	int close = it->second.close;
	int price_difference = close - open;
	if (price_difference == 0) {
		it->second.colour = Colour::GREY;
	}

	else if (price_difference > 0) {
		it->second.colour = Colour::GREEN;
	}

	else if (price_difference < 0) {
		it->second.colour = Colour::RED;
	}
}