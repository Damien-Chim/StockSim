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
    // Candle 0-9: GREEN
    // O=100, H=108, L=97, C=105
    {1,  100, 5},
    {3,  108, 2},
    {5,   97, 4},
    {8,  105, 3},

    // Candle 10-19: RED
    // O=110, H=115, L=101, C=103
    {11, 110, 4},
    {13, 115, 2},
    {16, 101, 8},
    {19, 103, 5},

    // Candle 20-29: GREY
    // O=107, H=112, L=102, C=107
    {20, 107, 3},
    {23, 112, 6},
    {25, 102, 2},
    {29, 107, 7},

    // Candle 30-39: RED
    // O=105, H=105, L=90, C=95
    {31, 105, 10},
    {34, 100, 3},
    {36,  90, 5},
    {38,  95, 2},

    // Candle 40-49: GREEN
    // O=95, H=120, L=95, C=118
    {41,  95, 4},
    {43, 100, 5},
    {46, 120, 1},
    {49, 118, 6}
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
            Candle new_candle{ .start_time = start_time, .open = price, .high = price, .low = price, .close = price, .volume = quantity, .colour=Colour::GREY };
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

    for (const auto& [start_time, candle] : candles) {
        std::cout
            << "Time: " << start_time
            << " | O: " << candle.open
            << " H: " << candle.high
            << " L: " << candle.low
            << " C: " << candle.close
            << " V: " << candle.volume
            << " COLOUR: " << static_cast<int>(candle.colour)
            << '\n';
    }
    return 0;
}