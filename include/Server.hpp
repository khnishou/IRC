/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:11:00 by smallem           #+#    #+#             */
/*   Updated: 2024/04/05 15:48:43 by smallem          ###   ########.fr       */
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
#include "lib.hpp"

struct Message {
    std::vector<std::string> tags;
    std::string source;
    std::string command;
    std::vector<std::string> parameters;
};

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
		
		void deleteUser(std::string uname);
		Users *getUserByUn(const std::string uname);
		Users *getUserByFd(int fd);
		
		size_t getNumberOfUsers();
		// message forwarding i guess
	
		std::string getPassword() const;
		Message parsing(std::string str);
};

#endif
