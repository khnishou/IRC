/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenhoci <ibenhoci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:11:00 by smallem           #+#    #+#             */
/*   Updated: 2024/04/22 15:32:02 by ibenhoci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include "Users.hpp"
#include "Channel.hpp"
#include "Lib.hpp"

class Users;
class Channel;

class Server {
	private:
		int							port;
		std::string					host;
		std::string					password;
		SERVER_STATE				state;
		int							serverSocket;
		char							buffer[1024];
		ssize_t						bytesReceived;
		std::vector<Users *>		all_users;
		std::vector<Channel *>		all_channels;
		std::vector<struct pollfd>	fds;
		Server();
		
	public:
		Server(int port, std::string password);
		~Server();
		Server(const Server &cp);
		Server &operator=(const Server &cp);
		
		// Server state handling
		void init();
		void start();
		void stop();
		
		// GETTERS
		SERVER_STATE getState() const;
		Users *getUserByUn(const std::string uname);
		Users *getUserByFd(int fd);
		std::string getPassword() const;
		Channel *getChannel(const std::string cname);
		size_t getNumberOfUsers();
		
		bool	nickNameExists(std::string nname);
		std::string fill_vec(std::vector<std::string> param);
		
		// message forwarding i guess
		void	send_2usr(int fd);

		int addNewClient();
		void handleMsg(Users *user, size_t i);
 		Message parsing(std::string str);
  		void executeCmd(Message msg, Users *user);

		// COMMANDS
		void c_kick(std::vector<std::string> param, Users *user);
		void c_invite(std::vector<std::string> param, Users *user);
		void c_topic(std::vector<std::string> param, Users *user);
		void c_mode(std::vector<std::string> param, Users *user);
		void c_pass(std::vector<std::string> param, Users *user);
		void c_nick(std::vector<std::string> param, Users *user);
};

#endif
