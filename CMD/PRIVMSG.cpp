/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 19:10:17 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 04:55:34 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void FindSubStrPM(std::string cmd, std::string tofind, std::string &str)
{
	size_t i = 0;
	for (; i < cmd.size(); i++)
	{
		if (cmd[i] != ' ')
		{
			std::string tmp;
			for (; i < cmd.size() && cmd[i] != ' '; i++)
				tmp += cmd[i];
			if (tmp == tofind)
				break;
			else 
				tmp.clear();
		}
	}
	if (i < cmd.size()) 
		str = cmd.substr(i);
	i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	str = str.substr(i);
}

std::string ExtractMessageFromCmd(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, msg;
	int count = 2;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 2) 
		return std::string("");
	FindSubStrPM(cmd, tmp[1], msg);
	return msg;
}

std::string ParseRecipientsAndMessage(std::string cmd, std::vector<std::string> &tmp)
{
	std::string str = ExtractMessageFromCmd(cmd, tmp);
	if (tmp.size() != 2)
	{
		tmp.clear();
		return std::string("");
	}
	tmp.erase(tmp.begin());
	std::string str1 = tmp[0];
	std::string str2;
	tmp.clear();
	for (size_t i = 0; i < str1.size(); i++)
	{
		if (str1[i] == ',')
		{
				tmp.push_back(str2);
				str2.clear();
		}
		else 
			str2 += str1[i];
	}
	tmp.push_back(str2);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i].empty())
			tmp.erase(tmp.begin() + i--);
	}
	if (str[0] == ':') 
		str.erase(str.begin());
	
	return  str;
	
}

void	Server::CheckForChannels_Clients(std::vector<std::string> &tmp, int fd)
{
	for(size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i][0] == '#' || tmp[i][0] == '&')
		{
			if(!GetChannel(tmp[i]))
			{
				senderror(403,GetClient(fd)->GetNickName(), "" + tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
				tmp.erase(tmp.begin() + i);
				i--;
			}
			else if (!GetChannel(tmp[i])->GetClientInChannel(GetClient(fd)->GetNickName()))
			{
				senderror(404, GetClient(fd)->GetNickName(), "" + tmp[i], GetClient(fd)->GetFd(), " :Cannot send to channel\r\n");
				tmp.erase(tmp.begin() + i);
				i--;
			}
		}
		else
		{
			if (!GetClientNick(tmp[i]))
			{
				senderror(401,GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such nick\r\n");
				tmp.erase(tmp.begin() + i);
				i--;
			}
		}
	}
}

void	Server::PRIVMSG(std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string message = ParseRecipientsAndMessage(cmd, tmp);
	if (!tmp.size())
	{
		senderror(411, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No recipient given (PRIVMSG)\r\n");
		return;
	}
	if (message.empty())
	{
		senderror(412, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :No text to send\r\n");
		return;
	}
	if (tmp.size() > 10) 
	{
		senderror(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many recipients\r\n");
		return;
	}
	CheckForChannels_Clients(tmp, fd);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		if (tmp[i][0] == '#' || tmp[i][0] == '&')
		{
			
			std::string resp = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " PRIVMSG " + tmp[i] + " :" + message + "\r\n";
			GetChannel(tmp[i])->sendTo_all(resp, fd);
		}
		else{
			std::string resp = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " PRIVMSG " + tmp[i] + " :" + message + "\r\n";
			_sendResponse(resp, GetClientNick(tmp[i])->GetFd());
		}
	}
}