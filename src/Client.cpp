
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

// Nicknames are non-empty strings with the following restrictions:

// > They MUST NOT contain any of the following characters:
//		space (' ', 0x20),comma (',', 0x2C), asterisk ('*', 0x2A),
// 		question mark ('?', 0x3F), exclamation mark ('!', 0x21),
//		at sign ('@', 0x40).
// > They MUST NOT start with any of the following characters:
// 		dollar ('$', 0x24), colon (':', 0x3A)
// > They MUST NOT start with a character listed as a channel type,
//		channel membership prefix, or prefix listed in the IRCv3
// 		multi-prefix Extension
// > They SHOULD NOT contain any dot character ('.', 0x2E).

bool Client::isValidNickname(const std::string &nickname) const {
  if (nickname.empty())
    return false;
  char firstChar = nickname[0];
  if (firstChar == '$' || firstChar == ':')
    return false;
  const std::string forbiddenChars = " ,*?!@";
  for (size_t i = 0; i < nickname.length(); ++i) {
    if (forbiddenChars.find(nickname[i]) != std::string::npos)
      return false;
  }
  return true;
}