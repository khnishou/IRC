
#include "../include/Server.hpp"
#include "../include/Users.hpp"
#include "../include/Channel.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv <port number> <password>" << std::endl;
		exit(EXIT_SUCCESS);
	}
	std::string pswd = argv[2];
	int port = std::atoi(argv[1]);
	// port check goes here

	Server serv(port, pswd);

	while (serv.getState() == START) {
		serv.init();
		serv.start();
	}
	exit(EXIT_SUCCESS);
}

