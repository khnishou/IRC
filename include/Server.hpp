/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:11:00 by smallem           #+#    #+#             */
/*   Updated: 2024/04/11 16:42:51 by smallem          ###   ########.fr       */
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
		int	serverSocket;
		ssize_t	bytesReceived;
		char	buffer[1024];
		std::string	password;
		std::vector<Users *> all_users;
		std::vector<Channel *> all_channels;
		std::vector<struct pollfd> fds;
		Server();
		
	public:
		Server(int port, std::string password);
		~Server();
		Server(const Server &cp);
		Server &operator=(const Server &cp);
		
		void start();
		void stop();
		
		// create user delete user
		int addNewClient();
		void handleMsg(Users *user, size_t i);
		void executeCmd(Message msg);

		Users *getUserByUn(const std::string uname);
		Users *getUserByFd(int fd);
		
		Channel *getChannel(const std::string cname);
		
		size_t getNumberOfUsers();
		// message forwarding i guess
	
		std::string getPassword() const;
		Message parsing(std::string str);

  int c_kick(std::vector<std::string> param, Users user);
  int c_invite(std::vector<std::string> param, Users user);
  int c_topic(std::vector<std::string> param, Users user);
  int c_mode(std::vector<std::string> param, Users user);
};

#endif
