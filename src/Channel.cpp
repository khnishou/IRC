/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:56 by smallem           #+#    #+#             */
/*   Updated: 2024/04/11 17:36:19 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel(std::string name, std::string topic, std::string modes,
	std::string password, Users *owner) : name(name), topic(topic), modes(modes),
	password(password), owner(owner) 
{
	this->userList.push_back(owner);
	this->operatorList.push_back(owner);
}

Channel::~Channel() {
	
}

Channel::Channel(const Channel &cp) {
	this->name = cp.getName();
	this->topic = cp.getTopic();
	this->modes = cp.getModes();
	this->password = cp.getPassword();
	this->owner = cp.getOwner();
	this->userList = cp.getUserList();
	this->operatorList = cp.getOperatorList();
}

Channel &Channel::operator=(const Channel &cp) {
	if (this != &cp) {
		this->name = cp.getName();
		this->topic = cp.getTopic();
		this->modes = cp.getModes();
		this->password = cp.getPassword();
		this->owner = cp.getOwner();
		this->userList = cp.getUserList();
		this->operatorList = cp.getOperatorList();
	}
	return *this;
}

std::string Channel::getName() const {
	return this->name;	
}

std::string Channel::getTopic() const {
	return this->topic;
}

std::string Channel::getModes() const {
	return this->modes;
}

std::string Channel::getPassword() const {
	return this->password;
}

Users		*Channel::getOwner() const {
	return this->owner;
}

std::vector<Users *> Channel::getUserList() const {
	return this->userList;
}

std::vector<Users *> Channel::getOperatorList() const {
	return this->operatorList;	
}

bool Channel::isOperator(const Users &user) {
	for (std::vector<Users *>::iterator it = this->operatorList.begin();
			it != this->operatorList.end(); ++it) {
		if ((*it)->getNickName() == user.getNickName())
			return true;		
	}
	return false;
}

bool Channel::isUser(const Users &user) {
	for (std::vector<Users *>::iterator it = this->userList.begin();
			it != this->userList.end(); ++it) {
		if ((*it)->getNickName() == user.getNickName())
			return true;		
	}
	return false;
}