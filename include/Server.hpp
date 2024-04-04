/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:11:00 by smallem           #+#    #+#             */
/*   Updated: 2024/04/04 18:29:03 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

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

class Users;
class Channel;

class Server {
	private:
		int	serverSocket;
		std::string	password;
		std::map<std::string, Users *> all_users;
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
		Users *createUser(int socketDescriptor);
		void deleteUser(std::string uname);
		Users *getUserByUn(const std::string uname);
		Users *getUserByFd(int fd);
		// create channel delete channel
		void createChannel(Channel *channel);
		void deleteChannel(Channel *channel);
		Channel *getChannel(const std::string &name);

		// message forwarding i guess
		void sendToAll(const std::string &msg);
		void sendToChannel(const std::string &msg, Channel *channel);
		void sendToUser(const std::string &msg, const std::string uname);

		std::string getPassword() const;
};

#endif
