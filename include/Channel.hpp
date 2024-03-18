/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:06:20 by smallem           #+#    #+#             */
/*   Updated: 2024/03/18 14:54:38 by smallem          ###   ########.fr       */
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
			std::string password, Users *owner, std::vector<Users *> userList,
			std::vector<Users *> operatorList);
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
		void joinChannel(Users *user);
		void leaveChannel(Users *user);
		void setMode(std::string mode);
};

#endif
