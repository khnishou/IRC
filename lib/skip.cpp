
#include "../include/lib.hpp"

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