/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 12:04:22 by smallem           #+#    #+#             */
/*   Updated: 2024/04/16 14:27:21 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USERS_HPP
# define USERS_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include "Channel.hpp"
#include "lib.hpp"

class Channel;

class Users {
	private:
		std::string hostName;
		std::string nickName;
		std::string userName;
		std::string modes;
		int 		permissionLevel;
		int			socketDescriptor;
		USER_STATE	status;
		std::vector<Channel *> invite_lst;
	public:
		Users(std::string hostname, int socketDescriptor);
		~Users();
		Users(const Users &cp);
		Users &operator=(const Users &cp);

		// GETTERS
		std::string getHostName() const;
		std::string getNickName() const;
		std::string getUserName() const;
		std::string getModes() const;
		int 		getPermissionLevel() const;
		int			getSocketDescriptor() const;
		USER_STATE	getStatus() const;
		// SETTERS
		void		setHostName(std::string hostname);
		void		setNickName(std::string nname);
		void		setUserName(std::string uname);
		void		setModes(std::string mode);
		void		setPermissionLevel(int permissionLevel);
		void		setSocketDescriptor(int sd);
		void		setStatus(USER_STATE stat);
		// OTHER
		void		invite(Channel *channel);
		bool		is_invited(std::string cname);
		void		remove_invite(std::string cname);

};

#endif
