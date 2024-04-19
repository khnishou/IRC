/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:12:02 by smallem           #+#    #+#             */
/*   Updated: 2024/04/16 18:30:36 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <algorithm>

Server::Server() {}

Server::Server(int port, std::string password) : port(port), password(password) {
	std::cout << "-------CREATING SERVER-------" << std::endl;
	this->state = START;
}

Server::Server(const Server &cp) { *this = cp; }

Server &Server::operator=(const Server &cp) {
  if (this != &cp)
    *this = cp;
  return *this;
}

Server::~Server() {
	// LOOK INTO THIS LATER, THIS CHANGED BECAUSE OF NEWLY ADDED SERVER STATE
}

void Server::init() {
	// HERE WE ARE GOING TO CREATE OUR SERVER, OR RATHER INITIALIZE IT FOR FIRSTTIME/AGAIN
	char name[1024];

	// getting the hostname
	if (gethostname(name, sizeof(name)) == -1) {
		std::cerr << "Error: gethostname: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	this->host = std::string(name);
	
	// creating the socket
	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSocket == -1) {
		std::cerr << "Error: socket: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// setting socket options
	int opt = 1;
	if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			== -1) {
		std::cerr << "Error: setsockopt: " << std::strerror(errno) << std::endl;
		// might have to close socket first think on this later, same remqrks bellow
		exit(EXIT_FAILURE);	
	}

	// set socket to nonblocking mode
	if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// bind the socket to the address and port
	// there is probably a better way and more secure way to do this, if encountering 
	// issues with restarting server or manipulating server state look into this further 
	// and change accordingly, prolly need to loop through usable addresses and use the right one
	struct sockaddr_in serverAdr;
	std::memset(&serverAdr, 0, sizeof(serverAdr));
	serverAdr.sin_family = AF_INET;
	serverAdr.sin_addr.s_addr = INADDR_ANY;
	serverAdr.sin_port = htons(this->port);
	if (bind(this->serverSocket, (struct sockaddr *)&serverAdr, sizeof(serverAdr))
			== -1) {
		std::cerr << "Error: bind: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);			
	}

	// start listening to incoming connections, secnod arg is max allowed connections
	if (listen(this->serverSocket, 128) == -1) {
		std::cerr << "Error: listen: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	

	// add the server socket to the pollfd array 
	struct pollfd sfd;
	sfd.fd = this->serverSocket;
	sfd.events = POLLIN;
	this->fds.push_back(sfd);

	// now server is ready, consider later myb adding a default starting channel.
}

void Server::start() {
	int activity;
	Users	*user;

	std::cout << "-------------IRCSERVER ON-------------" << std::endl;
	this->state = ON;
	while (this->state == ON) {
		// We listen for incoming connections or incoming anything really
		activity = poll(&(this->fds[0]), this->fds.size(), -1);	
		if (activity < 0) {
			std::cout << "Error: poll: " << std::strerror(errno) << std::endl;
			// probably have some stuff to free here first
			exit(EXIT_FAILURE);
		}
		// Check for events occuring on already connected clients
		for (size_t i = 0; i < this->fds.size(); ++i) {
			if (this->fds[i].revents & POLLIN) {
				if (this->fds[i].fd == this->serverSocket) {
					if (addNewClient() == -1)
						continue ;
				}
				else {
					user = getUserByFd(this->fds[i].fd);
					if (user)
						handleMsg(user, i);
				}
				// above logic probably needs to be changed a little for outgoing
				// messages/errors/replies	
			}
		}
	}
	if (this->state == OFF)
		std::cout << "-------------IRCSERVER OFF-------------" << std::endl;
	else if (this->state == START)
		std::cout << "-------------IRCSERVER STARTING-------------" << std::endl;
}

void Server::stop() { close(this->serverSocket); }

SERVER_STATE Server::getState() const {
	return this->state;
}

int Server::addNewClient() {
	struct sockaddr_in clientAdr;
	socklen_t clientAddrSize = sizeof(clientAdr);
	int clientSocket = accept(this->serverSocket, (struct sockaddr *)&clientAdr, &clientAddrSize);
	if (clientSocket == -1) {
		std::cerr << "Error: accept: " << std::strerror(errno) << std::endl;
		return -1;
	}
	else
		std::cout << "NEW CONNECTION" << std::endl;

	// setting client socket to non-blocking not sure tho
	if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// get hostname info
	char name[1000];
	if (getnameinfo((struct sockaddr*)&clientAdr, clientAddrSize, name, sizeof(name), 0, 0, NI_NAMEREQD) != 0) {
		std::cerr << "Error: getnameinfo: " << std::strerror(errno) << std::endl;
	}
	
	// Add the new client socket to the vector
    pollfd new_client_fd;
    new_client_fd.fd = clientSocket;
    new_client_fd.events = POLLIN | POLLOUT;
    fds.push_back(new_client_fd);

	// create new user and add to list
	Users *user = new Users(std::string(name), clientSocket);
	if (!user) {
		std::cerr << "Error: failed to create user for the connected client." << std::endl;
		close(clientSocket);
		return -1;
	}	
	this->all_users.push_back(user);
	return 0;
}

void Server::handleMsg(Users *user, size_t i) {
	this->bytesReceived = recv(user->getSocketDescriptor(), this->buffer, sizeof(this->buffer), 0);

	if (this->bytesReceived <= 0) {
		if (this->bytesReceived == 0)
			std::cout << "Connection closed." << std::endl;
		else
			std::cerr << "Error: recv: " << std::strerror(errno) << std::endl;
		// check this close, might have to do more
		this->fds.erase(this->fds.begin() + i);
		close(user->getSocketDescriptor());
		return ;
	}
	else {
		//  N A T S U E Z G H O D
		// thos needs to be changed to handle split messages and incomplete messages
		std::string msg(buffer, bytesReceived - 1);
		std::cout << "Received: " << msg << std::endl;
	}
}


size_t	Server::getNumberOfUsers() {
	return this->all_users.size();
}

std::string Server::getPassword() const { return this->password; }

Users *Server::getUserByFd(int fd) {
	for (std::vector<Users *>::iterator it = this->all_users.begin(); 
			it != this->all_users.end(); ++it) {
		if ((*it)->getSocketDescriptor() == fd)
			return *it;
	}
	return NULL;
}

Users *Server::getUserByUn(const std::string username) {
	for (std::vector<Users *>::iterator it = this->all_users.begin();
			it != this->all_users.end(); ++it) {
		if ((*it)->getUserName() == username)
			return *it;
	}
	return NULL;
}

Channel *Server::getChannel(const std::string cname) {
	for (std::vector<Channel *>::iterator it = this->all_channels.begin();
			it != this->all_channels.end(); ++it) {
		if ((*it)->getName() == cname)
			return *it;
	}
	return NULL;
}

// ['@' <tags> SPACE] [':' <source> SPACE] <command> <parameters> <crlf>
Message Server::parsing(std::string str, Users user) {

    Message msg;

	int i;
	int len;

	i = skip_space(str, 0);
	while (str[i] && str[i] == '@')
	{
		len = skip_arg(str, i);
		msg.tags.push_back(str.substr(i + 1, len - 1));
		i += len;
		i = skip_space(str, i);
	}
	if (str[i] && str[i] == ':')
	{
		len = skip_arg(str, i);
        msg.source = str.substr(i + 1, len - 1);
		i += len;
		i = skip_space(str, i);
    }
	if (str[i])
	{
		len = skip_arg(str, i);
        msg.command = str.substr(i, len);
		i += len;
		i = skip_space(str, i);
    }
	while (str[i])
	{
		len = skip_arg(str, i);
		msg.parameters.push_back(str.substr(i, len));
		i += len;
		i = skip_space(str, i);
	}
    return msg;
}

//	Command: KICK
//	Parameters: <channel> <user> *( "," <user> ) [<comment>]
//				ERR_NEEDMOREPARAMS (461)
//				ERR_NOSUCHCHANNEL (403)
//				ERR_CHANOPRIVSNEEDED (482)
//				ERR_USERNOTINCHANNEL (441)
//				ERR_NOTONCHANNEL (442)
int Server::c_kick(std::vector<std::string> param, Users user) {
  std::vector<std::string> split;
	if (!(param.size() >= 3))
		return (461); // error ERR_NEEDMOREPARAMS (461)
  Channel *channel = getChannel(param[0]);
  if (!channel)
		return (403); // error ERR_NOSUCHCHANNEL (403)
  if (!channel->isOperator(user))
		return (482); // error ERR_CHANOPRIVSNEEDED (482)
	if (!channel->isUser(user))
		return (442); // error ERR_NOTONCHANNEL (442) // check 441 before 442
  split = splitString(param[1], ',');
  for (size_t i = 0; i < split.size(); i++) {
		Users *toKick = getUserByUn(split[i]);
		if (!toKick)
			return (441); // error ERR_USERNOTINCHANNEL (441) // check repetition
		if (!channel->isUser(*toKick))
			return (441); // error ERR_USERNOTINCHANNEL (441) // check repetition
    channel->deleteUser(toKick); // add deleteUser in channel class
		// kick message // add kick message for the current user <toKick>
	}
	return (0); // no error no message return
}

//	Command: INVITE
//	Parameters: <nickname> <channel>
//				RPL_INVITING (341)
//				ERR_NEEDMOREPARAMS (461)
//				ERR_NOSUCHCHANNEL (403)
//				ERR_NOTONCHANNEL (442)
//				ERR_CHANOPRIVSNEEDED (482)
//				ERR_USERONCHANNEL (443)
int Server::c_invite(std::vector<std::string> param, Users user) {
	if (!(param.size() >= 2))
		return (461); // error ERR_NEEDMOREPARAMS (461)
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (403); // error ERR_NOSUCHCHANNEL (403)
	if (!channel->isUser(user))
    	return (442); // error ERR_NOTONCHANNEL (442)
	if (!channel->isOperator(user))
    	return (482); // error ERR_CHANOPRIVSNEEDED (482)
  Users *toAdd = getUserByUn(param[0]);
	if (!channel->isUser(*toAdd))
		return (443); // error ERR_USERONCHANNEL (443)
  channel->addUser(toAdd);
  return (341);   // no error RPL_INVITING (341)
}

//	Command: TOPIC
//	Parameters: <channel> [<topic>]
//				ERR_NEEDMOREPARAMS (461)
//				ERR_NOSUCHCHANNEL (403)
//				ERR_NOTONCHANNEL (442)
//				ERR_CHANOPRIVSNEEDED (482)
//				RPL_NOTOPIC (331)
//				RPL_TOPIC (332)
//				RPL_TOPICWHOTIME (333)
int Server::c_topic(std::vector<std::string> param, Users user) {
  if (!(param.size() >= 1))
    return (461); // error ERR_NEEDMOREPARAMS (461)
  Channel *channel = getChannel(param[1]);
  if (!channel)
    return (403); // error ERR_NOSUCHCHANNEL (403)
  if (!channel->isUser(user))
    return (442); // error ERR_NOTONCHANNEL (442)
  if (!channel->isOperator(user))
    return (482); // error ERR_CHANOPRIVSNEEDED (482)
  if (param.size() == 1) {
    if (!channel->getTopic().empty())
      return (331); // no error RPL_NOTOPIC (331)
    else
      return (332); // no error RPL_TOPIC (332)
  }
  return (333); // no error RPL_TOPICWHOTIME (333)
}

//	Command: MODE
//	Parameters: <target> [<modestring> [<mode arguments>...]]
//              ERR_UMODEUNKNOWNFLAG (501)
//              ERR_NOSUCHCHANNEL (403)
//              RPL_CHANNELMODEIS (324)
//              RPL_CREATIONTIME (329)
//              ERR_CHANOPRIVSNEEDED (482)
//      i t k o l
int Server::c_mode(std::vector<std::string> param, Users user)
{
	uint8_t mode;
	int i;

	if (!(param.size() >= 2))
    	return (461); // error ERR_NEEDMOREPARAMS (461) // check
	i = 0;
	while (++i < param.size())
		mode = initMode(param[i], mode);
	if (mode & (1 << 7))
		return (501); // error ERR_UMODEUNKNOWNFLAG (501)
	Channel *channel = getChannel(param[0]);
	if (!channel)
    	return (403); // error ERR_NOSUCHCHANNEL (403)
	if (!channel->isUser(user))
    	return (442); // error ERR_NOTONCHANNEL (442)
	if (!channel->isOperator(user))
    	return (482); // error ERR_CHANOPRIVSNEEDED (482)
	channel->setMode(mode);
	return (0); // check should return an RPL value
}

int Server::c_pass(std::vector<std::string> param, Users user)
{
	if (!(param.size() >= 1))
    	return (461); // error ERR_NEEDMOREPARAMS (461)
	if (user.getStatus() & PASS_FLAG)
		return (462); // error ERR_ALREADYREGISTERED (462)
	if (param[0] != <password>)
		return (464); // error ERR_PASSWDMISMATCH (464)
	user.setStatus(PASS_FLAG);
	return (0); // check should return an RPL value
}

int Server::c_nick(std::vector<std::string> param, Users user)
{
	if (!(param.size() >= 1))
    	return (431); // error ERR_NONICKNAMEGIVEN (431)
	if (!isNickname(param[0]))
		return (432); // error ERR_ERRONEUSNICKNAME (432)
	if (<nickname in use>)
		return (433); // error ERR_NICKNAMEINUSE (433)
	// check add nick name
	user.setStatus(NICK_FLAG);
	return (0); // check should return an RPL value
}

void Server::executeCmd(Message msg, Users user) {
	// handle tag
	// handle source 
	if (msg.command == "PASS") {
    	c_pass(msg.parameters, user);
	}
	else if (msg.command == "NICK") {
    	c_nick(msg.parameters, user);
	}
	else if (msg.command == "USER") {
		// add username setting command
	}
	else if (msg.command == "JOIN") {
		// add join command
	}
	else if (msg.command == "KICK") {
    	c_kick(msg.parameters, user);
  	}
  	else if (msg.command == "INVITE") {
    	c_invite(msg.parameters, user);
  	} 
	else if (msg.command == "TOPIC") {
    	c_topic(msg.parameters, user);
  	} 
	else if (msg.command == "MODE") {
		c_mode(msg.parameters, user);
  	}
	else if (msg.command == "RESTART") {
		// server restart command
		// this command checks if the client has the right privileges, if he 
		//does restart the server aka just set server state to START instead of ON
	}
	else if (msg.command == "QUIT") {
		// quit command
		// this command allows a user to properly exit the server
		// need to check wether we simply close the socket or remove the user from channels
	}
	else {
		//ERR_UNKNOWNCOMMAND(this->host, user.getNickName(), cmd);
		// invalid cmd or whatever
	}
	
}
