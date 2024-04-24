/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:20 by smallem           #+#    #+#             */
/*   Updated: 2024/04/24 12:22:44 by smallem          ###   ########.fr       */
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
		std::string 			name;
		std::string 			topic;
		uint8_t					modes;
		std::string				password;
		std::vector<Users *> userList;
		std::vector<Users *> operatorList;
	public:
		Channel(std::string name);
		~Channel();
		Channel(const Channel &cp);
		Channel &operator=(const Channel &cp);

		// GETTERS
		std::string 			getName() const;
		std::string 			getTopic() const;
		uint8_t					getModes() const;
		std::string				getPassword() const;
		std::vector<Users *> getUserList() const;
		std::vector<Users *> getOperatorList() const;
		
		// SETTERS
		void						setTopic(const std::string topic);
		void						setMode(const uint8_t mode);
		void						setPassword(const std::string pass);
		
		// UTILS
		void						addUser(Users *user);
		void						addOperator(Users *user);
		void						deleteUser(Users *removed, Users *remover, std::string host);
		void						deleteOperator(Users *removed, Users *remover, std::string host);
		bool 						isOperator(const Users *user);
		bool 						isUser(const Users *user);
};

#endif
