
#include "../include/Channel.hpp"

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Channel::Channel(std::string name) : _name(name), _topic(""), _password(""), _modes(0), _userLimit(20) { }

Channel::Channel(const Channel &cp) : _name(cp.getName()), _topic(cp.getTopic()),
_password(cp.getPassword()), _modes(cp.getModes()), _userLimit(cp.getUserLimit()), _userList(cp.getUserList()),
_operatorList(cp.getOperatorList()) { }

Channel::~Channel() { }

Channel &Channel::operator=(const Channel &cp) {
	if (this != &cp) {
		this->_name = cp.getName();
		this->_topic = cp.getTopic();
		this->_modes = cp.getModes();
		this->_userLimit = cp.getUserLimit();
		this->_password = cp.getPassword();
		this->_userList = cp.getUserList();
		this->_operatorList = cp.getOperatorList();
	}
	return *this;
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

std::string				Channel::getName() const { return this->_name; }
std::string				Channel::getTopic() const { return this->_topic; }
std::string				Channel::getPassword() const { return this->_password; }
int					Channel::getModes() const { return this->_modes; }
int						Channel::getUserLimit() const { return this->_userLimit; }
std::vector<Users *>	Channel::getUserList() const { return this->_userList; }
std::vector<Users *>	Channel::getOperatorList() const { return this->_operatorList; }

void					Channel::setName(const std::string name) { this->_name = name; }
void					Channel::setTopic(const std::string topic) { this->_topic = topic; }
void					Channel::setPassword(const std::string pass) { this->_password = pass; }
void					Channel::setMode(const int mode) { this->_modes = mode; }
void					Channel::setUserLimit(const int userLimit) { this->_userLimit = userLimit; }

//****************************************************************************//
//                           User Accessor Methods                            //
//****************************************************************************//

void Channel::addUser(Users *user) {
	for (std::vector<Users *>::iterator it = this->_userList.begin();
			it != this->_userList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName()) {
			return ;
		}
	}
	this->_userList.push_back(user);
}

void Channel::deleteUser(Users *removed, Users *remover, std::string host) {
	for (std::vector<Users *>::iterator it = this->_userList.begin();
			it != this->_userList.end(); ++it) {
		if ((*it)->getNickName() == removed->getNickName()) {
			this->_userList.erase(it);
			return ;
		}
	}
}

//****************************************************************************//
//                          Operator Accessor Methods                         //
//****************************************************************************//

void Channel::addOperator(Users *user) {
	for (std::vector<Users *>::iterator it = this->_operatorList.begin();
			it != this->_operatorList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName()) {
			return ;
		}
	}
	this->_operatorList.push_back(user);
}

void Channel::deleteOperator(Users *removed, Users *remover, std::string host) {
	for (std::vector<Users *>::iterator it = this->_operatorList.begin();
			it != this->_operatorList.end(); ++it) {
		if ((*it)->getNickName() == removed->getNickName()) {
			this->_operatorList.erase(it);
			break ;
		}
	}
// dprintf(2, YELLOW"con = %d\n", );
	if (this->_operatorList.empty() && !this->_userList.empty()) {
		Users *user = *(this->_userList.begin());
		this->_userList.erase(this->_userList.begin());
		this->_operatorList.push_back(user);
		user->setBuffer(RPL_YOUREOPER(host, this->getName()));
	}
}

//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

bool Channel::isUser(const Users *user) {
	for (std::vector<Users *>::iterator it = this->_userList.begin();
			it != this->_userList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName())
			return true;		
	}
	return false;
}

bool Channel::isOperator(const Users *user) {
	for (std::vector<Users *>::iterator it = this->_operatorList.begin();
			it != this->_operatorList.end(); ++it) {
		if ((*it)->getNickName() == user->getNickName())
			return true;		
	}
	return false;
}

bool	Channel::haveMultOps() {
	unsigned int count = this->_operatorList.size();

	if (count > 1)
		return true;
	return false;
}

bool Channel::haveUsers() {
	if (this->_userList.size() == 0)
		return false;
	return true;
}

bool	Channel::channelEmpty() {
	if (this->_userList.size() + this->_operatorList.size() == 0)
		return true;
	return false;
}

std::string Channel::convertMode() {
	int m = this->_modes;
	std::string mode = "+";
	std::string no_mode = "-";

	if (m & FLAG_I)
		mode += "i";
	else
		no_mode += "i";
	if (m & FLAG_T)
		mode += "t";
	else
		no_mode += "t";
	if (m & FLAG_K)
		mode += "k";
	else
		no_mode += "k";
	if (m & FLAG_L)
		mode += "l";
	else
		no_mode += "l";
	std::string tmp = mode + "\t" + no_mode;
	return tmp;
}

std::string Channel::getNickNameList() {
	std::string nnlist = "";
	for (std::vector<Users *>::iterator it = this->_operatorList.begin(); it != this->_operatorList.end(); ++it) {
		nnlist += " @" + (*it)->getNickName();
		if (it + 1 != this->_operatorList.end())
			nnlist += " ";
	}
	for (std::vector<Users *>::iterator it = this->_userList.begin(); it != this->_userList.end(); ++it) {
		nnlist += " @" + (*it)->getNickName();
		if (it + 1 != this->_userList.end())
			nnlist += " ";
	}
	return nnlist;
}

void	Channel::broadcastMsg(std::string msg) {
	for (std::vector<Users *>::iterator it = this->_userList.begin(); it != this->_userList.end(); ++it)
		(*it)->setBuffer(msg);
	for (std::vector<Users *>::iterator it = this->_operatorList.begin(); it != this->_operatorList.end(); ++it)
		(*it)->setBuffer(msg);
}
