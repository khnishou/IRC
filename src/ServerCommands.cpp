#include "../include/Server.hpp"

void	Server::c_kick(std::vector<std::string> param, Users *user) {
  std::vector<std::string> split;
	if (!(param.size() >= 3))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "KICK"))); // (461)
  Channel *channel = getChannel(param[0]);
  if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); // (403)
  if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
	if (!channel->isUser(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (442) // check 441 before 442
  split = splitString(param[1], ',');
  for (size_t i = 0; i < split.size(); i++) { // check -1
		Users *toKick = getUserByUn(split[i]);
		if (!toKick)
			return (user->setBuffer(ERR_USERNOTINCHANNEL(this->host, user->getNickName(), toKick->getNickName(), channel->getName()))); // (441) // check repetition
		if (!channel->isUser(toKick) && !channel->isOperator(toKick))
			return (user->setBuffer(ERR_USERNOTINCHANNEL(this->host, user->getNickName(), toKick->getNickName(), channel->getName()))); // (441) // check repetition
		if (channel->isUser(toKick))
			channel->deleteUser(toKick, user, this->getHost());
		else if (channel->isOperator(toKick))
			channel->deleteOperator(toKick, user, this->getHost());
	}
}

void	Server::c_invite(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 2))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "INVITE"))); // (461)
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user))
    	return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); //  (442)
	if (!channel->isOperator(user))
    	return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
  Users *toAdd = getUserByUn(param[0]);
	if (!channel->isUser(toAdd))
		return (user->setBuffer(ERR_USERONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (443)
  	channel->addUser(toAdd);
	// no error occured, setting the correct replies on the executing user and receiving user
	user->setBuffer(RPL_INVITING(this->host, user->getNickName(), toAdd->getNickName(), channel->getName()));
	toAdd->setBuffer(RPL_INVITE(user->getNickName(), user->getUserName(), user->getHostName(), toAdd->getNickName(), channel->getName()));
	toAdd->invite(channel);
}

void	Server::c_topic(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "TOPIC"))); // (461)
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); //  (403)
	if (!channel->isUser(user))
	if (param.size() == 1) {
		if (!channel->getTopic().empty())
      	return (user->setBuffer(RPL_NOTOPIC(this->host, user->getNickName(), channel->getName()))); // (331)
   	else
   		return (user->setBuffer(RPL_TOPIC(this->host, user->getNickName(), channel->getName(), channel->getTopic()))); // (332)
	}
		return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (442)
	// this check is not enough, need to check for permissions differently, doesnt need to be operator in channel to change topic
	if (!channel->isOperator(user)) // add cond here for priv check
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); //  (482)
	// add param vector together on string and then set
	std::string top;
	top = fill_vec(param);
	channel->setTopic(top);
	std::string time; // use this to set time
	// rpl is when topic being set/ changed notify everyone in channel using RPL_TOPIC followed by RPL_TOPICWHOTIME
}

void	Server::c_mode(std::vector<std::string> param, Users *user)
{
	uint8_t mode;
	int i;

	// need 2 revisit this logic, if its unsetting a mode than no params must be provided, if trying to set then need params
	if (!(param.size() >= 1))
    	return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "MODE"))); //(461) 
	i = 0;
	Channel *channel = getChannel(param[0]);
	if (!channel)
    	return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); // 403)
	mode = initMode(param, mode, channel);
	if (mode & (1 << 7))
		return (user->setBuffer(ERR_UMODEUNKNOWNFLAG(this->host, user->getNickName()))); //(501)
	if (!channel->isUser(user))
    	return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user))
    	return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
	channel->setMode(mode);
	// success rpl value basically sends a message on the channel to every user saying what the new modes are
}

void	Server::c_pass(std::vector<std::string> param, Users *user)
{
	if (!(param.size() >= 1))
    	return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PASS"))); // (461)
	if (user->getStatus() & PASS_FLAG)
		return (user->setBuffer(ERR_ALREADYREGISTRED(user->getNickName()))); // (462)
	if (param[0] != this->getPassword())
		return (user->setBuffer(ERR_PASSWDMISMATCH(user->getNickName()))); // (464)
	user->setStatus(PASS_FLAG);
}

