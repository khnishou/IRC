/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykerdel <ykerdel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:12:02 by smallem           #+#    #+#             */
/*   Updated: 2024/04/04 19:30:42 by ykerdel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <algorithm>

Server::Server() {}

Server::Server(int port, std::string password) {
  // Creating the socket
  this->password = password;
  this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->serverSocket == -1) {
    std::cerr << "Error: Failed to create server socket." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Setting the socket options for reusing address and port
  int opt = 1;
  if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) == -1) {
    std::cerr << "Error: failed to set socket options." << std::endl;
    exit(EXIT_FAILURE);
  }

	// if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1) {
	// 	std::cerr << "Error: Failed to set socket to non-blocking mode." << std::endl;
	// 	exit(EXIT_FAILURE);
	// }
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

  // Start listening for incoming connections
  if (listen(this->serverSocket, SOMAXCONN) == -1) {
    std::cerr << "Error: failed to start listening for incoming connections."
              << std::endl;
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

  // Clean up channels
  for (std::vector<Channel *>::iterator it = this->all_channels.begin();
       it != this->all_channels.end(); ++it) {
    delete *it;
  }

  // Clean up users
  for (std::map<std::string, Users *>::iterator it = this->all_users.begin();
       it != this->all_users.end(); ++it) {
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

int Server::addNewClient() {
	struct sockaddr_in clientAddress;
	socklen_t clientAddrSize = sizeof(clientAddress);
	int clientSocket = accept(this->serverSocket, (struct sockaddr *)&clientAddress, &clientAddrSize);
	if (clientSocket == -1) {
		std::cerr << "Error: Failed to accept incoming connection." << std::endl;
		return -1;
	}

	// possibly might have to set this socket as well ot non blocking before adding

	
	// Add the new client socket to the vector
    pollfd new_client_fd;
    new_client_fd.fd = clientSocket;
    new_client_fd.events = POLLIN;
    fds.push_back(new_client_fd);

	Users *user = createUser(clientSocket);
	if (!user) {
		std::cerr << "Error: failed to create user for the connected client." << std::endl;
		close(clientSocket);
		return -1;
	}

	if (user->getSocketDescriptor() != clientSocket || !user->getStatus()) {
		user->setSocketDescriptor(clientSocket);
		user->setStatus(1);
	}
	else
		this->all_users.insert(std::pair<std::string, Users *>(user->getUserName(), user));
	return 0;
}

void Server::start() {
	int activity;
	char	buffer[1024];
	ssize_t bytesReceived;
	Users	*user;
	
	// First adding the server socket to the fd list
	struct pollfd server_fd;
    server_fd.fd = this->serverSocket;
    server_fd.events = POLLIN;
    fds.push_back(server_fd);

	while (true) {
		// We listen for incoming connections or incoming anything really
		activity = poll(&(this->fds[0]), this->fds.size(), -1);	
		if (activity < 0) {
			std::cout << "Error: Poll error: " << errno << std::endl;
			continue ;
		}
		
		// Check for events occuring on server socket aka new clients connecting
		if (this->fds[0].revents & POLLIN) {
			if (addNewClient() == -1)
				continue ;
		}

		// Check for events occuring on already connected clients
		for (size_t i = 0; i < this->fds.size(); ++i) {
			if (this->fds[i].revents & POLLIN) {
				user = getUserByFd(this->fds[i].fd);
				if (!user)
					continue ;
				bytesReceived = recv(this->fds[i].fd, buffer, sizeof(buffer), 0);
				if (bytesReceived == 0) {
					close(this->fds[i].fd);
					this->fds.erase(this->fds.begin() + i);
					// set status to 0 and fd to -1
					user->setStatus(0);
					user->setSocketDescriptor(-1);
					std::cout << "Connection closed by @" << user->getUserName() << std::endl;
					continue ;
				}
				else if (bytesReceived < 0) {
					std::cerr << "Error receiving msg from @" << user->getUserName() << std::endl;
					continue ;	
				}
				std::string msg(buffer, bytesReceived - 1);
				std::cout << "@" << user->getUserName() << ": " << msg << std::endl;
			}
		}
	}
}

void Server::stop() { close(this->serverSocket); }

std::string Server::getPassword() const { return this->password; }

Users *Server::getUserByFd(int fd) {
	for (std::map<std::string, Users *>::iterator it = this->all_users.begin(); 
			it != this->all_users.end(); ++it) {
		if (it->second->getSocketDescriptor() == fd)
			return it->second;
	}
	return NULL;
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
      return;
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

void Server::sendToAll(const std::string &msg) {}

void Server::sendToChannel(const std::string &msg, Channel *channel) {}

void Server::sendToUser(const std::string &msg, const std::string uname) {}

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
