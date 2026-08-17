#pragma once
#include <string>
#include "Time.hpp"

class Trade {
private:
    std::string trade_id;
    std::string stock_id;
    std::string buyer_id;
    std::string seller_id;
    
    int quantity;
    int price;
    Timestamp executed_timestamp;

public:
    Trade(const std::string& trade_id, const std::string& stock_id, const std::string& buyer_id, const std::string& seller_id, int quantity, int price);
    const std::string& get_trade_id();
    const std::string& get_stock_id();
    const std::string& get_buyer_id();
    const std::string& get_seller_id();
    int get_quantity();
    int get_price();
    Timestamp get_executed_timestamp();
};
