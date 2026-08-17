#include<vector>
#include<map>
#include<iostream>
#include <algorithm>

#include "Candle.hpp"
struct Trade {
    int timestamp;
    int price;
    int quantity;
};

// sample trades
std::vector<Trade> trades = {
    // {timestamp, price, quantity}
    {1,  100, 5},
    {3,  105, 2},
    {6,   98, 4},
    {8,  103, 3},

    {12, 104, 5},
    {15, 108, 2},
    {19, 106, 7},

    {21, 105, 3},
    {27, 110, 6}
};


int main() {
    std::map<int, Candle> candles; // maps start_time to the actual Candle

    for (const auto& t : trades) {
        int timestamp = t.timestamp;
        int price = t.price;
        int quantity = t.quantity;

        int start_time = (timestamp / 10) * 10;
        auto it = candles.find(start_time);
        if (it == candles.end()) {
            // candle hasn't existed yet
            Candle new_candle{ .start_time = start_time, .open = price, .high = price, .low = price, .close = price, .volume = quantity };
            candles.emplace(start_time, new_candle);
        }

        else {
            // candle already existed
            it->second.high = std::max(it->second.high, price);
            it->second.low = std::min(it->second.low, price);
            it->second.close = price;
            it->second.volume += quantity;

        }
    }

    for (const auto& [start_time, candle] : candles) {
        std::cout
            << "Time: " << start_time
            << " | O: " << candle.open
            << " H: " << candle.high
            << " L: " << candle.low
            << " C: " << candle.close
            << " V: " << candle.volume
            << '\n';
    }
    return 0;
}