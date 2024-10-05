/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 00:48:37 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 16:05:37 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include <iostream>
#include <vector>
#include <string>

void FindSubStrKK(std::string cmd, std::string tofind, std::string &str)
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
    for (; i < str.size() && str[i] == ' '; i++)
        ;
    str = str.substr(i);
}

std::string ExtractReasonForKK(std::string &cmd, std::vector<std::string> &tmp)
{
    std::stringstream ss(cmd);
    std::string str, reason;
    int count = 3;
    while (ss >> str && count--)
        tmp.push_back(str);
    if (tmp.size() != 3)
        return std::string("");
    FindSubStrKK(cmd, tmp[2], reason);
    return reason;
}

std::string Server::SplitCmdKick(std::string cmd, std::vector<std::string> &channels, std::vector<std::string> &users, int fd)
{

    std::vector<std::string> tmp;
    std::string reason = ExtractReasonForKK(cmd, tmp);

    if (tmp.size() < 3)
        return std::string("");
    tmp.erase(tmp.begin()); // 'KICK' erased

    std::string channelStr = tmp[0];
    std::string userStr = tmp[1];

    std::string tmpStr;

    channels.clear();
    for (size_t i = 0; i < channelStr.size(); i++)
    {
        if (channelStr[i] == ',')
        {
            channels.push_back(tmpStr);
            tmpStr.clear();
        }
        else
        {
            tmpStr += channelStr[i];
        }
    }
    channels.push_back(tmpStr);

    users.clear();
    tmpStr.clear();
    for (size_t i = 0; i < userStr.size(); i++)
    {
        if (userStr[i] == ',')
        {
            users.push_back(tmpStr);
            tmpStr.clear();
        }
        else
        {
            tmpStr += userStr[i];
        }
    }
    users.push_back(tmpStr);

    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].empty())
            channels.erase(channels.begin() + i--);
    }
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (*(channels[i].begin()) != '#' && *(channels[i].begin()) != '&')
        {
            senderror(403, GetClient(fd)->GetNickName(), channels[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
            channels.erase(channels.begin() + i--);
        }
    }

    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].empty())
            users.erase(users.begin() + i--);
    }
    return reason;
}
void Server::KICK(std::string cmd, int fd)
{
    std::vector<std::string> channelss, userss;
    std::string reason = SplitCmdKick(cmd, channelss, userss, fd);

    if (userss.empty())
    {
        senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
        return;
    }
    if (channelss.empty())
    {
        senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
        return;
    }
    std::vector<std::string> channelsToDelete;
    for (size_t i = 0; i < channelss.size(); i++)
    {
        Channel *ch = GetChannel(channelss[i]);

        if (!ch)
        {
            senderror(403, GetClient(fd)->GetNickName(), channelss[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
            continue;
        }

        if (!ch->get_admin(fd))
        {
            senderror(482, GetClient(fd)->GetNickName(), channelss[i], GetClient(fd)->GetFd(), " :You're not channel operator\r\n");
            continue;
        }

        for (size_t j = 0; j < userss.size(); j++)
        {
            Client *targetUser = ch->GetClientInChannel(userss[j]);
            if (!targetUser)
            {
                senderror(441, GetClient(fd)->GetNickName(), channelss[i], GetClient(fd)->GetFd(), " :They aren't on that channel\r\n");
                continue;
            }

            std::stringstream ss;
            ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << GetClient(fd)->getIpAdd() << " KICK " << channelss[i] << " " << userss[j];
            if (!reason.empty())
                ss << " " << reason;
            ss << "\r\n";

            ch->sendTo_all(ss.str());

            if (ch->get_admin(targetUser->GetFd()))
            {
                ch->remove_admin(targetUser->GetFd());
                ch->promoteClientToAdminIfNone();
            }
            else
            {
                ch->remove_client(targetUser->GetFd());
            }

            if (ch->GetClientsNumber() == 0)
            {
                channelsToDelete.push_back(channelss[i]);
            }
        }
    }

    for (size_t i = 0; i < channelsToDelete.size(); i++)
    {
        Channel *ch = GetChannel(channelsToDelete[i]);
        if (ch)
        {
            DeleteChannel(channelsToDelete[i]);
        }
    }
}