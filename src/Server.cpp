
#include "../include/Server.hpp"
#include <algorithm>

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Server::Server() {}

Server::Server(int port, std::string password) : _port(port), _password(password), _buffer("") {
	std::cout << "-------CREATING SERVER-------" << std::endl;
	this->_state = START;
}

Server::Server(const Server &cp) { *this = cp; }

Server &Server::operator=(const Server &cp) {
  if (this != &cp)
    *this = cp;
  return *this;
}

Server::~Server() {
	close(_serverSocket);
	for (std::vector<Users *>::iterator it = _allUsers.begin(); it != _allUsers.end(); ++it)
		delete *it;
	for (std::vector<Channel *>::iterator it = _allChannels.begin(); it != _allChannels.end(); ++it)
		delete *it;
	_allUsers.clear();
	_allChannels.clear();
	_fds.clear();
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

int Server::getPort() const { return this->_port; }
SERVER_STATE Server::getState() const { return this->_state; }
int	Server::getServerSocket() const { return this->_serverSocket; }
size_t	Server::getNumberOfUsers() { return this->_allUsers.size(); }
ssize_t Server::getBytesReceived() const { return this->_bytesReceived; }
std::string Server::getHost() const { return this->_host; }
std::string Server::getPassword() const { return this->_password; }
std::vector<Users *> Server::getAllUsers() const { return this->_allUsers; }
std::vector<Channel *> Server::getAllChannels() const { return this->_allChannels; }
std::vector<struct pollfd>	Server::getFds() const { return this->_fds; }

Users *Server::getUserByFd(int fd) {
	for (std::vector<Users *>::iterator it = this->_allUsers.begin(); 
			it != this->_allUsers.end(); ++it) {
		if ((*it)->getSocketDescriptor() == fd)
			return *it;
	}
	return NULL;
}

Users *Server::getUserByUn(const std::string uname) {
	for (std::vector<Users *>::iterator it = this->_allUsers.begin();
			it != this->_allUsers.end(); ++it) {
		if ((*it)->getUserName() == uname)
			return *it;
	}
	return NULL;
}

Users *Server::getUserByNn(const std::string nname) {
	for (std::vector<Users *>::iterator it = this->_allUsers.begin();
			it != this->_allUsers.end(); ++it) {
		if ((*it)->getNickName() == nname)
			return *it;
	}
	return NULL;
}

Channel *Server::getChannel(const std::string cname) {
	for (std::vector<Channel *>::iterator it = this->_allChannels.begin();
			it != this->_allChannels.end(); ++it) {
		if ((*it)->getName() == cname)
			return *it;
	}
	return NULL;
}

std::vector<Channel *> Server::getChanList(Users *user) {
	std::vector<Channel *> lst;
	for (std::vector<Channel *>::iterator it = this->_allChannels.begin(); it != this->_allChannels.end(); ++it) {
		if ((*it)->isOperator(user) || (*it)->isUser(user))
			lst.push_back(*it);
	}
	return lst;
}

void	Server::setState(SERVER_STATE state) { this->_state = state; }
void	Server::setServerSocket(int serverSocket) { this->_serverSocket = serverSocket; }
void	Server::setHost(std::string host) { this->_host = host; }
void	Server::setBytesReceived(int BytesReceived) { this->_bytesReceived = BytesReceived; }

void	Server::addChan(Channel *channel) {	this->_allChannels.push_back(channel); }
void	Server::addUser(Users *user) { this->_allUsers.push_back(user); }
void	Server::addPfds(struct pollfd sfd) {this->_fds.push_back(sfd); }
void	Server::removePfds(struct pollfd sfd) {
	for (std::vector<struct pollfd>::iterator it = this->_fds.begin(); it != this->_fds.end(); ++it) {
		if ((*it).fd == sfd.fd && ((*it).events == sfd.events)) {
			this->_fds.erase(it);
			return ;
		}
	}
}
//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

bool	Server::nickNameExists(std::string nname) {
	for (std::vector<Users *>::iterator it = this->_allUsers.begin(); it != this->_allUsers.end(); ++it) {
		if ((*it)->getNickName() == nname)
			return true;
	}
	return false;
}

void Server::removeUserFromServer(Users *user) {
	for (std::vector<pollfd>::iterator it = this->_fds.begin(); it != this->_fds.end(); it++) {
		if ((*it).fd == this->_serverSocket)
			continue ;
		if ((*it).fd == user->getSocketDescriptor()) {
			this->_fds.erase(it);
			break ;
		}
	}
	for (std::vector<Users *>::iterator it = this->_allUsers.begin(); it != this->_allUsers.end(); ++it) {
		if ((*it) == user) {
			this->_allUsers.erase(it);
			break ;
		}
	}
	for (std::vector<Channel *>::iterator it = this->_allChannels.begin(); it != this->_allChannels.end(); ++it) {
		if ((*it)->isOperator(user)) {
			(*it)->deleteOperator(user, NULL, this->getHost());
			if (!check_channel(*it))
				break ;
			(*it)->broadcastMsg(RPL_PART(user->getNickName(), user->getUserName(), user->getHostName(), (*it)->getName(), "just left..."));
			break ;
		}
		else if ((*it)->isUser(user)) {
			(*it)->deleteUser(user, NULL, this->getHost());
			(*it)->broadcastMsg(RPL_PART(user->getNickName(), user->getUserName(), user->getHostName(), (*it)->getName(), "just left..."));
			break ;
		}
	}
	delete user;
}

