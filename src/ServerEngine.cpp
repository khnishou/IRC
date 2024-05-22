#include "../include/Server.hpp"

void Server::init() {
	// HERE WE ARE GOING TO CREATE OUR SERVER, OR RATHER INITIALIZE IT FOR FIRSTTIME/AGAIN
	char name[1024];

	// getting the hostname
	if (gethostname(name, sizeof(name)) == -1) {
		std::cerr << "Error: gethostname: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	setHost(std::string(name));
	
	// creating the socket
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1) {
		std::cerr << "Error: socket: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// setting socket options
	int opt = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			== -1) {
		std::cerr << "Error: setsockopt: " << std::strerror(errno) << std::endl;
		// might have to close socket first think on this later, same remqrks bellow
		exit(EXIT_FAILURE);	
	}

	// set socket to nonblocking mode
	if (fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
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
	serverAdr.sin_port = htons(getPort());
	if (bind(this->_serverSocket, (struct sockaddr *)&serverAdr, sizeof(serverAdr))
			== -1) {
		std::cerr << "Error: bind: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);			
	}

	// start listening to incoming connections, secnod arg is max allowed connections
	if (listen(this->_serverSocket, 128) == -1) {
		std::cerr << "Error: listen: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	

	// add the server socket to the pollfd array 
	struct pollfd sfd;
	sfd.fd = this->_serverSocket;
	sfd.events = POLLIN;
	addPfds(sfd);
	
	// now server is ready, consider later myb adding a default starting channel.
	Channel *general = new Channel("#GENERAL");
	// general->setMode(); figure out which modes to open this channel with
	general->setTopic("--GENERAL IRCSERV CHANNEL--");
	getAllChannels().push_back(general);
	addChan(general);
}

void Server::start() {
	int activity;
	Users	*user;

	std::cout << "-------------IRCSERVER ON-------------" << std::endl;
	setState(ON);
	while (getState() == ON) {
		// We listen for incoming connections or incoming anything really
		activity = poll(&(this->_fds[0]), this->_fds.size(), -1);	
		if (activity < 0) {
			std::cout << "Error: poll: " << std::strerror(errno) << std::endl;
			// probably have some stuff to free here first
			exit(EXIT_FAILURE);
		}
		// Check for events occuring on already connected clients
		for (size_t i = 0; i < this->_fds.size(); ++i) {
			if (this->_fds[i].revents & POLLIN) {
				if (this->_fds[i].fd == this->_serverSocket) {
					if (addNewClient() == -1)
						continue ;
				}
				else {
					user = getUserByFd(this->_fds[i].fd);
					if (user)
						handleMsg(user, i);
				}
			}
			// send outgoing error messages/replies or even private messages
			if (i < this->_fds.size() && this->_fds[i].revents & POLLOUT)
				send_2usr(this->_fds[i].fd);
		}
	}
	if (getState() == OFF)
		std::cout << "-------------IRCSERVER OFF-------------" << std::endl;
	else if (getState() == START)
		std::cout << "-------------IRCSERVER STARTING-------------" << std::endl;
}

void Server::stop() { close(this->_serverSocket); }

bool Server::allowed(Message msg, Users *user) {
	
	if (msg.command == "CAP")
		return (true);
	else if (user->getStatus() & CAPOFF_FLAG)
	{
		if (msg.command == "PASS")
			return (true);
		else if (user->getStatus() & PASS_FLAG)
		{
			if (msg.command == "NICK" || msg.command == "USER")
				return (true);
			else if ((user->getStatus() & NICK_FLAG) && (user->getStatus() & USER_FLAG))
				return (true);
			else
				return (false);
		}
		else
			return (false);
	}
	else
		return (false);
}

void Server::executeCmd(Message msg, Users *user) {
	// handle tag
	if (allowed(msg, user) == false) {
		user->setBuffer(ERR_NOTREGISTERED(getHost()));
		return ;
	}
	if (msg.command == "CAP")
		c_cap(msg.parameters, user);
	else if (msg.command == "PASS")
    	c_pass(msg.parameters, user);
	else if (msg.command == "NICK")
    	c_nick(msg.parameters, user);
	else if (msg.command == "USER")
    	c_user(msg.parameters, user);
	else if (msg.command == "PING")
    	c_ping(msg.parameters, user);
	else if (msg.command == "JOIN")
    	c_join(msg.parameters, user);
	else if (msg.command == "PART")
		c_part(msg.parameters, user);
	else if (msg.command == "KICK")
    	c_kick(msg.parameters, user);
  	else if (msg.command == "INVITE")
    	c_invite(msg.parameters, user);
	else if (msg.command == "TOPIC")
    	c_topic(msg.parameters, user);
	else if (msg.command == "MODE")
		c_mode(msg.parameters, user);
	else if (msg.command == "PRIVMSG")
		c_privmsg(msg.parameters, user);
	else if (msg.command == "QUIT")
		c_quit(msg.parameters, user);
	else if (msg.command == "RESTART")
		c_restart(msg.parameters, user);
	else if (msg.command == "BOT")
		c_bot(msg.parameters, user);
	else if (msg.command == "DCC")
		c_dcc(msg.parameters, user);
	else
		user->setBuffer(ERR_UNKNOWNCOMMAND(getHost(), user->getNickName(), msg.command));
	user->clearCmdBuff();
}

void Server::sendAllChan(std::vector<Channel *> lst, std::string msg) {
	for (std::vector<Channel *>::iterator it = lst.begin(); it != lst.end(); ++it)
		(*it)->broadcastMsg(msg);
}

void	Server::send_2usr(int fd) {
	if (fd == this->_serverSocket || fd == -1)
		return ;
	Users *user = getUserByFd(fd);
	std::string msg = user->getBuffer();
	if (msg.empty())
		return ;
	if (send(fd, msg.c_str(), msg.size(), 0) != (long)msg.size())
		std::cerr << "Error: send: did not send all data" << std::endl;
	user->clearBuff();
}

int Server::addNewClient() {
	struct sockaddr_in clientAdr;
	socklen_t clientAddrSize = sizeof(clientAdr);
	int clientSocket = accept(this->_serverSocket, (struct sockaddr *)&clientAdr, &clientAddrSize);
	if (clientSocket == -1) {
		std::cerr << "Error: accept: " << std::strerror(errno) << std::endl;
		return -1;
	}
	else
		std::cout << "NEW CONNECTION" << std::endl;

	// setting client socket to non-blocking not sure tho
	if (fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// get hostname info
	char name[1000];
	if (getnameinfo((struct sockaddr*)&clientAdr, clientAddrSize, name, sizeof(name), 0, 0, NI_NUMERICHOST) != 0) {
		std::cerr << "Error: getnameinfo: " << std::strerror(errno) << std::endl;
	}
	// Add the new client socket to the vector
    pollfd new_client_fd;
    new_client_fd.fd = clientSocket;
    new_client_fd.events = POLLIN | POLLOUT;
    addPfds(new_client_fd);
	// create new user and add to list
	Users *user = new Users(std::string(name), clientSocket);
	if (!user) {
		std::cerr << "Error: failed to create user for the connected client." << std::endl;
		close(clientSocket);
		removePfds(new_client_fd);
		return -1;
	}
	addUser(user);
	return 0;
}

void Server::handleMsg(Users *user, size_t i) {
	setBytesReceived(recv(user->getSocketDescriptor(), this->_buffer, sizeof(this->_buffer), 0));
	if (getBytesReceived() <= 0) {
		if (getBytesReceived() == 0)
			std::cout << "Connection closed." << std::endl;
		else
			std::cerr << "Error: recv: " << std::strerror(errno) << std::endl;
		removeUserFromServer(user);
	}
	else {
		std::string msg(_buffer, getBytesReceived());
		if (!checkStrSplit(msg, "\r\n")) {
			user->setCmdBuffer(msg);
			return ;
		}
		std::vector<std::string> vec = strSplitStr(user->getCmdBuffer() + msg, "\r\n");
		for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it) {
			Message cont = parsing(*it);
			executeCmd(cont, user);
		// 	// send reply if buffer not empty to client 
			std::cout << "Received: " << *it << std::endl;
			std::cout << "Reply code: " << user->getBuffer() << std::endl;
		}
	}
}
