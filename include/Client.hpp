
#pragma once

#include <string>

class Client {
private:
  std::string   _nickname;
  std::string   _hostRealName;
  std::string   _hostUsername;
  std::string   _connectedServer;
  // int           _adminLevel;

  bool isValidNickname(const std::string &nickname) const;

public:
  Client(const std::string &nickname, const std::string &hostRealName,
         const std::string &hostUsername, const std::string &connectedServer);
  Client(Client const &other);
  ~Client();
  Client &operator=(Client const &other);

  std::string getNickname() const;
  std::string getHostRealName() const;
  std::string getHostUsername() const;
  std::string getConnectedServer() const;

  void setNickname(const std::string &newNickname);
  void setHostRealName(const std::string &newHostRealName);
  void setHostUsername(const std::string &newHostUsername);
  void setConnectedServer(const std::string &newConnectedServer);

  class InvalidNickname : public std::exception {
  public:
    const char *what() const throw();
  };
};