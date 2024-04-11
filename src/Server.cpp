/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:12:02 by smallem           #+#    #+#             */
/*   Updated: 2024/04/11 15:43:28 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <algorithm>

Server::Server() {}

Server::Server(int port, std::string password) {
	//Creating the socket
	this->password = password;
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSocket == -1) {
		std::cerr << "Error: Failed to create server socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Setting the socket options for reusing address and port
	int opt = 1;
	if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			== -1) {
		std::cerr << "Error: failed to set socket options." << std::endl;
		exit(EXIT_FAILURE); 		
	}

	if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: Failed to set socket to non-blocking mode." << std::endl;
		exit(EXIT_FAILURE);
	}
	// Now binding the socket to the address and port
	struct sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	if (bind(this->serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))
			== -1) {
		std::cerr << "Error: failed to bind server socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	struct pollfd server_fd;
    server_fd.fd = this->serverSocket;
    server_fd.events = POLLIN;
    fds.push_back(server_fd);
	
	// Start listening for incoming connections
	if (listen(this->serverSocket, SOMAXCONN) == -1) {
		std::cerr << "Error: failed to start listening for incoming connections." << std::endl;
		exit(EXIT_FAILURE);
	}
}

Server::Server(const Server &cp) { *this = cp; }

Server &Server::operator=(const Server &cp) {
  if (this != &cp)
    *this = cp;
  return *this;
}

Server::~Server() {
	close(this->serverSocket);
	// probably close and clean all the FDS form the pollfd
	
    // Clean up channels
    for (std::vector<Channel*>::iterator it = this->all_channels.begin(); it != this->all_channels.end(); ++it) {
        delete *it;
    }

    // Clean up users
    for (std::vector<Users*>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it) {
        delete *it;
    }
}

int Server::addNewClient() {
	struct sockaddr_in clientAddress;
	socklen_t clientAddrSize = sizeof(clientAddress);
	int clientSocket = accept(this->serverSocket, (struct sockaddr *)&clientAddress, &clientAddrSize);
	if (clientSocket == -1) {
		std::cerr << "Error: Failed to accept incoming connection." << std::endl;
		return -1;
	}
	// possibly might have to set this socket as well ot non blocking before adding
	if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: Failed to set socket to non-blocking mode." << std::endl;
		exit(EXIT_FAILURE);
	}
	// Add the new client socket to the vector
    pollfd new_client_fd;
    new_client_fd.fd = clientSocket;
    new_client_fd.events = POLLIN;
    fds.push_back(new_client_fd);
	size_t id = getNumberOfUsers();
	
	std::string tmp;

	tmp = "default" + std::to_string(id);
	Users *user = new Users(tmp, tmp, "normal", 0, clientSocket);
	if (!user) {
		std::cerr << "Error: failed to create user for the connected client." << std::endl;
		close(clientSocket);
		return -1;
	}	
	this->all_users.push_back(user);
	std::cout << "New connection!" << std::endl;
	return 0;
}

void Server::handleMsg(Users *user, size_t i) {
	this->bytesReceived = recv(user->getSocketDescriptor(), this->buffer, sizeof(this->buffer), 0);

	if (this->bytesReceived <= 0) {
		close(user->getSocketDescriptor());
		this->fds.erase(this->fds.begin() + i);
		if (this->bytesReceived == 0)
			std::cout << "Connection closed." << std::endl;
		else
			std::cerr << "Error: recv: " << std::strerror(errno) << std::endl;
		return ;
	}
	else {
		//  N A T S U E Z G H O D
		std::string msg(buffer, bytesReceived - 1);
		std::cout << "Received: " << msg << std::endl;
	}
}

void Server::start() {
	int activity;
	Users	*user;

	while (true) {
		// We listen for incoming connections or incoming anything really
		activity = poll(&(this->fds[0]), this->fds.size(), -1);	
		if (activity < 0) {
			std::cout << "Error: Poll error: " << errno << std::endl;
			continue ;
		}
		// Check for events occuring on already connected clients
		for (size_t i = 0; i < this->fds.size(); ++i) {
			if (this->fds[i].revents & POLLIN) {
				if (this->fds[i].fd == this->serverSocket) {
					if (addNewClient() == -1)
						continue ;
				}
				else {
					user = getUserByFd(this->fds[i].fd);
					if (user)
						handleMsg(user, i);
				}		
			}
		}
	}
}

void Server::stop() { close(this->serverSocket); }

size_t	Server::getNumberOfUsers() {
	return this->all_users.size();
}

std::string Server::getPassword() const { return this->password; }

Users *Server::getUserByFd(int fd) {
	for (std::vector<Users *>::iterator it = this->all_users.begin(); 
			it != this->all_users.end(); ++it) {
		if ((*it)->getSocketDescriptor() == fd)
			return *it;
	}
	return NULL;
}

Users *Server::getUserByUn(const std::string username) {
	for (std::vector<Users *>::iterator it = this->all_users.begin();
			it != this->all_users.end(); ++it) {
		if ((*it)->getUserName() == username)
			return *it;
	}
	return NULL;
}

// ['@' <tags> SPACE] [':' <source> SPACE] <command> <parameters> <crlf>
Message Server::parsing(std::string str) {

    Message msg;

	int i;
	int len;

	i = skip_space(str, 0);
	while (str[i] && str[i] == '@')
	{
		len = skip_arg(str, i);
		msg.tags.push_back(str.substr(i + 1, len - 1));
		i += len;
		i = skip_space(str, i);
	}
	if (str[i] && str[i] == ':')
	{
		len = skip_arg(str, i);
        msg.source = str.substr(i + 1, len - 1);
		i += len;
		i = skip_space(str, i);
    }
	if (str[i])
	{
		len = skip_arg(str, i);
        msg.command = str.substr(i, len);
		i += len;
		i = skip_space(str, i);
    }
	while (str[i])
	{
		len = skip_arg(str, i);
		msg.parameters.push_back(str.substr(i, len));
		i += len;
		i = skip_space(str, i);
	}
    return msg;
}

//	Command: KICK
//	Parameters: <channel> <user> *( "," <user> ) [<comment>]
int Server::c_kick (std::vector<std::string> param)
{
	if (param.size() >= 3)
		; // error ERR_NEEDMOREPARAMS (461)
	Channel *channel;
	
}

//	Command: INVITE
//	Parameters: <nickname> <channel>
int Server::c_invite (std::vector<std::string> param)
{
	
}

//	Command: TOPIC
//	Parameters: <channel> [<topic>]
int Server::c_topic (std::vector<std::string> param)
{
	
}

//	Command: MODE
//	Parameters: <target> [<modestring> [<mode arguments>...]]
int Server::c_mode (std::vector<std::string> param)
{
	
}


void Server::executeCmd(Message msg)
{
	// handle tag
	// handle source 
	if (msg.command == "KICK") {
		c_kick(msg.parameters);	
	}
	else if (msg.command == "INVITE") {
		c_invite(msg.parameters);
	}
	else if (msg.command == "TOPIC") {
		c_topic(msg.parameters);
	}
	else if (msg.command == "MODE") {
		c_mode(msg.parameters);
	}
	else {
		// invalid cmd or whatever
	}
	
}
