
#include "../include/Lib.hpp"

bool checkSplit(const std::string& str, char del) {
  return !str.empty() && str.front() != del && str.back() != del;
}

std::vector<std::string> splitString(const std::string& str, char del)
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
				split.push_back(str.substr(start + 1, i - start));
		}
	}
	return split;
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

uint8_t setTheUnset(uint8_t mode, uint8_t flag, uint8_t setUnset)
{
	if ((setUnset & FLAG_SET) && !(setUnset & FLAG_UNSET))
		return (mode | flag);
	else if (!(setUnset & FLAG_SET) && (setUnset & FLAG_UNSET))
		return (mode & ~flag);
	return (mode);
}

std::string fill_vec(std::vector<std::string> param,std::vector<std::string>::iterator ite) {
	std::string res;

	for (std::vector<std::string>::iterator it = ite; it != param.end(); ++it) {
		res += *it;
		if (it != param.end() - 1)
			res += " ";
	}
	return res;
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
	while (str[i] && str[i] != '\r')
	{
		len = skip_arg(str, i);
		msg.parameters.push_back(str.substr(i, len));
		i += len;
		i = skip_space(str, i);
	}
   return msg;
}
