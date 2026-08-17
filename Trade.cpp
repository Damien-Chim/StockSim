#include "Trade.hpp"
#include <string>

Trade::Trade(const std::string& trade_id, const std::string& stock_id, const std::string& buyer_id, const std::string& seller_id, int quantity, int price) :
    trade_id{ trade_id }, stock_id{ stock_id }, buyer_id{ buyer_id }, seller_id{ seller_id }, quantity{ quantity }, price{ price } {

}

const std::string& Trade::get_trade_id() {
    return trade_id;
}

const std::string& Trade::get_stock_id() {
    return stock_id;
}
const std::string& Trade::get_buyer_id() {
    return buyer_id;
}
const std::string& Trade::get_seller_id() {
    return seller_id;
}
int Trade::get_quantity() {
    return quantity;
}
int Trade::get_price() {
    return price;
}

Timestamp Trade::get_executed_timestamp() {
    return executed_timestamp;
}