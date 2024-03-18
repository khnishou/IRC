/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 13:53:50 by smallem           #+#    #+#             */
/*   Updated: 2024/03/18 14:12:00 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Users.hpp"

Users::Users(std::string nickName, std::string userName, std::string hostName,
			std::string realName, std::string modes, int permissionLevel,
			int socketDescriptor) : nickName(nickName), userName(userName),
			hostName(hostName), realName(realName), modes(modes),
			permissionLevel(permissionLevel), socketDescriptor(socketDescriptor) {
				
}

Users::~Users() {
}

Users::Users(const Users &cp) {
	this->nickName = cp.getNickName();
    this->userName = cp.getUserName();
    this->hostName = cp.getHostName();
    this->realName = cp.getRealName();
    this->modes = cp.getModes();
    this->permissionLevel = cp.getPermissionLevel();
    this->socketDescriptor = cp.getSocketDescriptor();
}

Users &Users::operator=(const Users &cp) {
	if (this != &cp) {
		this->nickName = cp.getNickName();
		this->userName = cp.getUserName();
		this->hostName = cp.getHostName();
		this->realName = cp.getRealName();
		this->modes = cp.getModes();
		this->permissionLevel = cp.getPermissionLevel();
		this->socketDescriptor = cp.getSocketDescriptor();	
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

std::string Users::getRealName() const {
	return this->realName;
}

std::string Users::getModes() const {
	return this->modes;
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

void Users::setPermissionLevel(int permissionLevel) {
	this->permissionLevel = permissionLevel;
}