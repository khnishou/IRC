#include "../include/Server.hpp"

void	Server::c_cap(std::vector<std::string> param, Users *user) {
	if (param.size() == 1) {
		if (param[0] == "END" && (user->getStatus() & CAPON_FLAG)) {
			user->setStatus(CAPOFF_FLAG);
			user->unsetStatus(CAPON_FLAG);
		}
		else if (param[0] == "LS") {
			user->setBuffer(RPL_CAP(getHost()));
			user->setStatus(CAPON_FLAG);
		}
	}
}

void	Server::c_part(std::vector<std::string> param, Users *user) {
	if (param.size() < 1)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PART"))); // (461)
	if (!checkCSplit(param[0], ','))
		user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
	std::vector<std::string> split = cSplitStr(param[0], ',');
	for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it)
	{
		Channel *channel = getChannel(*it);
		if (!channel)
			return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), *it)));
		if (!channel->isUser(user) && !channel->isOperator(user))
			return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName())));
		if (param.size() > 1)
			std::string reason = fill_vec(&param, param.begin() + 1);
		if (channel->isUser(user))
			channel->deleteUser(user, NULL, "");
		else
			channel->deleteOperator(user, NULL, "");
		// after removing this guy, update channel
		if (param.size() == 1)
			channel->broadcastMsg(RPL_PART(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), "gonee... :'( "));
		else
			channel->broadcastMsg(RPL_PART(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), param[1]));
	}
}

void	Server::c_ping(std::vector<std::string> param, Users *user) {
	if (param.size() == 1)
		return (user->setBuffer(RPL_PING(getHost(), param[0])));
	user->setBuffer(RPL_PING(getHost(), ""));
}

void	Server::c_kick(std::vector<std::string> param, Users *user) {
	std::vector<std::string> split;
	if (!(param.size() >= 3))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "KICK")));
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0])));
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName())));
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
	if (!checkCSplit(param[1], ','))
		user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
	split = cSplitStr(param[1], ',');
	std::string reason;
	if (param.size() < 4) 
		reason = "Good boy points have dropped to 0!";
	else
		reason = fill_vec(&param, param.begin() + 2).substr();
	for (size_t i = 0; i < split.size(); i++) {
		Users *toKick = getUserByNn(split[i]);
		if (!toKick)
			return (user->setBuffer(ERR_NOSUCHNICK(getHost(), user->getNickName(), split[i])));
		if (!channel->isUser(toKick) && !channel->isOperator(toKick))
			return (user->setBuffer(ERR_USERNOTINCHANNEL(getHost(), user->getNickName(), toKick->getNickName(), channel->getName())));
		if (channel->isUser(toKick))
			channel->deleteUser(toKick, user, getHost());
		else if (channel->isOperator(toKick))
			channel->deleteOperator(toKick, user, getHost());
		toKick->setBuffer(RPL_KICK(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), toKick->getNickName(), reason));
		channel->broadcastMsg(RPL_KICK(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), toKick->getNickName(), reason));
	}
}

void	Server::c_invite(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 2))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "INVITE")));
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0])));
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName())));
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName())));
  	Users *toAdd = getUserByNn(param[0]);
	if (toAdd == NULL)
		return (user->setBuffer(ERR_NOSUCHNICK(this->getHost(), user->getNickName(), param[0])));
	if (channel->isUser(toAdd) || channel->isOperator(toAdd))
		return (user->setBuffer(ERR_USERONCHANNEL(getHost(), user->getNickName(), channel->getName())));
	channel->addUser(toAdd);
	user->setBuffer(RPL_INVITING(getHost(), user->getNickName(), toAdd->getNickName(), channel->getName()));
	toAdd->setBuffer(RPL_INVITE(user->getNickName(), user->getUserName(), user->getHostName(), toAdd->getNickName(), channel->getName()));
	// prob broadcast on chan, also send him reply about chan topic
}

