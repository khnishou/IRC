/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:12:02 by smallem           #+#    #+#             */
/*   Updated: 2024/04/23 17:38:51 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <algorithm>

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Server::Server() {}

Server::Server(int port, std::string password) : port(port), password(password) {
	std::cout << "-------CREATING SERVER-------" << std::endl;
	this->state = START;
}

Server::Server(const Server &cp) { *this = cp; }

Server &Server::operator=(const Server &cp) {
  if (this != &cp)
    *this = cp;
  return *this;
}

Server::~Server() {
	// LOOK INTO THIS LATER, THIS CHANGED BECAUSE OF NEWLY ADDED SERVER STATE
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

SERVER_STATE Server::getState() const {
	return this->state;
}

size_t	Server::getNumberOfUsers() {
	return this->all_users.size();
}

std::string Server::getHost() const { 
	return this->host; 
}

std::string Server::getPassword() const { 
	return this->password;
}

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

Channel *Server::getChannel(const std::string cname) {
	for (std::vector<Channel *>::iterator it = this->all_channels.begin();
			it != this->all_channels.end(); ++it) {
		if ((*it)->getName() == cname)
			return *it;
	}
	return NULL;
}

//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

bool	Server::nickNameExists(std::string nname) {
	for (std::vector<Users *>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it) {
		if ((*it)->getNickName() == nname)
			return true;
	}
	return false;
}

void Server::removeUserFromServer(Users *user) {
	for (std::vector<pollfd>::iterator it = this->fds.begin(); it != this->fds.end(); it++) {
		if ((*it).fd == this->serverSocket)
			continue ;
		if ((*it).fd == user->getSocketDescriptor()) {
			close(user->getSocketDescriptor());
			this->fds.erase(it);
			break ;
		}
	}
	for (std::vector<Users *>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it) {
		if ((*it) == user) {
			this->all_users.erase(it);
			break ;
		}
	}
	for (std::vector<Channel *>::iterator it = this->all_channels.begin(); it != this->all_channels.end(); ++it) {
		if ((*it)->isOperator(user)) {
			(*it)->deleteOperator(user, NULL, this->getHost());
			break ;
		}
		else if ((*it)->isUser(user)) {
			(*it)->deleteUser(user, NULL, this->getHost());
			break ;
		}
	}
	delete user;
}
