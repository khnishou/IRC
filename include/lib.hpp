
#pragma once

#include <string>
#include <vector>
#include <sstream>

//ERROR REPLIES
#define ERR_NOSUCHNICK(src, nick, chan)									":" + src + " 401 " + nick + " " + chan + " :No such nick/channel\r\n"
#define ERR_NOSUCHCHANNEL(src, nick, chan)								":" + src + " 403 " +  nick + " " + chan + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(src, nick, chan)							":" + src +  " 404 " + nick + " " + chan + " :Cannot send to channel\r\n"
#define	ERR_UNKNOWNCOMMAND(src, nick, cmd)								":" + src + " 421 " + nick + " " + cmd + " :Unknown command\r\n"
#define ERR_NONICKNAMEGIVEN(source)										":" + source + " 431 " + "NICK" + " :Nickname not given\r\n"
#define ERR_ERRONEUSNICKNAME(src, nick, err_nick)						":" + src + " 432 " + nick + " " + err_nick + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(src, nick, used)								":" + src + " 433 " + nick + " " + used + " :Nickname is already in use\r\n"
#define ERR_USERNOTINCHANNEL(src, nick, targ, chan)						":" + src +  " 441 " + nick + " " + targ + " " + chan + " :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL(src, nick, chan)								":" + src +  " 442 " + nick + " " + chan + " :You're not on that channel\r\n"
#define ERR_NOTREGISTERED(source)										":" + source + " 451 " + ":You have not registered\r\n"
#define ERR_ALREADYLOGEDIN(source)										":" + source + " 460 " ":Already logged in\r\n"
#define ERR_NEEDMOREPARAMS(source, cmd)									":" + source + " 461 " + cmd + " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED(source)									":" + source + " 462 " ":You may not reregister\r\n"
#define ERR_PASSWDMISMATCH(source)										":" + source + " 464 " + "PASS" + " :Password incorrect\r\n"
#define	ERR_UMODEUNKNOWNFLAGCH(src, nick)								":" + src +  " 472 " + nick + " :Unknown mode char to me\r\n"
#define ERR_INVITEONLYCHAN(src, nick, chan)								":" + src + " 473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n"
#define	ERR_NOPRIVILEGES(src, cmd)										":" + src + " 481 " + cmd + " :Permission Denied- You're not an IRC operator\r\n"
#define ERR_CHANOPRIVSNEEDED(src, nick, chan)							":" + src +  " 482 " + nick + " " + chan + " :You're not channel operator\r\n"
#define	ERR_UMODEUNKNOWNFLAUSR(src, nick)								":" + src +  " 501 " + nick + " :Unknown MODE flag\r\n"
#define ERR_USERSDONTMATCH(src, nick)									":" + src +  " 502 " + nick + " :Cant change mode for other users\r\n"

// NUMERIC REPLIES			
#define RPL_WELCOME(src, nick, user, host)								":" + src + " 001 " + nick + " :Welcome to the IRC_SERV Network " + nick + "!" + user + "@" + host + "\r\n"
#define RPL_MODEUSER(src, nick, mode)									":" + src + " 221 " + nick + " " + mode + "\r\n"
#define RPL_MODECHANNEL(src, nick,chan, mode)							":" + src + " 324 " + nick + " " + chan + " " + mode + "\r\n"
#define RPL_NOTOPIC(src, nick, chan)									":" + src + " 331 " + nick + " " + chan + " :No topic is set\r\n"
#define	RPL_TOPIC(src, nick, chan, topic)								":" + src + " 332 " + nick + " " + chan + " :" + topic + "\r\n"
#define	RPL_INVITING(src, nick, targ, chan)								":" + src + " 341 " + nick + " " + targ + " " + chan + "\r\n"
#define	RPL_NAMREPLY(src, nick, chan)									":" + src + " 353 " + nick + " = " + chan + " :"
#define RPL_ENDOFNAMES(src, nick, chan)									":" + src + " 366 " + nick + " " + chan + " :END of NAMES list\r\n"
#define	RPL_YOUREOPER(src)												":" + src + " 381 " + "PASS :You are now an IRC operator\r\n"
#define	RPL_QUOT(src, chan, quot)										":QUOTBOT!BOT@" + src + " NOTICE " + chan + " :" + quot + "\r\n"
//MAKE IT A NOTICE

// COMMAND REPLIES	
#define	RPL_CAP(src)													":" + src + " CAP * LS :cap reply...\r\n"
#define RPL_PING(src, token)											":" + src + " PONG " + src + " :" + token + "\r\n"
#define	RPL_NICKCHANGE(nick, user, user_host, nw_nick)					":" + nick + "!" + user + "@" + user_host + " " + "NICK" + " :" + nw_nick + "\r\n"
#define RPL_JOIN(nick, user, user_host, chan)							":" + nick + "!" + user + "@" + user_host + " JOIN " + chan + " * :" + user + "\r\n"
#define	RPL_INVITE(src_nick, src_usr, src_host, targ, chan) 			":" + src_nick + "!" + src_usr + "@" + src_host + " INVITE " + targ + " :" + chan + "\r\n"
#define RPL_PART(src_nick, src_usr, src_host, chan)						":" + src_nick + "!" + src_usr + "@" + src_host + " PART " + chan + " :gonee... :'( \r\n"
#define RPL_PRIVMSG(src_nick, src_usr, src_host, dis_nick, msg)			":" + src_nick + "!" + src_usr + "@" + src_host + " PRIVMSG " + dis_nick + " :" + msg + "\r\n"
#define RPL_NOTICE(src_nick, src_usr, src_host, dis_nick, msg)			":" + src_nick + "!" + src_usr + "@" + src_host + " NOTICE " + dis_nick + " :" + msg + "\r\n"
#define RPL_QUIT(src_nick, src_usr, src_host, reason)					":" + src_nick + "!" + src_usr + "@" + src_host + " " + reason + "\r\n"
#define RPL_TOPICCHANGE(src_nick, src_usr, src_host, chan, topic)		":" + src_nick + "!" + src_usr + "@" + src_host + " TOPIC " + chan + " :" + topic + "\r\n"
#define RPL_MODECHAN(src_nick, src_usr, src_host, chan, mode, nick) 	":" + src_nick + "!" + src_usr + "@" + src_host + " MODE " + chan + " " + mode + " " + nick + "\r\n"
#define RPL_KICK(src_nick, src_usr, src_host, chan, targ)				":" + src_nick + "!" + src_usr + "@" + src_host + " KICK " + chan + " " + targ + " :" + targ + "\r\n"

#define PASS_FLAG (1 << 0)
#define NICK_FLAG (1 << 1)
#define USER_FLAG (1 << 2)

enum SERVER_STATE {
	ON,
	OFF,
	START
};

struct Message {
    std::vector<std::string> tags;
    std::string source;
    std::string command;
    std::vector<std::string> parameters;
};

int							skip_space(std::string str, int i);
int							skip_arg(std::string str, int i);
std::vector<std::string>	splitString(const std::string& str, char delimiter);
uint8_t						initMode(std::string str, uint8_t mode);