void	Server::c_topic(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "TOPIC")));
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0])));
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName())));
	if (!channel->isOperator(user) && (channel->getModes() & FLAG_T))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName())));
	if (param.size() == 1) {
		if (channel->getTopic().empty())
			return (user->setBuffer(RPL_NOTOPIC(getHost(), user->getNickName(), channel->getName())));
		else
			return (user->setBuffer(RPL_TOPIC(getHost(), user->getNickName(), channel->getName(), channel->getTopic())));
	}
	std::string top;
	top = fill_vec(&param, param.begin() + 1).substr(0, TOPICLEN);
	channel->setTopic(top);
	std::time_t currTime = std::time(NULL);
	std::string time = std::ctime(&currTime);
	channel->broadcastMsg(RPL_TOPIC(getHost(), user->getNickName(), channel->getName(), channel->getTopic()));
	channel->broadcastMsg(RPL_TOPICWHOTIME(getHost(), channel->getName(), user->getNickName(), time));
}

void	Server::c_pass(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PASS")));
	if (user->getStatus() & PASS_FLAG)
		return (user->setBuffer(ERR_ALREADYREGISTRED(user->getNickName())));
	if (param[0] != getPassword())
		return (user->setBuffer(ERR_PASSWDMISMATCH(user->getNickName())));
	user->setStatus(PASS_FLAG);
	user->setBuffer(RPL_PASS(user->getHostName()));
}

void	Server::c_nick(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1))
		return (user->setBuffer(ERR_NONICKNAMEGIVEN(user->getNickName())));
	if (!isNickname(param[0]) || param[0].length() > NICKLEN)
		return (user->setBuffer(ERR_ERRONEUSNICKNAME(getHost(), user->getNickName(), param[0])));
	if (nickNameExists(param[0]))
		return (user->setBuffer(ERR_NICKNAMEINUSE(getHost(), user->getNickName(), param[0])));
	if (!(user->getStatus() & NICK_FLAG)) {
		user->setStatus(NICK_FLAG);
		if (user->getStatus() & USER_FLAG)
			user->setBuffer(RPL_WELCOME(getHost(), param[0], user->getUserName(), user->getHostName()));
		else
			user->setBuffer(RPL_NICK(user->getHostName(), param[0]));
	}
	else
		sendAllChan(getChanList(user), RPL_NICKCHANGE(param[0], user->getUserName(), user->getHostName(), param[0]));
	user->setNickName(param[0]);
}

void	Server::c_user(std::vector<std::string> param, Users *user)
{
	std::string username;
	if (param.size() < 4)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "USER")));
	if (param[1] != "0" || param[2] != "*")
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "USER")));
	user->setReal(param[3]);
	if (param[0][0] == '~') // check ignore the '~' if available
		username = param[0].substr(1);
	else
		username = param[0].substr(0);
	if (username.empty())
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "USER"))); // error no username "USER ~" or (461)
	if (!(user->getStatus() & USER_FLAG)) {
		user->setStatus(USER_FLAG);
		if (user->getStatus() & NICK_FLAG)
			user->setBuffer(RPL_WELCOME(getHost(), user->getNickName(), username, user->getHostName()));
		else
			user->setBuffer(RPL_USER(user->getHostName(), param[0]));
	}
	else
		sendAllChan(getChanList(user), RPL_NICKCHANGE(user->getNickName(), username, user->getHostName(), param[0]));
	user->setUserName(username);
}

