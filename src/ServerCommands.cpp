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
	if (checkSplit(param[0], ','))
		user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
	std::vector<std::string> split = splitString(param[0], ',');
	for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it)
	{
		Channel *channel = getChannel(*it);
		if (!channel)
			return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), *it))); // (403)
		if (!channel->isUser(user) && !channel->isOperator(user))
			return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442)
		if (param.size() > 1)
			std::string reason = fill_vec(&param, param.begin() + 1);
		if (channel->isUser(user))
			channel->deleteUser(user, NULL, "");
		else
			channel->deleteOperator(user, NULL, "");
		// after removing this guy, update channel
		channel->broadcastMsg(RPL_PART(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName()));
	}
}

void	Server::c_ping(std::vector<std::string> param, Users *user) {
	if (param.size() == 1)
		return (user->setBuffer(RPL_PING(getHost(), "")));
	user->setBuffer(RPL_PING(getHost(), param[0]));
}

void	Server::c_kick(std::vector<std::string> param, Users *user) {
	std::vector<std::string> split;
	if (!(param.size() >= 3))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "KICK"))); // (461)
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
	if (checkSplit(param[1], ','))
		user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
	split = splitString(param[1], ',');
	std::string reason;
	if (param.size() < 4) 
		reason = "Good boy points have dropped to 0!";
	else
		reason = fill_vec(&param, param.begin() + 2).substr();
	for (size_t i = 0; i < split.size(); i++) {
		Users *toKick = getUserByNn(split[i]);
		if (!toKick)
			return (user->setBuffer(ERR_NOSUCHNICK(getHost(), user->getNickName(), split[i]))); // (401) 
		if (!channel->isUser(toKick) && !channel->isOperator(toKick))
			return (user->setBuffer(ERR_USERNOTINCHANNEL(getHost(), user->getNickName(), toKick->getNickName(), channel->getName()))); // (441)
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
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "INVITE"))); // (461)
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); //  (442)
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
  	Users *toAdd = getUserByNn(param[0]);
	if (toAdd == NULL)
		return (user->setBuffer(ERR_NOSUCHNICK(this->getHost(), user->getNickName(), param[0])));
	if (channel->isUser(toAdd) || channel->isOperator(toAdd))
		return (user->setBuffer(ERR_USERONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (443)
	channel->addUser(toAdd);
	user->setBuffer(RPL_INVITING(getHost(), user->getNickName(), toAdd->getNickName(), channel->getName())); // (341)
	toAdd->setBuffer(RPL_INVITE(user->getNickName(), user->getUserName(), user->getHostName(), toAdd->getNickName(), channel->getName()));
	// prob broadcast on chan, also send him reply about chan topic
}

void	Server::c_topic(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "TOPIC"))); // (461)
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user) && (channel->getModes() & FLAG_T))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
	if (param.size() == 1) {
		if (channel->getTopic().empty())
			return (user->setBuffer(RPL_NOTOPIC(getHost(), user->getNickName(), channel->getName()))); // (331)
		else
			return (user->setBuffer(RPL_TOPIC(getHost(), user->getNickName(), channel->getName(), channel->getTopic()))); // (332)
	}
	std::string top;
	top = fill_vec(&param, param.begin() + 1).substr(0, TOPICLEN);
	channel->setTopic(top);
	std::time_t currTime = std::time(NULL);
	std::string time = std::ctime(&currTime);
	channel->broadcastMsg(RPL_TOPIC(getHost(), user->getNickName(), channel->getName(), channel->getTopic()));
	channel->broadcastMsg(RPL_TOPICWHOTIME(getHost(), channel->getName(), user->getNickName(), time));
}

void	Server::c_mode(std::vector<std::string> param, Users *user)
{
	uint8_t mode;
	int i;

	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "MODE"))); //(461)
	i = 0;
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0]))); // 403)
	if (param.size() < 2)
		return (user->setBuffer(RPL_CHANNELMODEIS(this->getHost(), user->getNickName(), channel->getName(), channel->convertMode()))); // (324)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
	mode = initMode(param, mode, channel);
	if (mode & (1 << 7))
		return (user->setBuffer(ERR_UMODEUNKNOWNFLAG(getHost(), user->getNickName()))); //(501)
	channel->setMode(mode);
	channel->broadcastMsg(RPL_CHANNELMODEIS(this->getHost(), user->getNickName(), channel->getName(), channel->convertMode())); // (324)
}

void	Server::c_pass(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PASS"))); // (461)
	if (user->getStatus() & PASS_FLAG)
		return (user->setBuffer(ERR_ALREADYREGISTRED(user->getNickName()))); // (462)
	if (param[0] != getPassword())
		return (user->setBuffer(ERR_PASSWDMISMATCH(user->getNickName()))); // (464)
	user->setStatus(PASS_FLAG);
}

void	Server::c_nick(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1))
		return (user->setBuffer(ERR_NONICKNAMEGIVEN(user->getNickName()))); // (431)
	if (!isNickname(param[0]) || param[0].length() > NICKLEN)
		return (user->setBuffer(ERR_ERRONEUSNICKNAME(getHost(), user->getNickName(), param[0]))); // (432)
	if (nickNameExists(param[0]))
		return (user->setBuffer(ERR_NICKNAMEINUSE(getHost(), user->getNickName(), param[0]))); // (433)
	if (!(user->getStatus() & NICK_FLAG)) {
		user->setStatus(NICK_FLAG);
		if (user->getStatus() & USER_FLAG)
			user->setBuffer(RPL_WELCOME(getHost(), user->getNickName(), user->getUserName(), user->getHostName()));
	}
	else
		sendAllChan(getChanList(user), RPL_NICKCHANGE(user->getNickName(), user->getUserName(), user->getHostName(), param[0]));
	user->setNickName(param[0]);
}

