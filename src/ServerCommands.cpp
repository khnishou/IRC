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
		; // error ",arg1,,arg2,"
	std::vector<std::string> split = splitString(param[0], ',');
	for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); ++it)
	{
		Channel *channel = getChannel(*it);
		if (!channel)
			return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), *it))); // (403) // check the buffer should be joined with the old one (we might need to check all the errors inside loops)
		if (!channel->isUser(user) && !channel->isOperator(user))
			return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442) // check the buffer should be joined with the old one
		if (param.size() > 1)
			std::string reason = fill_vec(&param, param.begin() + 1);
		// check leaving a channel
		// check RPL leaving a channel
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
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442) // check 441 before 442
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
	if (checkSplit(param[1], ','))
		; // error ",arg1,,arg2,"
	split = splitString(param[1], ',');
	std::string reason;
	if (param.size() < 4) 
		reason = "Good boy points have dropped to 0!"; // check the default reason
	else
		reason = fill_vec(&param, param.begin() + 2).substr();
	for (size_t i = 0; i < split.size(); i++) {
		Users *toKick = getUserByUn(split[i]);
		if (!toKick)
			return (user->setBuffer(ERR_USERNOTINCHANNEL(getHost(), user->getNickName(), split[i], channel->getName()))); // (441) // check maybe add a new
		if (!channel->isUser(toKick) && !channel->isOperator(toKick))
			return (user->setBuffer(ERR_USERNOTINCHANNEL(getHost(), user->getNickName(), toKick->getNickName(), channel->getName()))); // (441) // check repetition
		if (channel->isUser(toKick))
			channel->deleteUser(toKick, user, getHost());
		else if (channel->isOperator(toKick))
			channel->deleteOperator(toKick, user, getHost());
		toKick->setBuffer(RPL_KICK(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), toKick->getNickName(), reason)); // add reason later
		channel->broadcastMsg(RPL_KICK(user->getNickName(), user->getUserName(), user->getHostName(), channel->getName(), toKick->getNickName(), reason));
	}
}

void	Server::c_invite(std::vector<std::string> param, Users *user) {
	if (!(param.size() >= 2)) // check should be if (param.size() != 2) but should check the error message in this case
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "INVITE"))); // (461)
	Channel *channel = getChannel(param[1]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0]))); // (403)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); //  (442)
	if (!channel->isOperator(user))
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
  	Users *toAdd = getUserByUn(param[0]);
	if (channel->isUser(toAdd) || channel->isOperator(toAdd))
		return (user->setBuffer(ERR_USERONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (443)
	channel->addUser(toAdd);
	user->setBuffer(RPL_INVITING(getHost(), user->getNickName(), toAdd->getNickName(), channel->getName())); // (341)
	toAdd->setBuffer(RPL_INVITE(user->getNickName(), user->getUserName(), user->getHostName(), toAdd->getNickName(), channel->getName()));
	toAdd->invite(channel);
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

	// need 2 revisit this logic, if its unsetting a mode than no params must be provided, if trying to set then need params
	if (!(param.size() >= 1))
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "MODE"))); //(461)
	i = 0;
	Channel *channel = getChannel(param[0]);
	if (!channel)
		return (user->setBuffer(ERR_NOSUCHCHANNEL(getHost(), user->getNickName(), param[0]))); // 403)
	if (param.size() < 2)
		return ; // RPL_CHANNELMODEIS (324)
	mode = initMode(param, mode, channel);
	if (mode & (1 << 7))
		return (user->setBuffer(ERR_UMODEUNKNOWNFLAG(getHost(), user->getNickName()))); //(501)
	if (!channel->isUser(user) && !channel->isOperator(user))
		return (user->setBuffer(ERR_NOTONCHANNEL(getHost(), user->getNickName(), channel->getName()))); // (442)
	if (!channel->isOperator(user)) // check extra priv
		return (user->setBuffer(ERR_CHANOPRIVSNEEDED(getHost(), user->getNickName(), channel->getName()))); // (482)
	channel->setMode(mode);
	// success rpl value basically sends a message on the channel to every user saying what the new modes are
}

void	Server::c_pass(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1)) // check what if password have spaces?
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PASS"))); // (461)
	if (user->getStatus() & PASS_FLAG)
		return (user->setBuffer(ERR_ALREADYREGISTRED(user->getNickName()))); // (462)
	if (param[0] != getPassword())
		return (user->setBuffer(ERR_PASSWDMISMATCH(user->getNickName()))); // (464)
	user->setStatus(PASS_FLAG);
}

void	Server::c_nick(std::vector<std::string> param, Users *user)
{
	if (!(param.size() == 1)) // check what if nickname have spaces
		return (user->setBuffer(ERR_NONICKNAMEGIVEN(user->getNickName()))); // (431)
	if (!isNickname(param[0]) || param[0].length() > NICKLEN) // check if nickname is longer than NICKLEN throw an error
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
	if (param.size() == 0)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "USER"))); // (461)
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
		; // error ",arg1,,arg2,"
	channels = splitString(param[0], ',');
	if (param.size() == 2)
	{
		if (checkSplit(param[1], ','))
			; // error ",arg1,,arg2,"
		keys = splitString(param[1], ',');
	}
	i_key = 0;
	i_chn = 0;
	while (i_chn < channels.size())
	{
		Channel *channel = getChannel(channels[i_chn]);
		if (!channel) {
			if (getAllChannels().size() > CHANLIMIT)
				; // check in case the number of channels in the server > then the limit of channels in server
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
			if (!(channel->getModes() & FLAG_L) || (channel->getUserList().size() < channel->getUserLimit()))
				channel->addUser(user);
			else
				user->setBuffer(ERR_CHANNELISFULL(getHost(), user->getNickName(), channel->getName())); // (471)
		}
		else
			user->setBuffer(ERR_BADCHANNELKEY(getHost(), user->getNickName(), channel->getName())); // (475)
		i_key += ((channel->getModes() & FLAG_K) == FLAG_K);
		i_chn++;
	}
// If a client’s JOIN command to the server is successful, the server MUST send, in this order:

// 1\ A JOIN message with the client as the message <source> and the channel they
//    have joined as the first parameter of the message.
// 2\ The channel’s topic (with RPL_TOPIC (332) and optionally RPL_TOPICWHOTIME (333)), 
//    and no message if the channel does not have a topic.
// 3\ A list of users currently joined to the channel (with one or more RPL_NAMREPLY (353)
//	  numerics followed by a single RPL_ENDOFNAMES (366) numeric). These RPL_NAMREPLY messages
//    sent by the server MUST include the requesting client that has just joined the channel.
	
	// check add RPL msg, these are for each channel joined, will implement when changing how buffer works.
	// RPL_TOPIC (332)
	// RPL_TOPICWHOTIME (333)
	// RPL_NAMREPLY (353)
	// RPL_ENDOFNAMES (366)
}

void Server::c_privmsg(std::vector<std::string> param, Users *user) {
	if (param.size() < 2)
		return (user->setBuffer(ERR_NEEDMOREPARAMS(user->getNickName(), "PRIVMSG")));
	
	// Users *targ;
	// Channel *targ_channel;

	// if (param[0][0] == '#') {
	// 	targ_channel = getChannel(param[0]);
	// 	if (!targ_channel)
	// 		return (user->setBuffer());
	// }
	// else {
	// 	targ = getUserByUn(param[0]);
	// 	if (!targ)
	// 		return (user->setBuffer());
	// }
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
