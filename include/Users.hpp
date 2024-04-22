/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenhoci <ibenhoci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 12:04:22 by smallem           #+#    #+#             */
/*   Updated: 2024/04/22 15:28:39 by ibenhoci         ###   ########.fr       */
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
#include "Lib.hpp"

class Channel;

class Users {
	private:
		std::string 			hostName;
		std::string 			nickName;
		std::string 			userName;
		std::string 			modes;
		std::string				buffer;
		int 					permissionLevel;
		int						socketDescriptor;
		uint8_t					status;
		std::vector<Channel *>	invite_lst;
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
		std::string getBuffer() const;
		int 		getPermissionLevel() const;
		int			getSocketDescriptor() const;
		uint8_t		getStatus() const;
		// SETTERS
		void		setHostName(std::string hostname);
		void		setNickName(std::string nname);
		void		setUserName(std::string uname);
		void		setModes(std::string mode);
		void		setBuffer(std::string buf);
		void		setPermissionLevel(int permissionLevel);
		void		setSocketDescriptor(int sd);
		void		setStatus(uint8_t stat);
		void		unsetStatus(uint8_t stat);
		// OTHER
		void		invite(Channel *channel);
		bool		is_invited(std::string cname);
		void		remove_invite(std::string cname);

};

#endif
