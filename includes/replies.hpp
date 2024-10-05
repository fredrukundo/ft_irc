
#ifndef REPLIES_H
# define REPLIES_H

#define REPLY_WELCOME(nick) ": 001 " + nick + " :Welcome " + nick + " to ft_irc network !\r\n"  
#define REPLY_YOURHOST(nick, hostname) ": 002 " + nick + " :Your host is " + hostname + " running version 1.0 !\r\n"
#define REPLY_CREATED(nick, formatted_time) ": 003 " + nick + " :This server was created " + formatted_time + " !\r\n"
// #define REPLY_MYINFO(nick, hostname) ":" + hostname + " 004 " + nick + " :Host: " + hostname + ", Version: 1.0, User mode: none, Channel modes: o, t, k, i !\r\n"


#define ERROR_PASSWDMISMATCH(nick, hostname) ":" + hostname + " 464 " + nick + " :Password incorrect !\r\n"
#define ERROR_ALREADYREGISTERED(nick, hostname) ":" + hostname + " 462 " + nick + " :You are already registered !\r\n"
#define ERROR_NOTREGISTERED(nick, hostname) ":" + hostname + " 451 " + nick + " :You have not registered !\r\n"

#define ERROR_ERRONEUSNICKNAME(nick, hostname) ":" + hostname + " 432 " + nick + " :Erroneus nickname !\r\n"
#define ERROR_NONICKNAMEGIVEN(nick, hostname) ":" + hostname + " 431 " + nick + " :No nickname given !\r\n"
#define ERROR_NICKNAMEINUSE(nick, hostname) ":" + hostname + " 433 " + nick + " :Nickname is already in use !\r\n"
#define REPLY_NICKCHANGE(oldNick, nick) ":" + oldNick + " NICK " + nick + "\r\n"

#define ERROR_NEEDMOREPARAMS(nick, hostname) (std::string (":") + hostname + std::string(" 461 ") + nick + " :Not enough parameters !\r\n")

#define PART_REPLY(nickname, username, hostname, channel) ":" + std::string(nickname) + "!~" + std::string(username) + "@" + std::string(hostname) + " PART " + std::string(channel) + "\r\n"

#define RPL_AWAY(sender , username, hostname, message , nick_resever) ":" + std::string(sender) + "!~" + std::string(username) + "@" + std::string(hostname) + " PRIVMSG " + nick_resever + " :" + message + "\r\n"

#define ERROR_BADCHANNELNAME(nick, hostname, channelname) ":" + hostname + " 476 " + nick + " " + channelname + " :Invalid channel name." + "\r\n"
#define ERROR_CHANNELISFULL(nick, channelName) (std::string(":") + nick + " 471 " + channelName + " :Cannot join channel (+l)\r\n")
#define ERROR_BADCHANNELKEY(nick, hostname, channelName) (std::string(":") + hostname + " 475 " + nick + " " + channelName + " :Cannot join channel (+K) - bad key\r\n")
#define ERROR_INVITEONLY(nick, channelName) ":" + nick + " 473 " + channelName + " :Cannot join channel (+i)\r\n"


#define REPLY_TOPIC(hostname, nick, channel, setter, topic) ":" + hostname + " 332 " + nick + " " + channel + " :" + setter + " has set a new topic: " + topic + "\r\n"

#define REPLY_TOPICWHOTIME(topicsetter, time, nick, hostname, channelName) ":" + hostname + " 333 " + nick + " " + channelName + " " + topicsetter + "!~" + topicsetter + "@" + hostname + " " + time + "\r\n"
    

#define REPLY_MODEIS(channel, hostname, mode) ":" + hostname + " MODE " + channel + " " + mode + "\r\n"
#define REPLY_MODEISLIMIT(channel, hostname, mode, newlimit) ":" + hostname + " MODE " + channel + " " + mode + " " + newlimit + "\r\n"
#define REPLY_MODEISOP(channel, hostname, mode, target) ":" + hostname + " MODE " + channel + " " + mode + " " + target + "\r\n"
#define ERROR_INVALIDMODEPARAM(channel, hostname, flag) ":" + hostname + " 696 " + channel + " " + flag + " * you must specifiy a parameter for the op mode\r\n"
#define ERROR_USERNOTINCHANNEL(hostname, channel) ":" + hostname + " 441 " + channel + " " + ":they aren't on that channel\r\n"
#define ERROR_NOPRIVILEGES(hostname, channel) ":" + hostname + " 482 " + channel + " " + ":You're not a channel operator\r\n"

#define REPLY_CREATIONTIME(hostname, channelName, nick, time) ":" + hostname + " 329 " + nick + " " + channelName + " " + time + "\r\n"
#define REPLY_CHANNELMODES(hostname, channelName, nick, channelmode) ":" + hostname + " 324 " + nick + " " + channelName + " " + channelmode + "\r\n"

