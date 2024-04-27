#include "../include/Server.hpp"

void	Server::c_cap(std::vector<std::string> param, Users *user) {
	if (param.size() > 1) {
		if (param[0] == "END" && (user->getStatus() & CAPON_FLAG)) {
			user->setStatus(CAPOFF_FLAG);
			user->unsetStatus(CAPON_FLAG);
		}
		else if (param[0] == "LS") {
			user->setBuffer(RPL_CAP(this->host));
			user->setStatus(CAPON_FLAG);
		}
	}
}

void	Server::c_part(std::vector<std::string> param, Users *user) {

}

void	Server::c_kick(std::vector<std::string> param, Users *user) {
	std::vector<std::string> split;
	if (!(param.size() >= 3))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "KICK"))); // (461)
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (442) // check 441 before 442
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
	split = splitString(param[1], ',');
	std::string str;
	if (param.size() < 4)
		str = "defaul reason"; // check the default reason
	else
		str = fill_vec(param, param.begin() + 2).substr(0, KICKLEN); // check fill_vec returning wrong output
	for (size_t i = 0; i < split.size(); i++) {
		Users *toKick = getUserByUn(split[i]);
		if (!toKick)
			return (user->setBuffer(ERR_USERNOTINCHANNEL(this->host, user->getNickName(), split[i], channel->getName()))); // (441) // check maybe add a new
		if (!channel->isUser(toKick) && !channel->isOperator(toKick))
			return (user->setBuffer(ERR_USERNOTINCHANNEL(this->host, user->getNickName(), toKick->getNickName(), channel->getName()))); // (441) // check repetition
		if (channel->isUser(toKick))
			channel->deleteUser(toKick, user, this->getHost());
		else if (channel->isOperator(toKick))
			channel->deleteOperator(toKick, user, this->getHost());
		// toKick->setBuffer(RPL_KICK(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), toKick->getNickName())); // add reason later
		// channel->broadcastMsg(str);
	}
}

void	Server::c_invite(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 2)) // check should be if (param.size() != 2) but should check the error message in this case
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "INVITE"))); // (461)
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); //  (442)
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
  	Users *toAdd = getUserByUn(param[0]);
	if (channel->isUser(toAdd) || channel->isOperator(toAdd))
		return (user->setBuffer(ERR_USERONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (443)
	channel->addUser(toAdd);
	user->setBuffer(RPL_INVITING(this->host, user->getNickName(), toAdd->getNickName(), channel->getName())); // (341)
	toAdd->setBuffer(RPL_INVITE(user->getNickName(), user->getUserName(), user->getHostName(), toAdd->getNickName(), channel->getName()));
	toAdd->invite(channel);
}

void	Server::c_topic(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "TOPIC"))); // (461)
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(this->host, user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user) && (channel->getModes() & FLAG_T))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
	if (param.size() == 1) {
		if (channel->getTopic().empty())
			return (user->setBuffer(RPL_NOTOPIC(this->host, user->getNickName(), channel->getName()))); // (331)
		else
			return (user->setBuffer(RPL_TOPIC(this->host, user->getNickName(), channel->getName(), channel->getTopic()))); // (332)
	}
	std::string top;
	top = fill_vec(param, param.begin()).substr(0, TOPICLEN); // check should start from param[1]
	channel->setTopic(top);
	std::time_t currTime = std::time(NULL);
	std::string time = std::ctime(&currTime);
	channel->broadcastMsg(RPL_TOPIC(this->host, user->getNickName(), channel->getName(), channel->getTopic()));
	channel->broadcastMsg(RPL_TOPICWHOTIME(this->host, channel->getName(), user->getNickName(), time));
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
	if (param.size() < 2)
		return ; // RPL_CHANNELMODEIS (324)
	mode = initMode(param, mode, channel);
	if (mode & (1 << 7))
		return (user->setBuffer(ERR_UMODEUNKNOWNFLAG(this->host, user->getNickName()))); //(501)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(this->host, user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user)) // check extra priv
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(this->host, user->getNickName(), channel->getName()))); // (482)
	channel->setMode(mode);
	// success rpl value basically sends a message on the channel to every user saying what the new modes are
}

void	Server::c_pass(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1)) // check what if password have spaces?
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
	if (!(user->getStatus() & NICK_FLAG)) {
		user->setStatus(NICK_FLAG);
		if (user->getStatus() & USER_FLAG)
			user->setBuffer(RPL_WELCOME(this->host, user->getNickName(), user->getUserName(), user->getHostName()));
	}
	else
		this->sendAllChan(this->getChanList(user), RPL_NICKCHANGE(user->getNickName(), user->getUserName(), user->getHostName(), param[0]));
	user->setNickName(param[0]);
}

void	Server::c_user(std::vector<std::string> param, Users *user)
{
	// this username that is being set needs to be prefixed with ~, cuz not using ident protocol
	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "USER"))); // (461)
	if (user->getStatus() & USER_FLAG)
		return (user->setBuffer(ERR_ALREADYREGISTRED(user->getNickName()))); // (462)
	user->setUserName(param[0]);
	if (param.size() == 4 && param[1] == "0" && param[2] == "*")
		user->setReal(param[3]);
	user->setStatus(USER_FLAG);
	if (user->getStatus() & NICK_FLAG)
		user->setBuffer(RPL_WELCOME(this->host, user->getNickName(), user->getUserName(), user->getHostName()));
}

void	Server::c_join(std::vector<std::string> param, Users *user)
{
	int i;
	std::vector<std::string> keys;
	std::vector<std::string> channels;

	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "JOIN"))); // (461)
	channels = splitString(param[0], ',');
	if (channels.size() > CHANLIMIT)
		return (user->setBuffer(ERR_TOOMANYCHANNELS(this->host, user->getNickName(), channels[0]))); // (405)
	if (!(param.size() >= 2))
		keys = splitString(param[1], ',');
	i = 0;
	while (i < channels.size()) // check add -1 
	{
		Channel *channel = getChannel(channels[i]);
		if (!channel) {
			channel = new Channel(channels[i]);
			this->all_channels.push_back(channel);
		}
		else if (channel->getModes() & FLAG_I)
			user->setBuffer(ERR_INVITEONLYCHAN(this->host, user->getNickName(), channel->getName())); // (473)
		else if (!(channel->getModes() & FLAG_K) ||
			(!(keys.empty()) && !(keys[i].empty()) && keys[i] == channel->getPassword()))
		{
			if (channel->getUserList().size() > USERLIMIT)
				user->setBuffer(ERR_CHANNELISFULL(this->host, user->getNickName(), channel->getName())); // (471)
			else
				channel->addUser(user);
		}
		else
			user->setBuffer(ERR_BADCHANNELKEY(this->host, user->getNickName(), channel->getName())); // (475)
		i++;
	}
	// check add RPL msg, these are for each channel joined, will implement when changing how buffer works.
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
	for (std::vector<Users *>::iterator it = this->all_users.begin(); it != this->all_users.end(); ++it)
		delete (*it);
	for (std::vector<Channel *>::iterator it = this->all_channels.begin(); it != this->all_channels.end(); ++it)
		delete (*it);
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
					channel->addOperator(getUserByUn(param[itParam]));
				else if (setUnset & FLAG_UNSET)
					; // check channel->deleteOperator(getUserByUn(param[itParam]));
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
