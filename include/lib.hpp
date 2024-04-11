
#pragma once

#include <string>
#include <vector>

enum USER_STATE {
	INIT,
	LOGIN,
	REGISTERED,
	DISCONNECTED
};

struct Message {
    std::vector<std::string> tags;
    std::string source;
    std::string command;
    std::vector<std::string> parameters;
};

int skip_space(std::string str, int i);
int skip_arg(std::string str, int i);
