
#include "../include/Lib.hpp"
#include <iostream>

bool checkCSplit(const std::string& str, char del)
{
  	if (str.empty() || str.c_str()[0] == del || str.c_str()[str.length() - 1] == del)
		return (false);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (i && str[i] == del && str[i - 1] == del)
			return (false);
	}
	return (true);
}

std::vector<std::string> cSplitStr(const std::string& str, char del)
{
    std::vector<std::string> split;
	int start = 0;

	if (!str.empty())
	{
		int i = 0;

		while (str[i])
		{
			while (str[i] && str[i] == del)
				i++;
			start = i;
			while (str[i] && str[i] != del)
				i++;
			if (start != i)
				split.push_back(str.substr(start, i - start));
		}
	}
	return split;
}

bool checkStrSplit(const std::string& str, std::string del)
{
	if (str.empty() || del.empty())
		return (false);
	if (str.rfind(del) == std::string::npos)
		return (false);
	if (str.substr(str.rfind(del)) != del)
			return (false);
	return (true);
}

std::vector<std::string> strSplitStr(const std::string& str, std::string del)
{
    std::vector<std::string> split;

	if (!str.empty() && !del.empty())
	{
		int start = 0;
		int i = 0;
		while (str[i])
		{
			while (str[i] && str.substr(i, del.length()) != del)
				i++;
			if (str[i])
				split.push_back(str.substr(start, i - start));
			while (str[i] && str.substr(i, del.length()) == del)
				i++;
			start = (i + del.length() - 1);
		}
	}
	return (split);
}

int skip_space(std::string str, int i)
{
	while (str[i] && str[i] == ' ') // is_space
		i++;
	return (i);
}

int skip_arg(std::string str, int i)
{
	int j;
	j = i;
  	while (str[j] && str[j] != ' ' && str[j] != '\r')
		j++;
  	return (j - i);
}

bool isNickname(const std::string& nickname)
{
	int	i;

	if (nickname.empty() || nickname.length() > 30 || nickname[0] == '#' || nickname[0] == '&' || nickname[0] == ':'
			 || nickname[0] == '@' || isdigit(nickname[0]) || std::isspace(nickname[0]))
		return (false);
	i = 0;
	while (i < nickname.length() - 1)
	{
		if (!isalnum(nickname[i]) && nickname[i] != '\\' && nickname[i] != '|'
			&& nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '{'
			&& nickname[i] != '}' && nickname[i] != '-' && nickname[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

int setTheUnset(int mode, int flag, int setUnset)
{
	if ((setUnset & FLAG_SET) && !(setUnset & FLAG_UNSET))
		return (mode | flag);
	else if (!(setUnset & FLAG_SET) && (setUnset & FLAG_UNSET))
		return (mode & ~flag);
	return (mode);
}

std::string fill_vec(std::vector<std::string> *param, std::vector<std::string>::iterator ite) {
	std::string res;

	for (std::vector<std::string>::iterator it = ite; it != param->end(); ++it) {
		res += *it;
		if (it != param->end() - 1)
			res += " ";
	}
	return res;
}

static int paramLen(std::string cmd)
{
	if (cmd == "CAP")
		return (1);
	else if (cmd == "PASS")
		return (1);
	else if (cmd == "NICK")
		return (0);
	else if (cmd == "USER")
		return (4);
	else if (cmd == "PING")
		return (1);
	else if (cmd == "JOIN")
		return (1);
	else if (cmd == "PART")
		return (1);
	else if (cmd == "KICK")
		return (2);
  	else if (cmd == "INVITE")
		return (2);
	else if (cmd == "TOPIC")
		return (1);
	else if (cmd == "MODE")
		return (-1);
	else if (cmd == "PRIVMSG")
		return (1);
	else if (cmd == "QUIT")
		return (0);
	else if (cmd == "RESTART")
		return (0);
	else if (cmd == "BOT")
		return (2);
	else if (cmd == "DCC")
		return (4);
	return (-1);
}

Message parsing(std::string str) {

   Message msg;

	int i;
	int len;

	i = skip_space(str, 0);
	while (str[i] && str[i] == '@')
	{
		len = skip_arg(str, i);
		msg.tags.push_back(str.substr(i + 1, len - 1));
		i += len;
		i = skip_space(str, i);
	}
	if (str[i] && str[i] == ':')
	{
		len = skip_arg(str, i);
        msg.source = str.substr(i + 1, len - 1);
		i += len;
		i = skip_space(str, i);
	}
	if (str[i])
	{
		len = skip_arg(str, i);
      	msg.command = str.substr(i, len);
		i += len;
		i = skip_space(str, i);
	}
	int argsLen = paramLen(msg.command);
	while (str[i] && str[i] != '\r')
	{
		len = skip_arg(str, i);
		if (argsLen)
		{
			msg.parameters.push_back(str.substr(i, len));
			argsLen--;
		}
		else
		{
			msg.parameters.push_back(str.substr(i));
			break ;
		}
		i += len;
		i = skip_space(str, i);
	}
   return msg;
}

bool isUint(const std::string& str) // check the function again
{
	if (str.empty() || str[0] == '-' || str[0] == '+' || str.length() > 4)
		return (false);
	for (size_t i = 0; i < str.length(); i++)
		if (!isdigit(str[i]))
    		return (false);
	return (strtod(str.c_str(), NULL) >= 0);
}