void	Server::c_user(std::vector<std::string> param, Users *user) // check handle realname
{
	std::string username;
	if (param.size() < 4)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "USER"))); // (461)
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
			user->setBuffer(RPL_WELCOME(getHost(), user->getNickName(), user->getUserName(), user->getHostName()));
	}
	else
		sendAllChan(getChanList(user), RPL_NICKCHANGE(user->getNickName(), user->getUserName(), user->getHostName(), param[0]));
	user->setUserName(username);
}

void	Server::c_join(std::vector<std::string> param, Users *user)
{
	int i_key;
	int i_chn;
	std::vector<std::string> keys;
	std::vector<std::string> channels;

	if (param.size() < 1 || param.size() > 2)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "JOIN"))); // (461)
	if (checkSplit(param[0], ','))
		user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
	channels = splitString(param[0], ',');
	if (param.size() == 2)
	{
		if (checkSplit(param[1], ','))
			user->setBuffer(RPL_INPUTWARNING(this->getHost(), user->getNickName())); 
		keys = splitString(param[1], ',');
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
				getAllChannels().push_back(channel);
				channel->addOperator(user);
			}
		}
		else if (channel->getModes() & FLAG_I)
			user->setBuffer(ERR_INVITEONLYCHAN(getHost(), user->getNickName(), channel->getName())); // (473)
		else if (!(channel->getModes() & FLAG_K) ||
			(!(keys.empty()) && !(keys[i_key].empty()) && keys[i_key] == channel->getPassword()))
		{
			if (!(channel->getModes() & FLAG_L) || (channel->getUserList().size() < channel->getUserLimit())) {
				channel->addUser(user);
				channel->broadcastMsg(RPL_JOIN(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName()));
				if (!channel->getTopic().empty())
					user->setBuffer(RPL_TOPIC(this->getHost(), user->getNickName(), channel->getName(), channel->getTopic()));
				std::string reply = RPL_NAMREPLY(this->getHost(), user->getNickName(), channel->getName()) + channel->getNickNameList();
				user->setBuffer(reply);
				user->setBuffer(RPL_ENDOFNAMES(this->getHost(), user->getNickName(), channel->getName()));
			}
			else
				user->setBuffer(ERR_CHANNELISFULL(getHost(), user->getNickName(), channel->getName())); // (471)
		}
		else
			user->setBuffer(ERR_BADCHANNELKEY(getHost(), user->getNickName(), channel->getName())); // (475)
		i_key += ((channel->getModes() & FLAG_K) == FLAG_K);
		i_chn++;
	}
}

void Server::c_privmsg(std::vector<std::string> param, Users *user) {
	if (param.size() < 2)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PRIVMSG")));
	
	Users *targ;
	Channel *targ_channel;
	std::vector<std::string> lst = splitString(param[0], ',');
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

void Server::c_restart(std::vector<std::string> param, Users *user) {
	// check priv first
	setState(START);
	getFds().clear();

	close(getServerSocket());
	for (std::vector<Users *>::iterator it = getAllUsers().begin(); it != getAllUsers().end(); ++it) //check better work with a copy (getAllUsers())
		delete (*it);
	for (std::vector<Channel *>::iterator it = getAllChannels().begin(); it != getAllChannels().end(); ++it) //check better work with a copy (getAllUsers())
		delete (*it);
	getAllChannels().clear();
	getAllUsers().clear();
}

void Server::c_quit(std::vector<std::string> param, Users *user) {
	user->setBuffer(RPL_QUIT(user->getNickName(), user->getUserName(), user->getHostName(), "QUIT: " + "quitting..."));
	send_2usr(user->getSocketDescriptor());
	removeUserFromServer(user);
}

uint8_t Server::initMode(std::vector<std::string> param, uint8_t mode, Channel *channel) // check we might need to create a utils file for Server
{
	int		i;
	int		len;
	int		itParam;
	uint8_t	setUnset;
	uint8_t	flag;

	itParam = 1;
	setUnset = 0;
	flag = 0;
	i = 0;
	len = param[1].length();
	while (i < param[1].length())
	{
		if (param[1][i] == '+')
			setUnset = FLAG_SET;
		else if (param[1][i] == '-')
			setUnset = FLAG_UNSET;
		else if (param[1][i] == 'i') // type D
			mode = setTheUnset(mode, FLAG_I, setUnset);
		else if (param[1][i] == 't') // type D
			mode = setTheUnset(mode, FLAG_T, setUnset);
		else if (param[1][i] == 'k')
		{
			if (setUnset & FLAG_SET)
			{
				if (param[++itParam].empty())
					; // error need more param or set password to default
				else
					channel->setPassword(param[++itParam]);
			}
			mode = setTheUnset(mode, FLAG_K, setUnset);
		}
		else if (param[1][i] == 'o')
		{
			if (param[++itParam].empty())
				; // error need more param or do nothing
			else
			{
				if (setUnset & FLAG_SET)
					channel->addOperator(getUserByNn(param[itParam]));
				else if (setUnset & FLAG_UNSET)
					; // check channel->deleteOperator(getUserByNn(param[itParam]));
			}
			// check mode = setTheUnset(mode, FLAG_O, setUnset); // we don t need to set/unset it anymore but still there
		}
		else if (param[1][i] == 'l')
		{
			if (setUnset & FLAG_SET)
			{
				if (param[++itParam].empty())
					; // error need more param or set userMax to default
				else
					; // check channel->setChanLimit(param[++itParam]); // error if param not <int> or set it to default
			}
			mode = setTheUnset(mode, FLAG_L, setUnset);
		}
		else
			return (FLAG_ERR);
		i++;
	}
	return (mode);
}
