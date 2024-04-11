/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 13:53:50 by smallem           #+#    #+#             */
/*   Updated: 2024/04/11 17:31:45 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Users.hpp"

Users::Users(std::string nickName, std::string userName, std::string hostname,
		std::string modes,  int permissionLevel, int socketDescriptor)
	: nickName(nickName), userName(userName), hostName(hostname), modes(modes), 
	permissionLevel(permissionLevel), socketDescriptor(socketDescriptor),
	status(1) {	
}

Users::~Users() {
}

Users::Users(const Users &cp) {
	this->nickName = cp.getNickName();
    this->userName = cp.getUserName();
	this->hostName = cp.getHostName();
    this->modes = cp.getModes();
    this->permissionLevel = cp.getPermissionLevel();
    this->socketDescriptor = cp.getSocketDescriptor();
	this->status = 1;
}

Users &Users::operator=(const Users &cp) {
	if (this != &cp) {
		this->nickName = cp.getNickName();
		this->userName = cp.getUserName();
		this->hostName = cp.getHostName();
		this->modes = cp.getModes();
		this->permissionLevel = cp.getPermissionLevel();
		this->socketDescriptor = cp.getSocketDescriptor();
		this->status = cp.getStatus();
	}
	return *this;
}

std::string Users::getNickName() const {
	return this->nickName;	
}

std::string Users::getUserName() const {
	return this->userName;
}

std::string Users::getHostName() const {
	return this->hostName;
}

std::string Users::getModes() const {
	return this->modes;
}

int Users::getStatus() const {
	return this->status;
}

int Users::getPermissionLevel() const {
	return this->permissionLevel;
}

int	Users::getSocketDescriptor() const {
	return this->socketDescriptor;
}

void Users::setModes(std::string mode) {
	this->modes = mode;
}

void Users::setHostName(std::string hostname) {
	this->hostName = hostname;
}

void Users::setStatus(int stat) {
	this->status = stat;
}

void Users::setPermissionLevel(int permissionLevel) {
	this->permissionLevel = permissionLevel;
}

void Users::setSocketDescriptor(int sd) {
	this->socketDescriptor = sd;
}
