
#pragma once

#include <cstddef>
#include <string>

class Server {
private:
public:
  Server();
  Server(Server const &other);
  ~Server();
  Server &operator=(Server const &other);
};