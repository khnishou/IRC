
#include "../include/Server.hpp"
#include "../include/Users.hpp"
#include "../include/Channel.hpp"

int main() {
	Server serv(6667, "pswd");

	while (serv.getState() == START) {
		serv.init();
		serv.start();
	}
}
