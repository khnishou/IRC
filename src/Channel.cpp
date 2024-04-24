/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:56 by smallem           #+#    #+#             */
/*   Updated: 2024/04/24 12:23:59 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Channel::Channel(std::string name) : name(name), topic(""), modes(0), password("") {
}

Channel::~Channel() {
	
}

Channel::Channel(const Channel &cp) {
	this->name = cp.getName();
	this->topic = cp.getTopic();
	this->modes = cp.getModes();
	this->password = cp.getPassword();
	this->userList = cp.getUserList();
	this->operatorList = cp.getOperatorList();
}

Channel &Channel::operator=(const Channel &cp) {
	if (this != &cp) {
		this->name = cp.getName();
		this->topic = cp.getTopic();
		this->modes = cp.getModes();
		this->password = cp.getPassword();
		this->userList = cp.getUserList();
		this->operatorList = cp.getOperatorList();
	}
	return *this;
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

std::string Channel::getName() const {
	return this->name;	
}

std::string Channel::getTopic() const {
	return this->topic;
}

uint8_t Channel::getModes() const {
	return this->modes;
}

std::string Channel::getPassword() const {
	return this->password;
}

void Channel::setTopic(const std::string topic) {
	this->topic = topic;
}

void Channel::setMode(const uint8_t mode) {
	this->modes = mode;
}

void Channel::setPassword(const std::string pass) {
	this->password = pass;
}

std::vector<Users *> Channel::getUserList() const {
	return this->userList;
}

std::vector<Users *> Channel::getOperatorList() const {
	return this->operatorList;	
}

//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

void Channel::addUser(Users *user) {
	for (std::vector<Users *>::iterator it = this->userList.begin();
			it != this->userList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName()) {
			return ;
		}
	}
	this->userList.push_back(user);
}

void Channel::addOperator(Users *user) {
	for (std::vector<Users *>::iterator it = this->operatorList.begin();
			it != this->operatorList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName()) {
			return ;
		}
	}
	this->operatorList.push_back(user);
}

// tghese functions the two bellow need to send msg to everyoen on server saying who got kickced
void Channel::deleteUser(Users *removed, Users *remover, std::string host) {
	for (std::vector<Users *>::iterator it = this->userList.begin();
			it != this->userList.end(); ++it) {
		if ((*it)->getNickName() == removed->getNickName()) {
			this->userList.erase(it);
			// here add check if remover exists meaning got kicked or voluntarily left
			return ;
		}
	}
}

void Channel::deleteOperator(Users *removed, Users *remover, std::string host) {
	for (std::vector<Users *>::iterator it = this->operatorList.begin();
			it != this->operatorList.end(); ++it) {
		if ((*it)->getNickName() == removed->getNickName()) {
			this->operatorList.erase(it);
			// same note as above
			return ;
		}
	}
	
}

bool Channel::isOperator(const Users *user) {
	for (std::vector<Users *>::iterator it = this->operatorList.begin();
			it != this->operatorList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName())
			return true;		
	}
	return false;
}

bool Channel::isUser(const Users *user) {
	for (std::vector<Users *>::iterator it = this->userList.begin();
			it != this->userList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName())
			return true;		
	}
	return false;
}
