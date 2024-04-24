/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 13:53:50 by smallem           #+#    #+#             */
/*   Updated: 2024/04/24 11:34:27 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Users.hpp"

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Users::Users(std::string hostname, int socketDescriptor) : 
hostName(hostname), nickName("default"), userName("default"), modes("default"), 
buffer(""), permissionLevel(0), socketDescriptor(socketDescriptor), status(0)
{}

Users::~Users() {
}

Users::Users(const Users &cp) {
	this->hostName = cp.getHostName();
	this->nickName = cp.getNickName();
   this->userName = cp.getUserName();
   this->modes = cp.getModes();
	this->buffer = cp.getBuffer();
   this->permissionLevel = cp.getPermissionLevel();
   this->socketDescriptor = cp.getSocketDescriptor();
	this->status = cp.getStatus();
}

Users &Users::operator=(const Users &cp) {
	if (this != &cp) {
		this->hostName = cp.getHostName();
		this->nickName = cp.getNickName();
		this->userName = cp.getUserName();
		this->modes = cp.getModes();
		this->buffer = cp.getBuffer();
		this->permissionLevel = cp.getPermissionLevel();
		this->socketDescriptor = cp.getSocketDescriptor();
		this->status = cp.getStatus();
	}
	return *this;
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

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

std::string Users::getBuffer() const {
	return this->buffer;
}

uint8_t Users::getStatus() const {
	return this->status;
}

int Users::getPermissionLevel() const {
	return this->permissionLevel;
}

int	Users::getSocketDescriptor() const {
	return this->socketDescriptor;
}

void Users::setHostName(std::string hostname) {
	this->hostName = hostname;
}

void Users::setNickName(std::string nname) {
	this->nickName = nname;
}

void Users::setUserName(std::string uname) {
	this->userName = uname;
}

void Users::setModes(std::string mode) {
	this->modes = mode;
}

void Users::setBuffer(std::string buf) {
	this->buffer = buf;
}

void Users::setPermissionLevel(int permissionLevel) {
	this->permissionLevel = permissionLevel;
}

void Users::setSocketDescriptor(int sd) {
	this->socketDescriptor = sd;
}

void Users::setStatus(uint8_t stat) {
	this->status |= stat;
}

void Users::unsetStatus(uint8_t stat) {
	this->status &= ~stat;
}

//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

void Users::invite(Channel *channel) {
	if (this->is_invited(channel->getName()) == false)
		this->invite_lst.push_back(channel);
}

bool Users::is_invited(std::string cname) {
	for (std::vector<Channel *>::iterator it = this->invite_lst.begin(); it != this->invite_lst.end(); ++it) {
		if ((*it)->getName() == cname)
			return true;
	}
	return false;
}

void Users::remove_invite(std::string cname) {
	if (this->is_invited(cname) == true) {
		for (std::vector<Channel *>::iterator it = this->invite_lst.begin();
				it != this->invite_lst.end(); ++it) {
			if ((*it)->getName() == cname) {
				this->invite_lst.erase(it);
				return ;
			}
		}
	}
}
