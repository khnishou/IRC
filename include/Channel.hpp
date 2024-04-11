/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:20 by smallem           #+#    #+#             */
/*   Updated: 2024/04/11 17:34:34 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Users.hpp"
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
		Users		*getOwner() const;
		std::vector<Users *> getUserList() const;
		std::vector<Users *> getOperatorList() const;
		bool isOperator(const Users &user);
		bool isUser(const Users &user);
};

#endif