void	Server::c_join(std::vector<std::string> param, Users *user)
{
	int i_key;
	int i_chn;
	std::vector<std::string> keys;
	std::vector<std::string> channels;

	if (param.size() < 1 || param.size() > 2)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "JOIN")));
	if (!checkCSplit(param[0], ','))
		user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
	channels = cSplitStr(param[0], ',');
	if (param.size() == 2)
	{
		if (!checkCSplit(param[1], ','))
			user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
		keys = cSplitStr(param[1], ',');
	}
	i_key = 0;
	i_chn = 0;
	while (i_chn < channels.size())
	{
		Channel *channel = getChannel(channels[i_chn]);
		if (!channel) {
			if (getAllChannels().size() > CHANLIMIT) {
				user->setBuffer(ERR_TOOMANYCHANNELS(this->getHost(), user->getNickName(), channels[i_chn]));
				// check increments
				continue ;
			}
			else
			{
				channel = new Channel(channels[i_chn]); // check use a function instead
				addChan(channel);
				channel->addOperator(user);
				channel->broadcastMsg(RPL_JOIN(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName())); // look into this basically reply upon chan creation wasnt happening now should be
			}
		}
		else if (channel->getModes() & FLAG_I)
			user->setBuffer(ERR_INVITEONLYCHAN(getHost(), user->getNickName(), channel->getName()));
		else if (!(channel->getModes() & FLAG_K) ||
			(!(keys.empty()) && !(keys[i_key].empty()) && keys[i_key] == channel->getPassword()))
		{
			if (!(channel->getModes() & FLAG_L) || (channel->getUserList().size() < channel->getUserLimit())) {
				channel->addUser(user);
				channel->broadcastMsg(RPL_JOIN(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName()));
				if (!channel->getTopic().empty())
					user->setBuffer(RPL_TOPIC(this->getHost(), user->getNickName(), channel->getName(), channel->getTopic()));
					// check this might need to send NOTOPIC rply
				std::string reply = RPL_NAMREPLY(this->getHost(), user->getNickName(), channel->getName()) + channel->getNickNameList();
				user->setBuffer(reply);
				user->setBuffer(RPL_ENDOFNAMES(this->getHost(), user->getNickName(), channel->getName()));
			}
			else
				user->setBuffer(ERR_CHANNELISFULL(getHost(), user->getNickName(), channel->getName()));
		}
		else
			user->setBuffer(ERR_BADCHANNELKEY(getHost(), user->getNickName(), channel->getName()));
		i_key += ((channel->getModes() & FLAG_K) == FLAG_K);
		i_chn++;
	}
}
// here logic doesnt match above, we are silencing errors whereas earlier we quit the command altogether, look into this issue
void Server::c_privmsg(std::vector<std::string> param, Users *user) {
	if (param.size() < 2)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PRIVMSG")));
	
	Users *targ;
	Channel *targ_channel;
	std::vector<std::string> lst = cSplitStr(param[0], ',');
	for (std::vector<std::string>::iterator it = lst.begin(); it != lst.end(); ++it) {
		if ((*it)[0] == '#') {
			targ_channel = getChannel(*it);
			if (!targ_channel)
				user->setBuffer(ERR_NOSUCHCHANNEL(this->getHost(), user->getNickName(), *it));
			else if (!targ_channel->isOperator(user) && !targ_channel->isUser(user))
				user->setBuffer(ERR_CANNOTSENDTOCHAN(this->getHost(), user->getNickName(), targ_channel->getName()));
			else
				targ_channel->broadcastMsg(RPL_PRIVMSG(user->getNickName(), user->getUserName(), user->getHostName(), targ_channel->getName(), param[1]));
		}
		else {
			targ = getUserByNn(*it);
			if (!targ)
				user->setBuffer(ERR_NOSUCHNICK(this->getHost(), user->getNickName(), *it));
			targ->setBuffer(RPL_PRIVMSG(user->getNickName(), user->getUserName(), user->getHostName(), targ->getNickName(), param[1]));
		}
	}
}

void Server::c_quit(std::vector<std::string> param, Users *user) {
	user->setBuffer(RPL_QUIT(user->getNickName(), user->getUserName(), user->getHostName(), "QUIT: quitting..."));
	send_2usr(user->getSocketDescriptor());
	// also here need to notify eveery channel hes in, basically a part happening on them too
	removeUserFromServer(user);
}

void	Server::c_mode(std::vector<std::string> param, Users *user)
{
	uint8_t mode;
	int i;

	if (param.size() < 1)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "MODE")));
	i = 0;
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0])));
	if (param.size() < 2)
		return (user->setBuffer(RPL_CHANNELMODEIS(this->getHost(), user->getNickName(), channel->getName(), channel->convertMode()))); 
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName())));
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); 
	mode = checkMode(param);
	if (mode & ERR_PARAM)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "MODE"))); 
	if (mode & ERR_SYNTAX)
		return (user->setBuffer(ERR_UMODEUNKNOWNFLAG(getHost(), user->getNickName()))); //(501) // check error/RPL printed inside initMode
	mode = initMode(param, mode, channel, user);
	channel->setMode(mode);
	channel->broadcastMsg(RPL_CHANNELMODEIS(this->getHost(), user->getNickName(), channel->getName(), channel->convertMode()));
}

