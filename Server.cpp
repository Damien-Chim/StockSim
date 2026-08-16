#include "Server.hpp"
#include "Exchange.hpp"
#include "User.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <string>

using boost::asio::ip::tcp;

Server::Server(int port) : port{ port } {

}

static std::vector<std::string> split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

static std::string get_input_string(boost::asio::streambuf* buffer) {
    std::istream input(buffer);
    std::string line;
    std::getline(input, line);
    return line;
}

static std::string handle_log_in(std::string request, std::string& user_id, bool& logged_in) {
    std::vector<std::string> tokens = split(request, " ");
    if (tokens.empty() || tokens.size() != 2) {
        return "Invalid Command\n";
    }

    std::string instruction = tokens[0];
    std::string username = tokens[1];

    if (instruction == "login") {
        for (auto& [uid, user] : Exchange::get_users()) {
            if (user.get_username() == username) {
                user_id = user.get_user_id();
                logged_in = true;
                return "Login successful\n";
            }
        }

        return "User not found\n";

    }
    else if (instruction == "sign_up") {
        for (auto& [uid, user] : Exchange::get_users()) {
            if (user.get_username() == username) {
                return "User already exists\n";
            }
        }
        User new_user(username);
        Exchange::add_user(new_user);
        user_id = new_user.get_user_id();
        logged_in = true;
        return "Sign up and login successful\n";
    }

    return "Invalid command\n";
}

static std::string process_command(std::string request, const std::string user_id) {
    User* user = Exchange::get_user(user_id);
    std::ostringstream oss;
    std::vector<std::string> tokens = split(request, " ");
    if (tokens.empty()) {
        return "Invalid Command\n";
    }

    std::string instruction = tokens[0];
    //if (instruction == "register") {
    //    if (tokens.size() != 2) { return "Invalid Command\n"; }
    //    std::string username = tokens[1];
    //    Exchange::add_user(User(username));
    //    return "Successfully registered user\n";
    //}

    if (instruction == "get_id") {
        std::string response = user->get_user_id();
        response += '\n';
        return response;
    }

    else if (instruction == "get_available_cash") {
        long long available_cash = user->get_available_cash();
        oss << available_cash << "\n";
        std::string response = oss.str();
        return response;
    }

    else if (instruction == "get_reserved_cash") {
        long long reserved_cash = user->get_reserved_cash();
        oss << reserved_cash << "\n";
        std::string response = oss.str();
        return response;
    }
    
    else if (instruction == "deposit_cash") {
        long long deposit_amount = std::stoi(tokens[1]);
        if (user->deposit_cash(deposit_amount)) {
            return "Deposit successful\n";
        }
        return "Deposit failed\n";
    }

    else if (instruction == "buy_stock") {
        std::string stock_id = tokens[1];
        int quantity = std::stoi(tokens[2]);
        int limit_price = std::stoi(tokens[3]);
        if (user->buy_stock(stock_id, quantity, limit_price)) {
            return "Order Placed\n";
        }
        return "Failed to place order\n";
    }

    else if (instruction == "sell_stock") {
        std::string stock_id = tokens[1];
        int quantity = std::stoi(tokens[2]);
        int limit_price = std::stoi(tokens[3]);
        if (user->sell_stock(stock_id, quantity, limit_price)) {
            return "Order Placed\n";
        }
        return "Failed to place order\n";
    }

    return "Invalid Command\n";
}

static void client_thread(tcp::socket socket) {
    try {
        boost::asio::streambuf buffer;
        bool logged_in = false;
        std::string user_id;
        while (!logged_in) {
            boost::asio::read_until(socket, buffer, '\n');
            std::string input_string = get_input_string(&buffer);
            std::string response = handle_log_in(input_string, user_id, logged_in);
            boost::asio::write(socket, boost::asio::buffer(response));
        }

        while (true) {
            boost::asio::read_until(socket, buffer, '\n');
            std::string input_string = get_input_string(&buffer);
            std::string response = process_command(input_string, user_id);
            boost::asio::write(socket, boost::asio::buffer(response));
        }
    }
    catch (const boost::system::system_error& e) {
        if (e.code() != boost::asio::error::eof) {
            std::cerr << "Client error: " << e.what() << '\n';
        }
    }

    std::cout << "Client disconnected\n";
}

void Server::run() {
	boost::asio::io_context io_context;
	tcp::endpoint endpoint(tcp::v4(), port);
	tcp::acceptor acceptor(io_context, endpoint);
	std::cout << "Server listening on port " << port << std::endl;
    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket); // this line blocks until a client connects

        std::cout << "Client connected\n";

        std::thread(
            client_thread,
            std::move(socket)
        ).detach();
    }
	
}