
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
		std::string 			_name;
		std::string 			_topic;
		std::string				_password;
		int						_modes;
		int						_userLimit;
		std::vector<Users *>	_userList;
		std::vector<Users *>	_operatorList;

	public:
		Channel(std::string name);
		Channel(const Channel &cp);
		~Channel();
		Channel &operator=(const Channel &cp);

		std::string 			getName() const;
		std::string 			getTopic() const;
		std::string				getPassword() const;
		int						getModes() const;
		int						getUserLimit() const;
		std::vector<Users *>	getUserList() const;
		std::vector<Users *>	getOperatorList() const;
		
		void						setName(const std::string name);
		void						setTopic(const std::string topic);
		void						setPassword(const std::string pass);
		void						setMode(const int mode);
		void						setUserLimit(const int userlimit);
		
		void						addUser(Users *user);
		void						deleteUser(Users *removed, Users *remover, std::string host);

		void						addOperator(Users *user);
		void						deleteOperator(Users *removed, Users *remover, std::string host);

		bool 						isUser(const Users *user);
		bool 						isOperator(const Users *user);

		bool						haveMultOps();
		bool						haveUsers();
		bool						channelEmpty();
		std::string				convertMode();
		std::string				getNickNameList();

		void						broadcastMsg(std::string msg);
};

#endif