int Server::mode_i(uint8_t setUnset, Channel *channel, Users *user)
{
	if (setUnset & FLAG_SET)
		user->setBuffer(user->getNickName() + " invite set" + "\n"); // check RPL invite flag set
	else if (setUnset & FLAG_UNSET)
		user->setBuffer(user->getNickName() + " invite unset" + "\n"); // check RPL invite flag unset
	return (0);
}

int Server::mode_t(uint8_t setUnset, Channel *channel, Users *user)
{
	if (setUnset & FLAG_SET)
		user->setBuffer(user->getNickName() + " topic set" + "\n"); // check RPL topic private
	else if (setUnset & FLAG_UNSET)
		user->setBuffer(user->getNickName() + " topic unset" + "\n"); // check RPL topic public
	return (0);
}

int Server::mode_k(uint8_t setUnset, int i, int it, std::vector<std::string> param, Channel *channel, Users *user)
{
	if (setUnset & FLAG_SET)
	{
		if (i + ++it < param.size())
		{
			channel->setPassword(param[i + it++]);
			user->setBuffer(user->getNickName() + " password set to " + channel->getPassword() + "\n"); // check RPL password unset
		}
		else
			return (-1);
	}
	else if (setUnset & FLAG_UNSET)
		user->setBuffer(user->getNickName() + " password unset" + "\n"); // check RPL password unset
	return (it);
}

int Server::mode_l(uint8_t setUnset, int i, int it, std::vector<std::string> param, Channel *channel, Users *user)
{
	if (setUnset & FLAG_SET)
	{
		if (i + ++it < param.size() && isUint(param[i + it]))
		{
			channel->setUserLimit(std::stod(param[i + it]));
			user->setBuffer(user->getNickName() + " user limit set to " + param[i + it] + "\n"); // check RPL users limit set to <std::stod(param[i + it])>
		}
		else
			return (-1);
	}
	else if (setUnset & FLAG_UNSET)
		user->setBuffer(user->getNickName() + " user limit unset" + "\n"); // check RPL users limit unset
	return (it);
}

int Server::mode_o(uint8_t setUnset, int i, std::vector<std::string> param, Channel *channel, Users *user)
{
	std::vector<std::string> split = cSplitStr(param[i], ',');
	for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it)
	{
		Users *op = getUserByNn(*it);
		if (!op)
			std::cout << RED << "error : user not found" << DEFAULT << std::endl; // check error user not found
		else if (!channel->isUser(op) && !channel->isOperator(op))
			std::cout << RED << "error : user is not in the channel" << DEFAULT << std::endl; // check error user is not in the channel
		else if ((setUnset & FLAG_SET) && channel->isUser(op))
		{
			channel->addOperator(op);
			std::cout << GREEN << "RPL : add op to channel" << DEFAULT << std::endl; // check RPL add op to channel
		}
		else if (setUnset & FLAG_UNSET && channel->isOperator(op))
		{
			channel->deleteOperator(op, NULL, this->getHost());
			std::cout << GREEN << "RPL : delete op from channel" << DEFAULT << std::endl; // check RPL delete op from channel
		}
	}
	return (0);
}

uint8_t Server::checkMode(std::vector<std::string> param)
{
	size_t it;
	size_t j;
	size_t i;
	uint8_t setUnset;

	for (i = 1; i < param.size(); i++)
	{
		setUnset = 0;
		it = 0;
		for (j = 0; j < param[i].length(); j++)
		{
			if (!setUnset && param[i][j] == '+')
				setUnset = FLAG_SET;
			else if (!setUnset && param[i][j] == '-')
				setUnset = FLAG_UNSET;
			else if (setUnset && (param[i][j] == 'i' || param[i][j] == 't')) // type D
				;
			else if (setUnset && param[i][j] == 'k') // type C
			{
				if ((setUnset & FLAG_SET) && !(i + ++it < param.size()))
					return (ERR_PARAM);
			}
			else if (setUnset && param[i][j] == 'l') // type C
			{
				if ((setUnset & FLAG_SET))
				{
					++it;
					if (!(i + it < param.size()))
						return (ERR_PARAM); // check might change it into an inner error
					else if (!isUint(param[i + it]))
						return (ERR_SYNTAX);
				}
			}
			else if (setUnset && param[i][j] == 'o') // type B
			{
				if (i + ++it < param.size())
				{
					if (!checkCSplit(param[i + it], ','))
						return (ERR_SYNTAX); // check error syntax error
				}
				else
					return (ERR_PARAM); // check error param error
			}
			else
				return (ERR_SYNTAX); // check error syntax error
		}
		i += it;
	}
	return (0);
}

