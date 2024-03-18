/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 12:04:22 by smallem           #+#    #+#             */
/*   Updated: 2024/03/18 14:11:35 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USERS_HPP
# define USERS_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

class Users {
	private:
		std::string nickName;
		std::string userName;
		std::string hostName;
		std::string realName;
		std::string modes;
		int 		permissionLevel;
		int			socketDescriptor;
	public:
		Users(std::string nickName, std::string userName, std::string hostName,
			std::string realName, std::string modes, int permissionLevel,
			int socketDescriptor);
		~Users();
		Users(const Users &cp);
		Users &operator=(const Users &cp);

		std::string getNickName() const;
		std::string getUserName() const;
		std::string getHostName() const;
		std::string getRealName() const;
		std::string getModes() const;
		int 		getPermissionLevel() const;
		int			getSocketDescriptor() const;
		void		setModes(std::string mode);
		void		setPermissionLevel(int permissionLevel);
};

#endif