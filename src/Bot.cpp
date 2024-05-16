#include "../include/Server.hpp"

Bot::Bot() : _name("Jeff") {}

Bot::~Bot() {}

Bot::Bot(const Bot &cp) : _name(cp.getName()) { *this = cp; }

Bot &Bot::operator=(const Bot &cp) {
	if (this != &cp)
		*this = cp;
	return *this;
}

std::string Bot::getName() const { return this->_name; }

void Bot::setName(const std::string name) { this->_name = name; }

void Bot::tellJoke(Users *user, Channel *channel) {
    srand(static_cast<unsigned int>(time(0)));
	int	val = (rand() % 100) + 1;
	std::ifstream file("example.txt");
    int i = 0;
    std::string joke;
    while (std::getline(file, joke)) {
		i++;
		if (i == val)
			break ;
	}
	file.close();

	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "Tell Joke", joke));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "Tell Joke", joke));
}

void Bot::d20(Users *user, Channel *channel) {
	srand(static_cast<unsigned int>(time(0)));
	int	val = (rand() % 20) + 1;
	std::stringstream ss;
	ss << val;
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "D20", ss.str()));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "D20", ss.str()));
}

void Bot::eightBall(Users *user, Channel *channel) {
	srand(static_cast<unsigned int>(time(0)));
	int val = (rand() % 8) + 1;
	std::stringstream ss;
	ss << val;
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "Eight Ball", ss.str()));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "Eight Ball", ss.str()));
}

void Bot::coinFlip(Users *user, Channel *channel) {
	srand(static_cast<unsigned int>(time(0)));
	int val = rand() % 2;
	std::stringstream ss;
	ss << val;
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "Coin Flip", ss.str()));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "Coin Flip", ss.str()));
}
