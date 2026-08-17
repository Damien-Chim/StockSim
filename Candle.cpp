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