/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 14:50:25 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 18:33:40 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

int Server::ParseJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd)
{
    std::vector<std::string> tmp;
    std::string chanelStr, PassStr, buff;
    std::istringstream iss(cmd);
    while (iss >> cmd)
        tmp.push_back(cmd);

    if (tmp.size() < 2)
    {
        token.clear();
        return 0;
    }

    tmp.erase(tmp.begin());
    chanelStr = tmp[0];
    tmp.erase(tmp.begin());

    if (!tmp.empty())
    {
        PassStr = tmp[0];
        tmp.clear();
    }

    for (size_t i = 0; i < chanelStr.size(); i++)
    {
        if (chanelStr[i] == ',')
        {
            token.push_back(std::make_pair(buff, ""));
            buff.clear();
        }
        else
        {
            buff += chanelStr[i];
        }
    }
    token.push_back(std::make_pair(buff, ""));

    if (!PassStr.empty())
    {
        size_t j = 0;
        buff.clear();

        for (size_t i = 0; i < PassStr.size(); i++)
        {
            if (PassStr[i] == ',')
            {
                // Ensure j is within bounds of token size
                if (j < token.size())
                {
                    token[j].second = buff;
                    j++;
                }
                buff.clear();
            }
            else
            {
                buff += PassStr[i];
            }
        }
        if (j < token.size())
        {
            token[j].second = buff;
        }
    }

    for (size_t i = 0; i < token.size(); i++)
    {
        token[i].first.erase(0, token[i].first.find_first_not_of(' '));
        token[i].first.erase(token[i].first.find_last_not_of(' ') + 1);

        if (token[i].first.empty())
        {
            token.erase(token.begin() + i--);
        }
    }

    for (size_t i = 0; i < token.size(); i++)
    {
        char prefix = *(token[i].first.begin());
        if (prefix != '#' && prefix != '&')
        {
            senderror(403, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :No such channel\r\n");
            token.erase(token.begin() + i--);
        }
    }

    return token.empty() ? 0 : 1;
}

int Server::SearchForClients(std::string nickname)
{
    int count = 0;
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].GetClientInChannel(nickname))
            count++;
    }
    return count;
}

bool IsInvited(Client *cli, std::string ChName, Channel &channel, int flag)
{
    if (channel.GetLimit() && channel.GetClientsNumber() >= channel.GetLimit())
    {
        return false;
    }
    if (cli->GetInviteChannel(ChName))
    {
        if (flag == 1)
            cli->RmChannelInvite(ChName);
        return true;
    }

    return false;
}


void Server::ExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd)
{
    Client *cli = GetClient(fd);

    if (this->channels[j].GetClientInChannel(cli->GetNickName()))
    {
        senderror(443, cli->GetNickName(), " " + token[i].first, cli->GetFd(), " :is already on channel\r\n");
        return;
    }

    if (SearchForClients(cli->GetNickName()) >= 10)
    {
        senderror(405, cli->GetNickName(), cli->GetFd(), " :You have joined too many channels\r\n");
        return;
    }

    if (this->channels[j].GetLimit() && this->channels[j].GetClientsNumber() >= this->channels[j].GetLimit())
    {
        if (IsInvited(cli, token[i].first,channels[j], 1))
        {
            senderror(471, cli->GetNickName(), " " + token[i].first, cli->GetFd(), " :Cannot join channel (+l), channel is full\r\n");
            return;
        }
        else
        {
            senderror(471, cli->GetNickName(), " " + token[i].first, cli->GetFd(), " :Cannot join channel (+l)\r\n");
            return;
        }
        return;
    }

    if (!this->channels[j].GetPassword().empty() && this->channels[j].GetPassword() != token[i].second)
    {
        if (!IsInvited(cli, token[i].first, channels[j],0))
        {
            senderror(475, cli->GetNickName(), " " + token[i].first, cli->GetFd(), " :Cannot join channel (+k) - bad key\r\n");
            return;
        }
    }
    if (this->channels[j].GetInvitOnly() && !IsInvited(cli, token[i].first, channels[j],1))
    {
        senderror(473, cli->GetNickName(), " " + token[i].first, cli->GetFd(), " :Cannot join channel (+i)\r\n");
        return;
    }
    this->channels[j].add_client(*cli);
    if (channels[j].GetTopicName().empty())
        _sendResponse(REPLY_JOIN(cli->GetNickName(), cli->GetUserName(), cli->getIpAdd(), token[i].first) +
                      REPLY_NAMREPLYmsg(cli->get_client_host(cli->getIpAdd()), cli->GetNickName(), channels[j].GetName(), channels[j].clientChannel_list()) +
                      REPLY_ENDOFNAMES(cli->get_client_host(cli->getIpAdd()), cli->GetNickName(), channels[j].GetName()), fd);
    else
        _sendResponse(REPLY_JOIN(cli->GetNickName(), cli->GetUserName(), cli->getIpAdd(), token[i].first) +
                      REPLY_TOPICDISPLAY(cli->get_client_host(cli->getIpAdd()), cli->GetNickName(), channels[j].GetName(), channels[j].GetTopicName()) +
                      REPLY_NAMREPLY(cli->get_client_host(cli->getIpAdd()), cli->GetNickName(), channels[j].GetName(), channels[j].clientChannel_list()) +
                      REPLY_ENDOFNAMES(cli->get_client_host(cli->getIpAdd()), cli->GetNickName(), channels[j].GetName()), fd);

    channels[j].sendTo_all(REPLY_JOIN(cli->GetNickName(), cli->GetUserName(), cli->getIpAdd(), token[i].first), fd);
}



void Server::NotExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int fd)
{
    if (SearchForClients(GetClient(fd)->GetNickName()) >= 10)
    {
        senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n");
        return;
    }
    Channel newChannel;
    newChannel.SetName(token[i].first);
    newChannel.add_admin(*GetClient(fd));
    newChannel.set_createiontime();
    this->channels.push_back(newChannel);
    _sendResponse(REPLY_JOIN(GetClient(fd)->GetNickName(), GetClient(fd)->GetUserName(), GetClient(fd)->getIpAdd(), token[i].first) +
                      REPLY_NAMREPLY(GetClient(fd)->get_client_host(GetClient(fd)->getIpAdd()), GetClient(fd)->GetNickName(), newChannel.GetName(), newChannel.clientChannel_list()) +
                      REPLY_ENDOFNAMES(GetClient(fd)->get_client_host(GetClient(fd)->getIpAdd()), GetClient(fd)->GetNickName(), newChannel.GetName()),
                  fd);
}

void Server::JOIN(std::string cmd, int fd)
{
    std::vector<std::pair<std::string, std::string> > token;

    if (!ParseJoin(token, cmd, fd))
    {
        senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
        return;
    }
    if (token.size() > 10)
    {
        senderror(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many channels\r\n");
        return;
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        bool flag = false;
        for (size_t j = 0; j < this->channels.size(); j++)
        {
            if (this->channels[j].GetName() == token[i].first)
            {
                ExistCh(token, i, j, fd);
                flag = true;
                break;
            }
        }
        if (!flag)
            NotExistCh(token, i, fd);
    }
}