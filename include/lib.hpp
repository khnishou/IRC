
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib> 
#include <cerrno> 
#include <cstring>

#define RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define ORANGE "\033[38;5;202m\033[22m"
#define GRAY "\033[1;30m"
#define DEFAULT "\033[1;39m"

//ERROR REPLIES
#define ERR_UNKNOWNERROR(src, nick, cmd, msg)                           ":" + src + " 400 " + nick + " " + cmd + " :" + msg + "\r\n"
#define ERR_NOSUCHNICK(src, nick, chan)									":" + src + " 401 " + nick + " " + chan + " :No such nick/channel\r\n"
#define ERR_NOSUCHCHANNEL(src, nick, chan)								":" + src + " 403 " +  nick + " " + chan + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(src, nick, chan)							":" + src + " 404 " + nick + " " + chan + " :Cannot send to channel\r\n"
#define ERR_TOOMANYCHANNELS(src, nick, chan)                            ":" + src + " 405 " + nick + " " + chan + " : You have joined too many channels\r\n"
#define	ERR_UNKNOWNCOMMAND(src, nick, cmd)								":" + src + " 421 " + nick + " " + cmd + " :Unknown command\r\n"
#define ERR_NONICKNAMEGIVEN(source)										":" + source + " 431 " + "NICK" + " :Nickname not given\r\n"
#define ERR_ERRONEUSNICKNAME(src, nick, err_nick)						":" + src + " 432 " + nick + " " + err_nick + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(src, nick, used)								":" + src + " 433 " + nick + " " + used + " :Nickname is already in use\r\n"
#define ERR_USERNOTINCHANNEL(src, nick, targ, chan)						":" + src +  " 441 " + nick + " " + targ + " " + chan + " :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL(src, nick, chan)								":" + src +  " 442 " + nick + " " + chan + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(src, nick, chan)                              ":" + src + " 443 " + nick + " " + chan + " :is already on channel\r\n"
#define ERR_NOTREGISTERED(source)										":" + source + " 451 " + ":You have not registered\r\n"
#define ERR_ALREADYLOGEDIN(source)										":" + source + " 460 " ":Already logged in\r\n"
#define ERR_NEEDMOREPARAMS(source, cmd)									":" + source + " 461 " + cmd + " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED(source)									":" + source + " 462 " ":You may not reregister\r\n"
#define ERR_PASSWDMISMATCH(source)										":" + source + " 464 " + "PASS" + " :Password incorrect\r\n"
#define ERR_CHANNELISFULL(src, nick, chan)                              ":" + src + " 471 " + nick + " " + chan + " :Cannot join channel (+l)"
#define	ERR_UMODEUNKNOWNFLAGCH(src, nick)								":" + src +  " 472 " + nick + " :Unknown mode char to me\r\n"
#define ERR_INVITEONLYCHAN(src, nick, chan)								":" + src + " 473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY(src, nick, chan)                              ":" + src + " 475 " + nick + " " + chan + " :Cannot join chanel (+k)"
#define	ERR_NOPRIVILEGES(src, cmd)										":" + src + " 481 " + cmd + " :Permission Denied- You're not an IRC operator\r\n"
#define ERR_CHANOPRIVSNEEDED(src, nick, chan)							":" + src +  " 482 " + nick + " " + chan + " :You're not channel operator\r\n"
#define	ERR_UMODEUNKNOWNFLAG(src, nick)								    ":" + src +  " 501 " + nick + " :Unknown MODE flag\r\n"
#define ERR_USERSDONTMATCH(src, nick)									":" + src +  " 502 " + nick + " :Cant change mode for other users\r\n"
#define ERR_INVALIDINPUT(src, nick, cmd, reason)                        ":" + src + " " + nick + " " + cmd + " :" + reason + "\r\n"
// NUMERIC REPLIES			
#define RPL_WELCOME(src, nick, user, host)								":" + src + " 001 " + nick + " :Welcome to the IRC_SERV Network " + nick + "! " + user + "@" + host + "\r\n"
#define RPL_MODEUSER(src, nick, mode)									":" + src + " 221 " + nick + " " + mode + "\r\n"
#define RPL_CHANNELMODEIS(src, nick, chan, mode)						":" + src + " 324 " + nick + " " + chan + " " + mode + "\r\n" // check might create a function that return the right RPL
#define RPL_NOTOPIC(src, nick, chan)									":" + src + " 331 " + nick + " " + chan + " :No topic is set\r\n"
#define	RPL_TOPIC(src, nick, chan, topic)								":" + src + " 332 " + nick + " " + chan + " :" + topic + "\r\n"
#define RPL_TOPICWHOTIME(src, chan, nick, time)                         ":" + src + " 333 " + chan + " " + nick + " " + time + "\r\n"
#define	RPL_INVITING(src, nick, targ, chan)								":" + src + " 341 " + nick + " " + targ + " " + chan + "\r\n"
#define	RPL_NAMREPLY(src, nick, chan)									":" + src + " 353 " + nick + " = " + chan + " :"
#define RPL_ENDOFNAMES(src, nick, chan)									":" + src + " 366 " + nick + " " + chan + " :END of NAMES list\r\n"
#define	RPL_YOUREOPER(src, chan)										":" + src + "You are now an IRC operator in : " + chan + "\r\n"
#define RPL_NOLONGEROP(src, chan)                                       ":" + src + "You are no longer an IRC operator in : " + chan + "\r\n"
#define	RPL_QUOT(src, chan, quot)										":QUOTBOT!BOT@" + src + " NOTICE " + chan + " :" + quot + "\r\n"
#define RPL_INPUTWARNING(src, nick)                                     ":" + src + " " + nick + " :Empty parameters will be ingored...\r\n"
#define RPL_RECEIVEDTREQ(src, targ_nick, targ_host)                     ":" + src + " " + targ_nick + "@" + targ_host +":Received your file transfer request\r\n"
#define RPL_TRANSFERREQ(src, src_nick, src_host, src_port, fname)       ":" + src + " " + src_nick + "@" + src_host + " with port: " + src_port + " wants to send: " + fname + "\r\n"
#define RPL_BOT(bot_name, bot_cmd, res)                                 ":" + bot_name + " " + bot_cmd + " :" + res + "\r\n"
#define RPL_PASS(host)                                                  ":" + host + " : Password is correct! Please proceed to choose a nickname and username!\r\n"
#define RPL_NICK(host, new_nick)                                        ":" + host + " : Your nickname now is: " + new_nick + "\r\n"
#define RPL_USER(host, new_user)                                        ":" + host + " : Your username now is: " + new_user + "\r\n"
//MAKE IT A NOTICE

