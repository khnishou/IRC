
#include "../include/Client.hpp"
#include <cstddef>

/******************************************************************************/
/*                          Constructor & Destructor                          */
/******************************************************************************/

Client::Client(const std::string &nickname, const std::string &hostRealName,
               const std::string &hostUsername,
               const std::string &connectedServer)
    : _hostRealName(hostRealName), _hostUsername(hostUsername),
      _connectedServer(connectedServer) {
  if (!isValidNickname(nickname)) {
    throw InvalidNickname();
  }
  this->_nickname = nickname;
}
Client &Client::operator=(Client const &other) {
  _nickname = other.getNickname();
  _hostRealName = other.getHostRealName();
  _hostUsername = other.getHostUsername();
  _connectedServer = other.getConnectedServer();
  return *this;
}
Client::Client(Client const &other) { *this = other; }
Client::~Client() {}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

std::string Client::getNickname() const { return _nickname; }
std::string Client::getHostRealName() const { return _hostRealName; }
std::string Client::getHostUsername() const { return _hostUsername; }
std::string Client::getConnectedServer() const { return _connectedServer; }

void Client::setNickname(const std::string &newNickname) {
  _nickname = newNickname;
}
void Client::setHostRealName(const std::string &newHostRealName) {
  _hostRealName = newHostRealName;
}
void Client::setHostUsername(const std::string &newHostUsername) {
  _hostUsername = newHostUsername;
}
void Client::setConnectedServer(const std::string &newConnectedServer) {
  _connectedServer = newConnectedServer;
}

//****************************************************************************//
//                           Error Message Function                           //
//****************************************************************************//

const char *Client::InvalidNickname::what() const throw() {
  return "Invalid Nickname.";
}

//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

bool Client::isValidNickname(const std::string &nickname) const {
  if (nickname.empty())
    return false;
  char firstChar = nickname[0];
  if (firstChar == '$' || firstChar == ':')
    return false;
  const std::string forbiddenChars = " ,*?!@";
  for (size_t i = 0; i < nickname.length(); ++i){
    if (forbiddenChars.find(nickname[i]) != std::string::npos)
      return false;
  }
  return true;
}