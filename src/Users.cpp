
#include "../include/Users.hpp"

//****************************************************************************//
//                          Constructor & Destructor                          //
//****************************************************************************//

Users::Users(std::string hostname, int socketDescriptor) : 
_hostName(hostname), _nickName("*"), _userName("*"), _real("*"), _modes("*"),  _cmdBuffer(""),
_buffer(""), _permissionLevel(0), _socketDescriptor(socketDescriptor), _status(0) { }

Users::Users(const Users &cp) : 
_hostName(cp.getHostName()), _nickName(cp.getNickName()), _userName(cp.getUserName()), _real(cp.getReal()),
_modes(cp.getModes()), _buffer(cp.getBuffer()), _permissionLevel(cp.getPermissionLevel()),
_socketDescriptor(cp.getSocketDescriptor()), _status(cp.getStatus()) { }

Users::~Users() { close(_socketDescriptor); }

Users &Users::operator=(const Users &cp) {
	if (this != &cp) {
		this->_hostName = cp.getHostName();
		this->_nickName = cp.getNickName();
		this->_userName = cp.getUserName();
		this->_real = cp.getReal();
		this->_modes = cp.getModes();
		this->_buffer = cp.getBuffer();
		this->_permissionLevel = cp.getPermissionLevel();
		this->_socketDescriptor = cp.getSocketDescriptor();
		this->_status = cp.getStatus();
	}
	return *this;
}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

std::string	Users::getNickName() const { return this->_nickName; }
std::string	Users::getUserName() const { return this->_userName; }
std::string	Users::getHostName() const { return this->_hostName; }
std::string Users::getReal() const { return this->_real; }
std::string	Users::getModes() const { return this->_modes; }
std::string	Users::getBuffer() const { return this->_buffer; }
std::string Users::getCmdBuffer() const { return this->_cmdBuffer; }
int		Users::getStatus() const { return this->_status; }
int			Users::getPermissionLevel() const { return this->_permissionLevel; }
int			Users::getSocketDescriptor() const { return this->_socketDescriptor; }

void		Users::setHostName(std::string hostname) { this->_hostName = hostname; }
void		Users::setNickName(std::string nname) { this->_nickName = nname; }
void		Users::setUserName(std::string uname) { this->_userName = uname; }
void		Users::setReal(std::string real) { this->_real = real; }
void		Users::setModes(std::string mode) { this->_modes = mode; }
void		Users::setBuffer(std::string buf) { this->_buffer += buf; }
void		Users::setCmdBuffer(std::string buf) { this->_cmdBuffer += buf; }
void		Users::setPermissionLevel(int permissionLevel) { this->_permissionLevel = permissionLevel; }
void		Users::setSocketDescriptor(int sd) { this->_socketDescriptor = sd; }
void		Users::setStatus(int stat) { this->_status |= stat; }
void		Users::unsetStatus(int stat) { this->_status &= ~stat; }
void		Users::clearBuff() { this->_buffer.clear(); }
void		Users::clearCmdBuff() { this->_cmdBuffer.clear(); }

