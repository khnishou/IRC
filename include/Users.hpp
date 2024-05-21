
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
		std::string 				_hostName;
		std::string 				_nickName;
		std::string 				_userName;
		std::string					_real;
		std::string 				_modes;
		std::string					_cmdBuffer;
		std::string					_buffer;
		int 							_permissionLevel;
		int							_socketDescriptor;
		uint8_t						_status;
		std::vector<Channel *>	_invite_lst;

	public:
		Users(std::string hostname, int socketDescriptor);
		Users(const Users &cp);
		~Users();
		Users &operator=(const Users &cp);

		std::string		getHostName() const;
		std::string		getNickName() const;
		std::string		getUserName() const;
		std::string		getReal() const;
		std::string		getModes() const;
		std::string		getBuffer() const;
		int				getPermissionLevel() const;
		int				getSocketDescriptor() const;
		uint8_t			getStatus() const;
		std::string		getCmdBuffer() const;
	
		void			setHostName(std::string hostname);
		void			setNickName(std::string nname);
		void			setUserName(std::string uname);
		void			setReal(std::string real);
		void			setModes(std::string mode);
		void			setBuffer(std::string buf);
		void			setCmdBuffer(std::string buf);
		void			setPermissionLevel(int permissionLevel);
		void			setSocketDescriptor(int sd);
		void			setStatus(uint8_t stat);
		void			unsetStatus(uint8_t stat);
		void			clearBuff();
		void			clearCmdBuff();
	
		void			invite(Channel *channel);
		bool			is_invited(std::string cname);
		void			remove_invite(std::string cname);
};

#endif
