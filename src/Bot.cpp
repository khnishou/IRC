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
	std::ifstream file("src/jokes.txt");
   
	if (!file.is_open()) {
		if (channel)
			channel->broadcastMsg(ERR_BOT(this->getName(), "joke", "could not open file"));
		else if (user)
			user->setBuffer(ERR_BOT(this->getName(), "joke", "could not open file"));
		return ;
	}

	int i = 0;
   std::string joke;
	while (std::getline(file, joke)) {
		i++;
		if (i == val)
			break ;
	}
	
	
	file.close();
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "brace yourself for impact", joke));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "brace yourself for impact", joke));
}

void Bot::d20(Users *user, Channel *channel) {
	srand(static_cast<unsigned int>(time(0)));
	int	val = (rand() % 20) + 1;
	std::stringstream ss;
	ss << val;
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "rolling a d20... score", ss.str()));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "rolling a d20... score", ss.str()));
}

void Bot::eightBall(Users *user, Channel *channel) {
	srand(static_cast<unsigned int>(time(0)));
	int val = (rand() % 8) + 1;
	std::ifstream file("src/eightball.txt");

	if (!file.is_open()) {
		if (channel)
			channel->broadcastMsg(ERR_BOT(this->getName(), "eightball", "could not open file"));
		else if (user)
			user->setBuffer(ERR_BOT(this->getName(), "eightball", "could not open file"));
		return ;
	}

	std::string rpl;
	int	i = 0;
	while (std::getline(file, rpl)) {
		i++;
		if (i == val)
			break;
	}

	file.close();
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "the answer you seek is", rpl));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "the answer you seek is", rpl));
}

void Bot::coinFlip(Users *user, Channel *channel) {
	srand(static_cast<unsigned int>(time(0)));
	int val = rand() % 2;
	std::string res;
	if (val == 0)
		res = "HEADS";
	else
		res = "TAILS";
	if (user)
		user->setBuffer(RPL_BOT(this->getName(), "Coin Flip", res));
	else if (channel)
		channel->broadcastMsg(RPL_BOT(this->getName(), "Coin Flip", res));
}

