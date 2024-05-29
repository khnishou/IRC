
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
#include "Lib.hpp"
#include "Bot.hpp"

#define CHANLIMIT	25
#define USERLIMIT	20
#define KICKLEN	255
#define TOPICLEN	307
#define NICKLEN	30
#define USERLEN	12

class Users;
class Channel;

// general guideline, need 2 update my reply sending system to the user, apparently server should be able to send
// multiple replies/error to a user on the same message
class Server {
	private:
		int								_port;
		std::string						_host;
		std::string						_password;
		SERVER_STATE					_state;
		int								_serverSocket;
		char								_buffer[1024];
		ssize_t							_bytesReceived;
		std::vector<Users *>			_allUsers;
		std::vector<Channel *>		_allChannels;
		std::vector<struct pollfd>	_fds;
		Bot								_bot;
		Server();
		
	public:
		Server(int port, std::string password);
		~Server();
		Server(const Server &cp);
		Server &operator=(const Server &cp);
		
		// Server state handling
		void init();
		void start();
		void stop();
		
		// GETTERS
		int								getPort() const;
		int								getServerSocket() const;
		SERVER_STATE					getState() const;
		Users*							getUserByUn(const std::string uname);
		Users*							getUserByNn(const std::string nname);
		Users*							getUserByFd(int fd);
		ssize_t							getBytesReceived() const;
		std::string						getHost() const;
		std::string						getPassword() const;
		std::vector<Users *>			getAllUsers() const;
		std::vector<Channel *>		getAllChannels() const;
		std::vector<struct pollfd>	getFds() const;
		Channel*							getChannel(const std::string cname);
		size_t 							getNumberOfUsers();
		std::vector<Channel *>		getChanList(Users *user);

		void								setState(SERVER_STATE state);
		void								setServerSocket(int serverSocket);
		void								setHost(std::string host);
		void								setBytesReceived(int BytesReceived);

		bool								nickNameExists(std::string nname);
		void								addChan(Channel *channel);
		void								addUser(Users *user);
		void								addPfds(struct pollfd sfd);
		void								removePfds(struct pollfd sfd);
		int								check_channel(Channel *channel);

		// message forwarding i guess
		void								send_2usr(int fd);
		void								sendAllChan(std::vector<Channel *> lst, std::string msg);
		bool								usrInChan(Users *user);
		int								addNewClient();
		void								removeUserFromServer(Users *user);
		void								handleMsg(Users *user, size_t i);
  		void								executeCmd(Message msg, Users *user);
		bool								allowed(Message msg, Users *user);

		// COMMANDS // check maybe the naming for the cmds is not allowed (c_cmd -> cCmd)
		void								c_cap(std::vector<std::string> param, Users *user);
		void								c_pass(std::vector<std::string> param, Users *user);
		void								c_ping(std::vector<std::string> param, Users *user);
		void								c_nick(std::vector<std::string> param, Users *user);
		void								c_user(std::vector<std::string> param, Users *user);
		void								c_join(std::vector<std::string> param, Users *user);
		void								c_part(std::vector<std::string> param, Users *user);
		void								c_invite(std::vector<std::string> param, Users *user);
		void								c_kick(std::vector<std::string> param, Users *user);
		void								c_topic(std::vector<std::string> param, Users *user);
		void								c_mode(std::vector<std::string> param, Users *user);
		void								c_privmsg(std::vector<std::string> param, Users *user);
		void								c_quit(std::vector<std::string> param, Users *user);
		void								c_bot(std::vector<std::string> param, Users *user);

		int								mode_o(int setUnset, int i, std::vector<std::string> param, Channel *channel, Users *user);

		void								c_dcc(std::vector<std::string> param, Users *user);
		// UTILS // for now
		int 								initMode(std::vector<std::string> param, int mode, Channel *channel, Users *user);
		int								checkMode(std::vector<std::string> param);
};

#endif