// COMMAND REPLIES	
#define	RPL_CAP(src)													":" + src + " CAP * LS :cap reply...\r\n"
#define RPL_PING(src, token)											":" + src + " PONG " + src + " :" + token + "\r\n"
#define	RPL_NICKCHANGE(nick, user, user_host, nw_nick)					":" + nick + "!" + user + "@" + user_host + " " + "NICK" + " :" + nw_nick + "\r\n"
#define RPL_JOIN(nick, user, user_host, chan)							":" + nick + "!" + user + "@" + user_host + " JOIN " + chan + " * :" + user + "\r\n"
#define	RPL_INVITE(src_nick, src_usr, src_host, targ, chan) 			":" + src_nick + "!" + src_usr + "@" + src_host + " INVITE " + targ + " :" + chan + "\r\n"
#define RPL_PART(src_nick, src_usr, src_host, chan, reason)						":" + src_nick + "!" + src_usr + "@" + src_host + " PART " + chan + " :" + reason + "\r\n"
#define RPL_PRIVMSG(src_nick, src_usr, src_host, dis_nick, msg)			":" + src_nick + "!" + src_usr + "@" + src_host + " PRIVMSG " + dis_nick + " :" + msg + "\r\n"
#define RPL_NOTICE(src_nick, src_usr, src_host, dis_nick, msg)			":" + src_nick + "!" + src_usr + "@" + src_host + " NOTICE " + dis_nick + " :" + msg + "\r\n"
#define RPL_QUIT(src_nick, src_usr, src_host, reason)					":" + src_nick + "!" + src_usr + "@" + src_host + " " + reason + "\r\n"
#define RPL_MODECHAN(src_nick, src_usr, src_host, chan, mode, nick) 	":" + src_nick + "!" + src_usr + "@" + src_host + " MODE " + chan + " " + mode + " " + nick + "\r\n"
#define RPL_KICK(src_nick, src_usr, src_host, chan, targ, reason)		":" + src_nick + "!" + src_usr + "@" + src_host + " KICK " + chan + " " + targ + " :" + reason + "\r\n"

// USER STATES
#define PASS_FLAG   (1 << 0)
#define NICK_FLAG   (1 << 1)
#define USER_FLAG   (1 << 2)
#define CAPON_FLAG  (1 << 3)
#define CAPOFF_FLAG (1 << 4)

// MODE STATES
#define FLAG_I		(1 << 0)
#define FLAG_T		(1 << 1)
#define FLAG_K		(1 << 2)
#define FLAG_L		(1 << 3)
#define ERR_SILENT	(1 << 5)
#define ERR_PARAM	(1 << 6)
#define ERR_SYNTAX	(1 << 7)

// FlAG UTILS
#define FLAG_SET	(1 << 0)
#define FLAG_UNSET	(1 << 1)

enum SERVER_STATE {
	ON,
	OFF,
	START
};

struct Message {
    std::vector<std::string>    tags;
    std::string                 source;
    std::string                 command;
    std::vector<std::string>    parameters;
};

int							skip_space(std::string str, int i);
int							skip_arg(std::string str, int i);
bool                        checkCSplit(const std::string& str, char del);
std::vector<std::string>    cSplitStr(const std::string& str, char del);
bool                        checkStrSplit(const std::string& str, std::string del);
std::vector<std::string>    strSplitStr(const std::string& str, std::string del);
int                     setTheUnset(int mode, int flag, int setUnset);
bool                        isNickname(const std::string& nickname);
std::string                 fill_vec(std::vector<std::string> *param, std::vector<std::string>::iterator ite);
Message                     parsing(std::string str);
bool                        isUint(const std::string& str);
