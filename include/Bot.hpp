#ifndef BOT_HPP
# define BOT_HPP

#include "../include/Lib.hpp"
#include "../include/Server.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

class Users;
class Channel;
class Server;

class Bot {
	private:
		std::string _name;
	public:
		Bot();
		~Bot();
		Bot(const Bot &cp);
		Bot &operator=(const Bot &cp);

		std::string getName() const;
		void setName(const std::string name);

		void tellJoke(Users *user, Channel *channel, std::string sender);
		void d20(Users *user, Channel *channel, std::string sender);
		void eightBall(Users *user, Channel *channel, std::string sender);
		void coinFlip(Users *user, Channel *channel, std::string sender);
};

#endif