#define REPLY_INVITING(hostname, inviting, invited, channel) ":" + hostname + " 341 " + inviting + " " + invited + " " + channel + " :Inviting " + invited + " to " + channel + "\r\n"
#define REPLY_INVITE(nick, username, clienthostname, invited, channel) ":" + nick + "!" + username + "@" + clienthostname + " INVITE " + invited + " :" + channel + "\r\n"
#define ERROR_NOTONCHANNEL(hostname, channel) ":" + hostname + " 442 " + channel + " " + ":You're not on that channel\r\n"
#define ERROR_NOSUCHNICK(hostname, channel, argument) ":" + hostname + " 401 " + channel + " " + argument + " :No such nickname\r\n"
#define ERROR_USERONCHANNEL(hostname, channel, nick) ":" + hostname + " 443 " + nick + " " + channel + "  :is already on channel\r\n"

#define ERROR_INPUTTOOLONG(nick, hostname) ":" + hostname + " 417 " + nick + " :Input line was too long !\r\n"

#define REPLY_VIEWTOPIC(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " :" + topic + "\r\n"
#define REPLY_VIEWTIME(hostname, nick, channel, time) ":" + hostname + " 333 " + nick + " " + channel + " " + nick + " " + time + "\r\n"
#define REPLY_SETTOPIC(nick, hostname, channel, topic) ":" + nick + "!" + nick + "@" + hostname + " TOPIC " + channel + " :" + topic + "\r\n"

#define REPLY_BOT(hostname, nick, message) ":" + hostname + " 001 " + nick + " Dad joke: " + message + "\r\n"
#define ERROR_BOT(hostname, nick) ":" + hostname + " 450 " + nick + "enter [BOT generate] to generate a joke" + "\r\n"
#define REPLY_UMODEIS(hostname, channelname) ":" + hostname + " MODE " + channelname + " +nt\r\n"

#define REPLY_YOUREOPER(hostname, nick) ":" + hostname + " 381 " + nick + ":You are now an IRC operator\r\n"
#define REPLY_KICK(kicker, username, host, channel, targetuser, reason) ":" + kicker + "!" + username + "@" + host + " KICK " + channel + " " + targetuser + " :" + reason + "\r\n"
#define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
#define ERROR_UNKNOWNCOMMAND(nick, hostname, command) ":" + hostname + " 421 " + nick + " " + command + " :Unknown command\r\n"

#define REPLY_YOUREOPER(hostname, nick) ":" + hostname + " 381 " + nick + ":You are now an IRC operator\r\n"
#define REPLY_KICK(kicker, username, host, channel, targetuser, reason) ":" + kicker + "!" + username + "@" + host + " KICK " + channel + " " + targetuser + " :" + reason + "\r\n"
#define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
#define ERROR_UNKNOWNCOMMAND(nick, hostname, command) ":" + hostname + " 421 " + nick + " " + command + " :Unknown command\r\n"

#define RPL_NOTOPIC(hostname, channel) ":" + hostname + " 331 " + channel + " :No topic is set\r\n"
#define RPL_TOPIC(hostname, channel, topic) ":" + hostname + " 332 " + channel + " :" + topic + "\r\n"
#define ERR_NOTONCHANNEL(hostname, channel, nick) ":" + hostname + " 442 " + nick + " " + channel + " :You're not on that channel\r\n"
#define ERR_NOTEXTTOSEND(hostname) ":" + hostname + " 412 " + ":No text to send\r\n"

#define CRLF "\r\n"

#define REPLY_JOIN(nick, username, ipaddress, channelname) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + CRLF
#define REPLY_NAMREPLY(hostname, nick, channelname, clients) ":" + hostname + " 353 " + nick + " = " + channelname + " :" + clients + CRLF
#define REPLY_NAMREPLYmsg(hostname, nick, channelname, clients) ":" + hostname + " 353 " + nick + " @ " + channelname + " :" + clients + CRLF
#define REPLY_ENDOFNAMES(hostname, nick, channelname) ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list.\r\n"
#define REPLY_TOPICDISPLAY(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " " + topic + CRLF
#define RPL_CREATIONTIME(nickname, channelname, creationtime) \
    (":" + nickname + " 329 " + nickname + " " + channelname + " " + creationtime + CRLF)
#define RPL_CHANNELMODES(nickname, channelname, modes) \
    (":" + nickname + " 324 " + nickname + " " + channelname + " " + modes + CRLF)
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) \
    (":" + hostname + " MODE " + channelname + " " + mode + " " + arguments + CRLF)


#define ERR_NOTOPERATOR(channelname) (": 482 " + channelname + " :You're not a channel operator" + CRLF)
#define ERR_NEEDMODEPARM(channelname, mode) \
    (": 696 " + channelname + " * You must specify a parameter for the key mode " + mode + CRLF)
#define ERR_INVALIDMODEPARM(channelname, mode) \
    (": 696 " + channelname + " :Invalid mode parameter " + mode + CRLF)
#define ERR_KEYSET(channelname) \
    (": 467 " + channelname + " :Channel key already set" + CRLF)
#define ERR_UNKNOWNMODE(nickname, channelname, mode) \
    (": 472 " + nickname + " " + channelname + " " + mode + " :is not a recognised channel mode" + CRLF)
#define ERR_NOTENOUGHPARAM(nickname) \
    (": 461 " + nickname + " :Not enough parameters" + CRLF)
#define ERR_NOSUCHNICK(channelname, name) \
    (": 401 " + channelname + " " + name + " :No such nick/channel" + CRLF)

#endif