#include "Server.hpp"
#include "Exchange.hpp"
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

static std::string process_command(std::string request) {
    std::vector<std::string> tokens = split(request, " ");
    if (tokens.empty()) {
        return "Invalid Command\n";
    }

    std::string instruction = tokens[0];
    if (instruction == "register") {
        if (tokens.size() != 2) { return "Invalid Command"; }
        std::string username = tokens[1];
        Exchange::add_user(User(username));
        return "Successfully registered user\n";
    }

    else if (instruction == "get_id") {

    }

    else if (instruction == "get_available_cash") {

    }

    else if (instruction == "get_reserved_cash") {

    }
    
    else if (instruction == "get_available_cash") {

    }

    return "Invalid Command\n";
}

static void client_thread(tcp::socket socket) {
    try {
        boost::asio::streambuf buffer;

        while (true) {
            boost::asio::read_until(socket, buffer, '\n');

            std::istream input(&buffer);
            std::string line;
            std::getline(input, line);
            
            std::string response = process_command(line);
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