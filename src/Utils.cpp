
#include "../include/Lib.hpp"

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
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
  while (str[j] && str[j] != ' ')
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

static uint8_t setTheUnset(uint8_t mode, uint8_t flag, uint8_t setUnset)
{
	if ((setUnset & FLAG_SET) && !(setUnset & FLAG_UNSET))
		return (mode | flag);
	else if (!(setUnset & FLAG_SET) && (setUnset & FLAG_UNSET))
		return (mode & ~flag);
	return (mode);
}

uint8_t initMode(std::string str, uint8_t mode)
{
	int		i;
	int		len;
	uint8_t	setUnset;
	uint8_t	flag;

	setUnset = 0;
	flag = 0;
	i = 0;
	len = str.length();
	while (i < str.length())
	{
		if (str[i] == '+')
			setUnset = FLAG_SET;
		else if (str[i] == '-')
			setUnset = FLAG_UNSET;
		else if (str[i] == 'i')
			mode = setTheUnset(mode, FLAG_I, setUnset);
		else if (str[i] == 't')
			mode = setTheUnset(mode, FLAG_T, setUnset);
		else if (str[i] == 'k')
			mode = setTheUnset(mode, FLAG_K, setUnset);
		else if (str[i] == 'o')
			mode = setTheUnset(mode, FLAG_O, setUnset);
		else if (str[i] == 'l')
			mode = setTheUnset(mode, FLAG_L, setUnset);
		else
			return (FLAG_ERR);
		i++;
	}
	return (mode);
}

std::string fill_vec(std::vector<std::string> param) {
	std::string res;

	for (std::vector<std::string>::iterator it = param.begin(); it != param.end(); ++it) {
		res += *it;
		if (it != param.end() - 1)
			res += " ";
	}
	return res;
}