/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:56 by smallem           #+#    #+#             */
/*   Updated: 2024/03/18 15:37:20 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel(std::string name, std::string topic, std::string modes,
	std::string password, Users *owner, std::vector<Users *> userList,
	std::vector<Users *> operatorList) : name(name), topic(topic), modes(modes),
	password(password), owner(owner), userList(userList), operatorList(operatorList)
{
	
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

void Channel::joinChannel(Users *user) {
	bool	flag = false;

	for (std::vector<Users *>::iterator it = this->userList.begin();
		it != this->userList.end(); ++it) {
		if (*it && (*it)->getNickName() == user->getNickName()) {
			flag = true;
			break ;
		}		
	}
	if (flag) {
		this->userList.push_back(user);
	}
	else
		std::cout << "User already in the channel!" << std::endl;
}

void Channel::leaveChannel(Users *user) {
	bool	flag = false;

	for (std::vector<Users *>::iterator it = this->userList.begin();
		it != this->userList.end(); ++it) {
		if (*it && (*it)->getNickName() == user->getNickName()) {
			this->userList.erase(it);
			std::cout << "User removed from channel!" << std::endl;
			return ;
		}
	}
	std::cout << "User is not in channel!" << std::endl;
}

void Channel::setMode(std::string mode) {
	this->modes = mode;	
}
