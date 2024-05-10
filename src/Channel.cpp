
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
uint8_t					Channel::getModes() const { return this->_modes; }
int						Channel::getUserLimit() const { return this->_userLimit; }
std::vector<Users *>	Channel::getUserList() const { return this->_userList; }
std::vector<Users *>	Channel::getOperatorList() const { return this->_operatorList; }

void					Channel::setName(const std::string name) { this->_name = name; }
void					Channel::setTopic(const std::string topic) { this->_topic = topic; }
void					Channel::setPassword(const std::string pass) { this->_password = pass; }
void					Channel::setMode(const uint8_t mode) { this->_modes = mode; }
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

// tghese functions the two bellow need to send msg to everyoen on server saying who got kickced
void Channel::deleteUser(Users *removed, Users *remover, std::string host) {
	for (std::vector<Users *>::iterator it = this->_userList.begin();
			it != this->_userList.end(); ++it) {
		if ((*it)->getNickName() == removed->getNickName()) {
			this->_userList.erase(it);
			// here add check if remover exists meaning got kicked or voluntarily left
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
			// same note as above
			return ;
		}
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
	uint8_t m = this->_modes;
	std::string mode = "+";

	if (m & FLAG_I)
		mode += "i";
	if (m & FLAG_T)
		mode += "t";
	if (m & FLAG_K)
		mode += "k";
	if (m & FLAG_L)
		mode += "l";
	return mode;
}

std::string Channel::getNickNameList() {
	std::string nnlist = "";
	for (std::vector<Users *>::iterator it = this->_operatorList.begin(); it != this->_operatorList.end(); ++it) {
		nnlist += "@" + (*it)->getNickName();
		if (it + 1 != this->_operatorList.end())
			nnlist += " ";
	}
	for (std::vector<Users *>::iterator it = this->_userList.begin(); it != this->_userList.end(); ++it) {
		nnlist += "@" + (*it)->getNickName();
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
