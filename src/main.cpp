#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include <exception>
#include "Utils.hpp"

#include "Server.hpp"

Server* Server::_serverInstance = nullptr;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		return(1);
	}

	try {
		ConfigParser parser;
		parser.load(argv[1]);
		Config config;
		Server server(config);
		Server::_serverInstance = &server;
		server._handleSignals();
		raise(SIGINT);
		// server.listen();
	} catch (const WSException& e) {
		std::cerr << "Error: " << e.code() << " " << e.code().message() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