void	Server::c_nick(std::vector<std::string> param, Users *user)
{
	if (!(param.size() >= 1))
    	return (user->setBuffer(ERR_NONICKNAMEGIVEN(user->getNickName()))); // (431)
	if (!isNickname(param[0]))
		return (user->setBuffer(ERR_ERRONEUSNICKNAME(this->host, user->getNickName(), param[0]))); // (432)
	if (nickNameExists(param[0]))
		return (user->setBuffer(ERR_NICKNAMEINUSE(this->host, user->getNickName(), param[0]))); // (433)
	user->setNickName(param[0]);
	user->setStatus(NICK_FLAG);
	// RPL is, if first time no reply, however if changing nick then need 2 inform everyone that shares channels with this user
}

void	Server::c_user(std::vector<std::string> param, Users *user)
{
	if (!(param.size() >= 1))
    	return ; // error ERR_NEEDMOREPARAMS (461)
	if (user->getStatus() & USER_FLAG)
		return ; // error ERR_ALREADYREGISTERED (462)
	user->setUserName(param[0]);
	if (param.size() >= 4)
		;// check  set realName
	user->setStatus(USER_FLAG);
	// RPL logic is same as nick
}

void	Server::c_join(std::vector<std::string> param, Users *user)
{
	int i;
	std::vector<std::string> keys;
	std::vector<std::string> channels;

	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "INVITE"))); // (461)
	channels = splitString(param[0], ',');
	if (channels.size() > CHANLIMIT)
		; // error ERR_TOOMANYCHANNELS (405)
	if (!(param.size() >= 2))
		keys = splitString(param[1], ',');
	i = 0;
	while (i < channels.size()) // check add -1 
	{
		Channel *channel = getChannel(channels[i]);
		if (!channel)
			this->all_channels.push_back(channel);
		else if (channel->getModes() & FLAG_I)
			; // error ERR_INVITEONLYCHAN (473)
		else if (!(channel->getModes() & FLAG_K) ||
			(!(keys.empty()) && !(keys[i].empty()) && keys[i] == channel->getPassword()))
		{
			if (channel->getUserList().size() > USERLIMIT)
				; // error ERR_CHANNELISFULL (471)
			else
				channel->addUser(user);
		}
		else
			; // error ERR_BADCHANNELKEY (475)
		i++;
	}
	// check add RPL msg
	// RPL_TOPIC (332)
	// RPL_TOPICWHOTIME (333)
	// RPL_NAMREPLY (353)
	// RPL_ENDOFNAMES (366)
}

void Server::c_privmsg(std::vector<std::string> param, Users *user) {
	
}

void Server::c_restart(std::vector<std::string> param, Users *user) {
	// check priv first
	this->state = START;
	this->fds.clear();
	close(this->serverSocket);
	// need 2 delete users and channels for leaks first
	this->all_channels.clear();
	this->all_users.clear();
}

void Server::c_quit(std::vector<std::string> param, Users *user) {
	removeUserFromServer(user);
}

uint8_t Server::initMode(std::vector<std::string> param, uint8_t mode, Channel *channel) // check we might need to create a utils file for Server
{
	int		i;
	int		len;
	int		itParam;
	uint8_t	setUnset;
	uint8_t	flag;

	itParam = 0;
	setUnset = 0;
	flag = 0;
	i = 0;
	len = param[0].length();
	while (i < param[0].length())
	{
		if (param[0][i] == '+')
			setUnset = FLAG_SET;
		else if (param[0][i] == '-')
			setUnset = FLAG_UNSET;
		else if (param[0][i] == 'i')
			mode = setTheUnset(mode, FLAG_I, setUnset);
		else if (param[0][i] == 't')
			mode = setTheUnset(mode, FLAG_T, setUnset);
		else if (param[0][i] == 'k')
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
		else if (param[0][i] == 'o')
		{
			if (param[++itParam].empty())
				; // error need more param or do nothing
			else
			{
				if (setUnset & FLAG_SET)
					channel->addOperator(getUserByUn(param[itParam]));
				else if (setUnset & FLAG_UNSET)
					;// check channel->deleteOperator(getUserByUn(param[itParam]));
			}
			// check mode = setTheUnset(mode, FLAG_O, setUnset); // we don t need to set/unset it anymore but still there
		}
		else if (param[0][i] == 'l')
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
