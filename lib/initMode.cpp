
#include "../include/lib.hpp"
#include <cstdint>
#include <iostream>

#define FLAG_I		(1 << 0)
#define FLAG_T		(1 << 1)
#define FLAG_K		(1 << 2)
#define FLAG_O		(1 << 3)
#define FLAG_L		(1 << 4)
#define FLAG_ERR	(1 << 7)

#define FLAG_SET	(1 << 0)
#define FLAG_UNSET	(1 << 1)

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
