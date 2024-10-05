/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 05:23:02 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 04:55:44 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

std::string Server::TimeCreation()
{
	std::time_t current = std::time(NULL);
	std::tm *timeinfo = std::localtime(&current);

	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

	std::stringstream res;
	res << buffer;

	return res.str();
}
std::string Server::gettopic(std::string &input)
{
	size_t pos = input.find(":");
	if (pos == std::string::npos)
	{
		return "";
	}
	return input.substr(pos);
}

int Server::getpos(std::string &cmd)
{
	for (int i = 0; i < (int)cmd.size(); i++)
		if (cmd[i] == ':' && (cmd[i - 1] == 32))
			return i;
	return -1;
}

void Server::Topic(std::string &cmd, int &fd)
{
	if (cmd == "TOPIC :" || cmd == "topic :")
	{
		senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");
		return;
	}
	std::vector<std::string> scmd = split_cmd(cmd);
	if (scmd.size() == 1)
	{
		senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");
		return;
	}
	
	std::string nmch = scmd[1];
	if (!GetChannel(nmch))
	{
		senderror(403, "" + nmch, fd, " :No such channel\r\n");
		return;
	}
	if (!(GetChannel(nmch)->get_client(fd)) && !(GetChannel(nmch)->get_admin(fd)))
	{
		senderror(442, "" + nmch, fd, " :You're not on that channel\r\n");
		return;
	}
	if (!GetChannel(nmch)->get_admin(fd) && GetChannel(nmch)->Gettopic_restriction())
	{
		_sendResponse(ERR_NOTOPERATOR(GetChannel(nmch)->GetName()), fd);
		return;
	}
	if (scmd.size() == 2)
	{
		if (GetChannel(nmch)->GetTopicName() == "")
		{
			_sendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + nmch + " :No topic is set\r\n", fd);
			return;
		}
		if (GetChannel(nmch)->GetTopicName().empty())
		{
			_sendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + nmch + " :No topic is set\r\n", fd);
			return;
		}
		size_t pos = GetChannel(nmch)->GetTopicName().find(":");
		if (GetChannel(nmch)->GetTopicName() != "" && pos == std::string::npos)
		{
			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + nmch + " " + GetChannel(nmch)->GetTopicName() + "\r\n", fd);
			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + nmch + " " + GetClient(fd)->GetNickName() + " " + GetChannel(nmch)->GetTime() + "\r\n", fd);
			return;
		}
		else
		{
			size_t pos = GetChannel(nmch)->GetTopicName().find(" ");
			if (pos == 0)
				GetChannel(nmch)->GetTopicName().erase(0, 1);
			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + nmch + " " + GetChannel(nmch)->GetTopicName() + "\r\n", fd);
			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + nmch + " " + GetClient(fd)->GetNickName() + " " + GetChannel(nmch)->GetTime() + "\r\n", fd);
			return;
		}
	}

	if (scmd.size() >= 3)
	{
		std::vector<std::string> tmp;
		int pos = getpos(cmd);
		if (pos == -1 || scmd[2][0] != ':')
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(scmd[2]);
		}
		else
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(cmd.substr(getpos(cmd)));
		}

		if (tmp[2][0] == ':' && tmp[2][1] == '\0')
		{
			senderror(331, "" + nmch, fd, " :No topic is set\r\n");
			return;
		}

		if (!GetChannel(nmch)->get_admin(fd) && GetChannel(nmch)->Gettopic_restriction())
		{
			senderror(482, "" + nmch, fd, " :You're Not a channel operator\r\n");
			return;
		}

		else if (!GetChannel(nmch)->Gettopic_restriction() || GetChannel(nmch)->get_admin(fd))
		{

			if (tmp[2][0] == ':' && tmp[2][1] == ':')
				tmp[2] = tmp[2].substr(1);
			GetChannel(nmch)->SetTime(TimeCreation());
			GetChannel(nmch)->SetTopicName(tmp[2]);

			std::string rpl;
			std::string topic = GetChannel(nmch)->GetTopicName();
			
			if (topic[0] != ':')
				rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " TOPIC " + nmch + " :" + topic + "\r\n";
			else if (topic[0] == ':' && topic[1] == ':')
			{
				topic.erase(topic.begin());
				rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " TOPIC " + nmch + " " + topic + "\r\n";
			}
			else
				rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " TOPIC " + nmch + " " + topic + "\r\n";

			GetChannel(nmch)->sendTo_all(rpl);
		}
		else
		{
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				GetChannel(nmch)->SetTime(TimeCreation());
				
				GetChannel(nmch)->SetTopicName(tmp[2]);
				rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " TOPIC " + nmch + " " + GetChannel(nmch)->GetTopicName() + "\r\n";
			}
			else if (tmp[2][0] == ':' && tmp[2][1] == ':')
			{
				tmp[2] = tmp[2].substr(1);
				GetChannel(nmch)->SetTopicName(tmp[2]);
				GetChannel(nmch)->SetTime(TimeCreation());
				rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " TOPIC " + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332)
			}
			else
			{
				GetChannel(nmch)->SetTopicName(tmp[2]);
				GetChannel(nmch)->SetTime(TimeCreation());
				rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " TOPIC " + nmch + " :" + GetChannel(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332)
			}
			GetChannel(nmch)->sendTo_all(rpl);
		}
	}
}
