#include <iostream>
#include "Config.hpp"
#include "Error.hpp"
#include "Server.hpp"
#include <exception>
#include <thread>
#include "Utils.hpp"
#include "SignalHandle.hpp"

#include "Server.hpp"

volatile sig_atomic_t sigintReceived = 0;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		return(1);
	}

	try {
		handleSignals();
		ConfigParser parser;
		Config config = parser.load(argv[1]);
		std::vector<Server> servers;
		// std::vector<std::thread> threads;
		// for (const auto& serverConfig : config.servers) {
		// 	threads.push_back(std::thread([serverConfig] {
		// 		Server server(serverConfig);
		// 		server.start();
		// 	}));
		// }
		// for (auto& thread : threads) {
		// 	thread.join();
		// }
		Server server(config);
		server.listen();
		// raise(SIGINT);
	} catch (const WSException& e) {
		std::cerr << "Error: " << e.code() << " " << e.code().message() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
