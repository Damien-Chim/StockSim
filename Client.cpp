#include "Client.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
using boost::asio::ip::tcp;

void Client::run() {
	try {
		boost::asio::io_context io_context;
		tcp::resolver resolver(io_context);
		tcp::resolver::results_type server_endpoints = resolver.resolve("127.0.0.1", "54000");
		tcp::socket socket(io_context);
		boost::asio::connect(socket, server_endpoints);
		std::cout << "Connected to a server" << std::endl;
		while (true) {
			std::string line;
			std::getline(std::cin, line);
			if (line.empty()) { continue; }
			if (line == "QUIT") { break; }
			
			// wrap the string inside a boost::asio::buffer()
			// pass it to the write function
			line += '\n';
			boost::asio::write(socket, boost::asio::buffer(line));

			std::string data;
			std::size_t n = boost::asio::read_until(socket, boost::asio::dynamic_buffer(data), '\n');
			std::string response = data.substr(0, n);
			data.erase(0, n);
			std::cout << response << std::endl;
		}
	}

	catch (const std::exception& e) {
		std::cerr << "Client error: " << e.what() << '\n';
	}
}