/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenhoci <ibenhoci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:11:00 by smallem           #+#    #+#             */
/*   Updated: 2024/04/22 11:59:17 by ibenhoci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include "Users.hpp"
#include "Channel.hpp"
#include "lib.hpp"

class Users;
class Channel;

class Server {
	private:
		int							port;
		std::string					host;
		std::string					password;
		SERVER_STATE				state;
		int							serverSocket;
		char							buffer[1024];
		ssize_t						bytesReceived;
		std::vector<Users *>		all_users;
		std::vector<Channel *>		all_channels;
		std::vector<struct pollfd>	fds;
		Server();
		
	public:
		Server(int port, std::string password);
		~Server();
		Server(const Server &cp);
		Server &operator=(const Server &cp);
		
		void init();
		void start();
		void stop();
		
		SERVER_STATE getState() const;
		// create user delete user
		int addNewClient();
		void handleMsg(Users *user, size_t i);
  		void executeCmd(Message msg, Users *user);

		Users *getUserByUn(const std::string uname);
		Users *getUserByFd(int fd);
		bool	nickNameExists(std::string nname); // implement
		
		Channel *getChannel(const std::string cname);
		
		size_t getNumberOfUsers();
		// message forwarding i guess
	
		std::string getPassword() const;
 		Message parsing(std::string str, Users *user);

		int c_kick(std::vector<std::string> param, Users *user);
		int c_invite(std::vector<std::string> param, Users *user);
		int c_topic(std::vector<std::string> param, Users *user);
		int c_mode(std::vector<std::string> param, Users *user);
		int c_pass(std::vector<std::string> param, Users *user);
		int c_nick(std::vector<std::string> param, Users *user);
		int c_user(std::vector<std::string> param, Users *user);
		int c_join(std::vector<std::string> param, Users *user);

		// need to add following to start testing-- also probably new user cant do 
		// any other command until they do these.
		// cmd pass checks if the user is in the right state to prompt for password
		// cmd nick checks user state then prompts for nickname
		// cmd user checks user state then prompts for username
		// structure is something like this:
		// pass needs to be ran, check if right user state and check password and send the right reply
		// then nickname, also check if right state then send reply and same for user
		
		// PASS--> USER STATE NEEDS TO BE INIT
		// NICK/USER --> USERSTATE NEEDS TO BE LOGIN
		// after that userstate should be set to registered and from that point onwards cant use any of these
		


		// unrelated but kidna is to above situation,, due to how server socket and client sockets work
		// and the factthat everything is non blocking, we need to add some sort of buffer either on server
		// or on client to make it so that if there is a reply/error or anything it needs ot be saved on a buffer
		// and then we can send the replies accordingly.
		
};

#endif
