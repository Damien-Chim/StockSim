#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <vector>

class BotManager {
private:
    std::vector<std::string> bot_ids;

    std::atomic<bool> running{ false };
    std::thread bot_thread;

    void run();
    void make_trade(const std::string& bot_id);

public:
    BotManager() = default;

    void create_bots(int count);
    void start();
    void stop();

    ~BotManager();
};