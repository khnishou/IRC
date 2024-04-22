
#include "../include/lib.hpp"
#include <iostream>
#include <cctype>
#include <algorithm>

bool isNickname(const std::string& nickname)
{
	int	i;

	if (nickname.empty() || nickname.length() > 30 || nickname[0] == '#' || nickname[0] == '&' || nickname[0] == ':'
			 || nickname[0] == '@' || isdigit(nickname[0]) || std::isspace(nickname[0]))
		return (false);
	i = 0;
	while (i < nickname.length())
	{
		if (!isalnum(nickname[i]) && nickname[i] != '\\' && nickname[i] != '|'
			&& nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '{'
			&& nickname[i] != '}' && nickname[i] != '-' && nickname[i] != '_')
			return (false);
		i++;
	}
	return (true);
}