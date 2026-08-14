#pragma once
class Server {
private:
	int port;
public:
	// Constructor for the server
	Server(int port);

	// server starter
	void run();
};