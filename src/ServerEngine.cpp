#include "../include/Server.hpp"

void Server::init() {
	char name[1024];

	if (gethostname(name, sizeof(name)) == -1) {
		std::cerr << "Error: gethostname: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	setHost(std::string(name));
	
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1) {
		std::cerr << "Error: socket: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			== -1) {
		std::cerr << "Error: setsockopt: " << strerror(errno) << std::endl;
		close(this->_serverSocket);
		exit(EXIT_FAILURE);
	}

	if (fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl: " << strerror(errno) << std::endl;
		close(this->_serverSocket);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverAdr;
	std::memset(&serverAdr, 0, sizeof(serverAdr));
	serverAdr.sin_family = AF_INET;
	serverAdr.sin_addr.s_addr = INADDR_ANY;
	serverAdr.sin_port = htons(getPort());
	if (bind(this->_serverSocket, (struct sockaddr *)&serverAdr, sizeof(serverAdr))
			== -1) {
		std::cerr << "Error: bind: " << strerror(errno) << std::endl;
		close(this->_serverSocket);
		exit(EXIT_FAILURE);			
	}

	if (listen(this->_serverSocket, 128) == -1) {
		std::cerr << "Error: listen: " << strerror(errno) << std::endl;
		close(this->_serverSocket);
		exit(EXIT_FAILURE);
	}
	
	struct pollfd sfd;
	std::memset(&sfd, 0, sizeof(sfd));
	sfd.fd = this->_serverSocket;
	sfd.events = POLLIN;
	addPfds(sfd);
}

void Server::start() {
	int activity;
	Users	*user;

	std::cout << "-------------IRCSERVER ON-------------" << std::endl;
	setState(ON);
	while (getState() == ON) {
		activity = poll(&(this->_fds[0]), this->_fds.size(), -1);
		if (activity < 0) {
			std::cout << "Error: poll: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}

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
			if (i < this->_fds.size() && (this->_fds[i].revents & POLLOUT))
				send_2usr(this->_fds[i].fd);
		}
	}
	if (getState() == OFF)
		std::cout << "-------------IRCSERVER OFF-------------" << std::endl;
	else if (getState() == START)
		std::cout << "-------------IRCSERVER STARTING-------------" << std::endl;
}

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
	// if (allowed(msg, user) == false) {
	// 	user->setBuffer(ERR_NOTREGISTERED(getHost()));
	// 	return ;
	// }
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
	else if (msg.command == "BOT")
		c_bot(msg.parameters, user);
	else if (msg.command == "DCC")
		c_dcc(msg.parameters, user);
	else
		user->setBuffer(ERR_UNKNOWNCOMMAND(getHost(), user->getNickName(), msg.command));
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
	std::cout << GREEN << "SENT: " << DEFAULT << msg << std::endl;
	user->clearBuff();
}

int Server::addNewClient() {
	struct sockaddr_in clientAdr;
	socklen_t clientAddrSize = sizeof(clientAdr);
	int clientSocket = accept(this->_serverSocket, (struct sockaddr *)&clientAdr, &clientAddrSize);
	if (clientSocket == -1) {
		std::cerr << "Error: accept: " << strerror(errno) << std::endl;
		return -1;
	}
	else
		std::cout << BLUE << "NEW CONNECTION" << DEFAULT << std::endl;

	if (fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl: " << strerror(errno) << std::endl;
		return -1;
	}

	char name[1000];
	if (getnameinfo((struct sockaddr*)&clientAdr, clientAddrSize, name, sizeof(name), 0, 0, NI_NUMERICHOST) != 0) {
		std::cerr << "Error: getnameinfo: " << strerror(errno) << std::endl;
		return -1;
	}
	
	pollfd new_client_fd;
	std::memset(&new_client_fd, 0, sizeof(new_client_fd));
   new_client_fd.fd = clientSocket;
   new_client_fd.events = POLLIN | POLLOUT;
   addPfds(new_client_fd);
	
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
			std::cerr << "Error: recv: " << strerror(errno) << std::endl;
		removeUserFromServer(user);
	}
	else {
		std::string msg(_buffer, getBytesReceived());
		std::memset(this->_buffer, '\0', sizeof(this->_buffer));
		user->setCmdBuffer(msg);
		if (!checkStrSplit(user->getCmdBuffer(), "\r\n"))
			return ;
		std::vector<std::string> vec = strSplitStr(user->getCmdBuffer(), "\r\n");
		std::string tmp = user->getCmdBuffer().substr(user->getCmdBuffer().rfind("\r\n") + 2);
		user->clearCmdBuff();
		user->setCmdBuffer(tmp);
		for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it) {
			Message cont = parsing(*it);
			executeCmd(cont, user);
			std::cout << YELLOW << "Received: " << DEFAULT << *it << std::endl;
		}
	}
}
