
#pragma once

#include <string>

class Channel {
private:
  std::string	_channelName;
//   int			_requiredAdminLevel;

  bool isValidChannelName(const std::string &ChannelName) const;

public:
  Channel(const std::string &ChannelName);
  Channel(Channel const &other);
  ~Channel();
  Channel &operator=(Channel const &other);

  std::string getChannelName() const;

  void setChannelName(const std::string &newChannelName);

  class InvalidChannelName : public std::exception {
  public:
    const char *what() const throw();
  };
};