uint8_t Server::initMode(std::vector<std::string> param, uint8_t mode, Channel *channel, Users *user)
{
	size_t it;
	size_t j;
	size_t i;
	uint8_t setUnset;

	for (i = 1; i < param.size(); i++)
	{
		setUnset = 0;
		it = 0;
		for (j = 0; j < param[i].length(); j++)
		{
			if (!setUnset && param[i][j] == '+')
				setUnset = FLAG_SET;
			else if (!setUnset && param[i][j] == '-')
				setUnset = FLAG_UNSET;
			else if (setUnset && param[i][j] == 'i') // type D
			{
				mode_i(setUnset, channel, user);
				mode = setTheUnset(mode, FLAG_I, setUnset);
			}
				
			else if (setUnset && param[i][j] == 't') // type D
			{
				mode_t(setUnset, channel, user);
				mode = setTheUnset(mode, FLAG_T, setUnset);
			}
				
			else if (setUnset && param[i][j] == 'k') // type C
			{
				it = mode_k(setUnset, i, it, param, channel, user);
				if (it == -1)
					return (ERR_PARAM);
				mode = setTheUnset(mode, FLAG_K, setUnset);
			}
			else if (setUnset && param[i][j] == 'l') // type C
			{
				it = mode_l(setUnset, i, it, param, channel, user);
				if (it == -1)
					return (ERR_PARAM);
				mode = setTheUnset(mode, FLAG_L, setUnset);
			}
			else if (setUnset && param[i][j] == 'o') // type B
			{
				if (i + ++it < param.size())
				{
					mode_o(setUnset, i + it, param, channel, user);
				}
				else
					return (ERR_PARAM); // check error param error
			}
			else
				return (ERR_SYNTAX); // check error syntax error
		}
		i += it;
	}
	return (mode);
}

void Server::c_dcc(std::vector<std::string> param, Users *user) {
	if (param.size() != 4)
		user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "DCC"));
	if (param[0] != "SEND")
		user->setBuffer(ERR_UNKNOWNCOMMAND(this->getHost(), user->getNickName(), "DCC: " + param[0]));
	std::string fname = param[1];
	std::string hostname = param[2];
	int	port = std::atoi(param[3].c_str());
	if (hostname.empty() || fname.empty() || param[3].size() == 0 || port < 0 || port > 65535)
		return user->setBuffer(ERR_UNKNOWNERROR(this->getHost(), user->getNickName(), "DCC", "one or multiple invalid parameters"));
	Users *receiver = getUserByNn(hostname);
	if (!receiver)
		return user->setBuffer(ERR_NOSUCHNICK(this->getHost(), user->getNickName(), hostname));
	user->setBuffer(RPL_RECEIVEDTREQ(this->getHost(), receiver->getNickName(), receiver->getHostName()));
	receiver->setBuffer(RPL_TRANSFERREQ(this->getHost(), user->getNickName(), user->getHostName(), param[3], fname));
}

void Server::c_bot(std::vector<std::string> param, Users *user) {
	if (param.size() != 2)
		user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "BOT"));
	Channel *chan = NULL;
	Users *usr = NULL;

	if (param[0][0] == '#')
		chan = getChannel(param[0].substr(1));
	else if (param[0][0] == '@')
		usr = getUserByNn(param[0].substr(1));

	if (param[1] == "joke")
		this->_bot.tellJoke(usr, chan);
	else if (param[1] == "d20")
		this->_bot.d20(usr, chan);
	else if (param[1] == "coinflip")
		this->_bot.coinFlip(usr, chan);
	else if (param[1] == "eightball")
		this->_bot.eightBall(usr, chan);
}

