#include "BotManager.hpp"

#include "Exchange.hpp"
#include "Stock.hpp"
#include "User.hpp"

#include <chrono>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>


void BotManager::create_bots(int count) {
    bot_ids.clear();
    for (int i = 0; i < count; ++i) {
        User bot("BOT_" + std::to_string(i));

        // Give each bot plenty of cash
        bot.deposit_cash(1'000'000);

        // Give each bot stocks to sell
        for (const auto& [stock_id, stock] : Exchange::get_stocks()) {
            bot.add_available_stocks({
                {stock_id, 1000}
                });
        }

        std::string bot_id = bot.get_user_id();

        Exchange::add_user(bot);

        bot_ids.push_back(bot_id);
    }
}


void BotManager::start() {
    if (running) {
        return;
    }

    running = true;

    bot_thread = std::thread(
        &BotManager::run,
        this
    );
}


void BotManager::stop() {
    running = false;

    if (bot_thread.joinable()) {
        bot_thread.join();
    }
}


BotManager::~BotManager() {
    stop();
}


void BotManager::run() {
    std::mt19937 rng(std::random_device{}());

    std::uniform_int_distribution<int> sleep_dist(100, 400);

    while (running) {
        if (!bot_ids.empty()) {
            std::uniform_int_distribution<std::size_t> bot_dist(
                0,
                bot_ids.size() - 1
            );

            make_trade(
                bot_ids[bot_dist(rng)]
            );
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                sleep_dist(rng)
            )
        );
    }
}


void BotManager::make_trade(const std::string& bot_id) {
    auto stocks = Exchange::get_stocks();

    if (stocks.empty()) {
        return;
    }

    std::vector<std::string> stock_ids;

    for (const auto& [stock_id, stock] : stocks) {
        stock_ids.push_back(stock_id);
    }

    static thread_local std::mt19937 rng(
        std::random_device{}()
    );

    std::uniform_int_distribution<std::size_t> stock_dist(
        0,
        stock_ids.size() - 1
    );

    std::string stock_id =
        stock_ids[stock_dist(rng)];

    Stock* stock =
        Exchange::get_stock(stock_id);

    User* bot =
        Exchange::get_user(bot_id);

    if (stock == nullptr || bot == nullptr) {
        return;
    }

    int market_price =
        stock->get_market_price();

    std::uniform_int_distribution<int> quantity_dist(1, 20);
    std::uniform_int_distribution<int> offset_dist(-20, 20);
    std::uniform_int_distribution<int> side_dist(0, 1);

    int quantity =
        quantity_dist(rng);

    int limit_price =
        market_price + offset_dist(rng);

    if (limit_price <= 0) {
        limit_price = 1;
    }

    bool buying =
        side_dist(rng) == 0;

    if (buying) {
        bot->buy_stock(
            stock_id,
            quantity,
            limit_price
        );
    }
    else {
        bot->sell_stock(
            stock_id,
            quantity,
            limit_price
        );
    }
}