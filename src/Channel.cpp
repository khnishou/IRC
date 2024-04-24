
#include "../include/Channel.hpp"

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Channel::Channel(std::string name) : _name(name), _topic(""), _password(""), _modes(0) { }

Channel::Channel(const Channel &cp) : _name(cp.getName()), _topic(cp.getTopic()),
_password(cp.getPassword()), _modes(cp.getModes()), _userList(cp.getUserList()),
_operatorList(cp.getOperatorList()) { }

Channel::~Channel() { }

Channel &Channel::operator=(const Channel &cp) {
	if (this != &cp) {
		this->_name = cp.getName();
		this->_topic = cp.getTopic();
		this->_modes = cp.getModes();
		this->_password = cp.getPassword();
		this->_userList = cp.getUserList();
		this->_operatorList = cp.getOperatorList();
	}
	return *this;
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

std::string Channel::getName() const { return this->_name; }
std::string Channel::getTopic() const { return this->_topic; }
std::string Channel::getPassword() const { return this->_password; }
uint8_t Channel::getModes() const { return this->_modes; }
std::vector<Users *> Channel::getUserList() const { return this->_userList; }
std::vector<Users *> Channel::getOperatorList() const { return this->_operatorList; }

void Channel::setName(const std::string name) { this->_name = name; }
void Channel::setTopic(const std::string topic) { this->_topic = topic; }
void Channel::setMode(const uint8_t mode) { this->_modes = mode; }
void Channel::setPassword(const std::string pass) { this->_password = pass; }

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
