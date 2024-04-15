/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:20 by smallem           #+#    #+#             */
/*   Updated: 2024/04/12 16:47:36 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Users.hpp"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

class Users;

class Channel {
	private:
		std::string name;
		std::string topic;
		std::string modes;
		std::string password;
		uint8_t		modes;
		Users 		*owner;
		std::vector<Users *> userList;
		std::vector<Users *> operatorList;
	public:
		Channel(std::string name, std::string topic, std::string modes,
			std::string password, Users *owner);
		~Channel();
		Channel(const Channel &cp);
		Channel &operator=(const Channel &cp);

		std::string getName() const;
		std::string getTopic() const;
		std::string getModes() const;
		std::string getPassword() const;
		uint8_t		getModes() const;
		Users		*getOwner() const;
		std::vector<Users *> getUserList() const;
		std::vector<Users *> getOperatorList() const;
		
		void		setTopic(const std::string topic);
		void		setMode(const uint8_t mode);
		void		setPassword(const std::string pass);
		void		setOwner(Users *user);
		
		void		addUser(Users *user);
		void		addOperator(Users *user);
		void		deleteUser(Users *user);
		void		deleteOperator(Users *user);
		
		bool isOperator(const Users &user);
		bool isUser(const Users &user);
};

#endif
