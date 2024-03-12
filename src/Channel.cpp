
#include "../include/Channel.hpp"
#include <cstddef>

/******************************************************************************/
/*                          Constructor & Destructor                          */
/******************************************************************************/

Channel::Channel(const std::string &channelName) {
  if (!isValidChannelName(channelName)) {
    throw InvalidChannelName();
  }
  this->_channelName = channelName;
}
Channel &Channel::operator=(Channel const &other) {
  _channelName = other.getChannelName();
  return *this;
}
Channel::Channel(Channel const &other) { *this = other; }
Channel::~Channel() {}

//****************************************************************************//
//                              Accessor Methods                              //
//****************************************************************************//

std::string Channel::getChannelName() const { return _channelName; }

void Channel::setChannelName(const std::string &newChannelName) {
  _channelName = newChannelName;
}

//****************************************************************************//
//                           Error Message Function                           //
//****************************************************************************//

const char *Channel::InvalidChannelName::what() const throw() {
  return "Invalid channel name.";
}

//****************************************************************************//
//                               Other Function                               //
//****************************************************************************//

// Channel names are strings with specific prefix characters and restrictions:

// > They MUST start with one of the following characters
//   indicating the channel type:
//		hash ('#', 0x23) for standard channels known to all servers,
// 		ampersand ('&', 0x26) for server-specific or local channels.
// > They MUST NOT contain any of the following characters:
//		space (' ', 0x20), control G/BELL ('^G', 0x07), comma (',',
//0x2C) which 		is used as a list item separator by the protocol.

bool Channel::isValidChannelName(const std::string &channelName) const {
  if (channelName.empty())
    return false;
  if (channelName[0] != '#' && channelName[0] != '&')
    return false;
  const std::string forbiddenChars = " ,\x07";
  for (size_t i = 0; i < channelName.length(); ++i) {
    if (forbiddenChars.find(channelName[i]) != std::string::npos)
      return false;
  }
  return true;
}