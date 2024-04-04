/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:12:02 by smallem           #+#    #+#             */
/*   Updated: 2024/04/04 16:57:25 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server() {
}

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
	
	// Start listening for incoming connections
	if (listen(this->serverSocket, SOMAXCONN) == -1) {
		std::cerr << "Error: failed to start listening for incoming connections." << std::endl;
		exit(EXIT_FAILURE);
	}
	this->start();
}

Server::Server(const Server &cp) {
	*this = cp;
}

Server &Server::operator=(const Server &cp) {
	if (this != &cp)
		*this = cp;
	return *this;
}

Server::~Server() {
	close(this->serverSocket);

    // Clean up channels
    for (std::vector<Channel*>::iterator it = this->all_channels.begin(); it != this->all_channels.end(); ++it) {
        delete *it;
    }

    // Clean up users
    for (std::map<std::string, Users*>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it) {
        delete it->second;
    }
}

Users* Server::createUser(int socketDescriptor) {
    // Obtain client information such as nickname, username, hostname, etc.
    // You can use socketDescriptor to retrieve client details if needed
	char buffer[1024];
	ssize_t bytesReceived;
	std::string msg = "Welcome to this IRC Server.\nPlease input the password.\n";
	send(socketDescriptor, msg.c_str(), msg.size(), 0);
	bytesReceived = recv(socketDescriptor, buffer, sizeof(buffer), 0);
	if (bytesReceived <= 0)
		return NULL;
	std::string passwd(buffer, bytesReceived - 1);
	if (passwd != this->getPassword())		
		return NULL;

	msg = "Input your Username.\n";
	send(socketDescriptor, msg.c_str(), msg.size(), 0);
	bytesReceived = recv(socketDescriptor, buffer, sizeof(buffer), 0);
	if (bytesReceived <= 0)
		return NULL;
	std::string userName(buffer, bytesReceived - 1);
	if (this->getUserByUn(userName) != NULL) {
		std::cout << "@" << (this->getUserByUn(userName))->getNickName() << " rejoined the server" << std::endl;
		return this->getUserByUn(userName);
	}

	msg = "Please choose a nickname, this will be your broadcast ID.\n";
	send(socketDescriptor, msg.c_str(), msg.size(), 0);
	bytesReceived = recv(socketDescriptor, buffer, sizeof(buffer), 0);
	if (bytesReceived <= 0)
		return NULL;

	std::string nickName(buffer, bytesReceived - 1);
    std::string defaultModes = "normal";
    int defaultPermissionLevel = 0; // Default permission level
    // Create a new Users object with default values

    Users *newUser = new Users(nickName, userName, defaultModes, defaultPermissionLevel, socketDescriptor);
	std::cout << "Created @" << newUser->getUserName() << std::endl;
    return newUser;
}

void Server::start() {
	fd_set readfds;
	int max_sd = this->serverSocket;
	int activity;
    char buffer[1024];
	ssize_t bytesReceived;
	Users	*user;

	FD_ZERO(&readfds);
	while (true) {
		FD_SET(this->serverSocket, &readfds);
		for (std::map<std::string, Users *>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it) {
			if (!it->second->getStatus())
				continue ;
			int cs = it->second->getSocketDescriptor();
			FD_SET(cs, &readfds);
			if (cs > max_sd)
				max_sd = cs;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if (activity < 0 && errno != EINTR) {
			std::cerr << "Some type of error: " << errno << std::endl;
			continue;
		}

		if (FD_ISSET(this->serverSocket, &readfds)) {
			struct sockaddr_in clientAddress;
			socklen_t clientAddrSize = sizeof(clientAddress);
			int cs = accept(this->serverSocket, (struct sockaddr *)&clientAddress, &clientAddrSize);
			if (cs == -1) {
				std::cerr << "Error: Failed to accept incoming connection." << std::endl;
				continue;
			}
			
			user = this->createUser(cs);
			if (!user) {
				std::cerr << "Error: failed to create user for the connected client." << std::endl;
				close(cs);
				continue ;
			}
			if (user->getSocketDescriptor() != cs || !user->getStatus()) {
				user->setSocketDescriptor(cs);
				user->setStatus(1);
			}
			else
				this->all_users.insert(std::pair<std::string, Users *>(user->getUserName(), user));
			FD_SET(cs, &readfds);
			if (cs >= max_sd)
				max_sd = cs;
			user = NULL;
		}
		for (std::map<std::string, Users*>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it) {
            int clientSocket = it->second->getSocketDescriptor();
            if (FD_ISSET(clientSocket, &readfds) && it->second->getStatus()) {
				bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
				if (bytesReceived == 0) {
					close(clientSocket); // Close the socket
            		FD_CLR(clientSocket, &readfds); // Remove from FD_SET
					it->second->setStatus(0);
					it->second->setSocketDescriptor(-1);
					std::cerr << "Connection closed by @" << it->second->getNickName() << std::endl;
					continue ;
				}
				else if (bytesReceived < 0) {
					std::cerr << "Error receiving msg from @" << it->second->getNickName() << std::endl;
					continue ;
				}
				std::string msg(buffer, bytesReceived - 1);
				// parse msg, type std::string 
				std::cout << "@" << it->second->getNickName() << ": " << msg << std::endl;
            }
        }
	}
}

void Server::stop() {
	close(this->serverSocket);
}

std::string Server::getPassword() const {
	return this->password;
}

Users *Server::getUserByUn(const std::string username) {
	for (std::map<std::string, Users *>::iterator it = this->all_users.begin();
			it != this->all_users.end(); ++it) {
		if (it->first == username)
			return it->second;
	}
	return NULL;
}

void Server::deleteUser(std::string uname) {
    std::map<std::string, Users *>::iterator it = this->all_users.find(uname);
    if (it != this->all_users.end()) {
        delete it->second;
        this->all_users.erase(it);
        // close(); // Close the socket
    }
}

void Server::createChannel(Channel *channel) {
	this->all_channels.push_back(channel);
}

void Server::deleteChannel(Channel *channel) {
	for (std::vector<Channel *>::iterator it = this->all_channels.begin();
			it != this->all_channels.end(); ++it) {
		if (*it == channel) {
			delete *it;
			this->all_channels.erase(it);
			return ;
		}
	}
}

Channel *Server::getChannel(const std::string &name) {
	for (std::vector<Channel *>::iterator it = this->all_channels.begin();
			it != this->all_channels.end(); ++it) {
		if ((*it)->getName() == name)
			return *it;
	}
	return NULL;
}

void Server::sendToAll(const std::string &msg) {
	
}

void Server::sendToChannel(const std::string &msg, Channel *channel) {
	
}

void Server::sendToUser(const std::string &msg, const std::string uname) {
	
}
