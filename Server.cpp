#include "Server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
using boost::asio::ip::tcp;

Server::Server(int port) : port{ port } {

}

static void client_thread(tcp::socket socket) {
    try {
        boost::asio::streambuf buffer;

        while (true) {
            boost::asio::read_until(socket, buffer, '\n');

            std::istream input(&buffer);
            std::string line;
            std::getline(input, line);

            std::string response = "Pong\n";
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