/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 14:50:25 by frukundo          #+#    #+#             */
/*   Updated: 2024/09/09 07:03:35 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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
    tmp.erase(tmp.begin()); // rm 1st part of tmp which is JOIN
    chanelStr = tmp[0];
    tmp.erase(tmp.begin());
    if (!tmp.empty())
    {
        PassStr = tmp[0];
        tmp.clear();
    }
    // parse the channel name
    for (size_t i = 0; i < chanelStr.size(); i++)
    {
        if (chanelStr[i] == ',')
        {
            token.push_back(std::make_pair(buff, ""));
            buff.clear();
        }
        else
            buff += chanelStr[i];
    }
    token.push_back(std::make_pair(buff, ""));
    // parse the password(keys) if available
    if (!PassStr.empty())
    {
        size_t j = 0;
        buff.clear();
        for (size_t i = 0; i < PassStr.size(); i++)
        {
            if (PassStr[i] == ',')
            {
                token[j].second = buff;
                j++;
                buff.clear();
            }
            else
                buff += PassStr[i];
        }
        token[j].second = buff;
    }
    for (size_t i = 0; i < token.size(); i++) // erase the empty channel name
    {
        if (token[i].first.empty())
            token.erase(token.begin() + i--);
    }
    // if the channel doesn't exit or invalid
    for (size_t i = 0; i < token.size(); i++)
    {
        if (*(token[i].first.begin()) != '#') // ERR_NOSUCHCHANNEL (403)
        {
            senderror(403, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :No such channel\r\n");
            token.erase(token.begin() + i--);
        }
        else
            token[i].first.erase(token[i].first.begin()); // if valid rm # on it
    }
    return 1;
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

bool IsInvited(Client *cli, std::string ChName, int flag)
{
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
    if (this->channels[j].GetClientInChannel(GetClient(fd)->GetNickName())) // if the client is already registered
        return;
    // If the client is already in the channel, send ERR_USERONCHANNEL (443)
    if (this->channels[j].GetClientInChannel(GetClient(fd)->GetNickName()))
    {
        senderror(443, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :is already on channel\r\n");
        return;
    }

    if (SearchForClients(GetClient(fd)->GetNickName()) >= 10) // ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
    {
        senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n");
        return;
    }

    if (!this->channels[j].GetPassword().empty() && this->channels[j].GetPassword() != token[i].second)
    { // ERR_BADCHANNELKEY (475) // if the password is incorrect
        if (!IsInvited(GetClient(fd), token[i].first, 0))
        {
            senderror(475, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+k) - bad key\r\n");
            return;
        }
    }
    if (this->channels[j].GetInvitOnly())
    { // ERR_INVITEONLYCHAN (473) // if the channel is invite only
        if (!IsInvited(GetClient(fd), token[i].first, 1))
        {
            senderror(473, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+i)\r\n");
            return;
        }
    }
    if (this->channels[j].GetLimit() && this->channels[j].GetClientsNumber() >= this->channels[j].GetLimit()) // ERR_CHANNELISFULL (471) // if the channel reached the limit of number of clients
    {
        senderror(471, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join channel (+l)\r\n");
        return;
    }
    // add the client to the channel
    Client *cli = GetClient(fd);
    this->channels[j].add_client(*cli);
    if (channels[j].GetTopicName().empty())
        _sendResponse(REPLY_JOIN(GetClient(fd)->GetNickName(), GetClient(fd)->GetUserName(), token[i].first, GetClient(fd)->getIpAdd()) +
                          REPLY_NAMREPLY(GetClient(fd)->getHostname(), channels[j].clientChannel_list(), channels[j].GetName(), GetClient(fd)->GetNickName()) +
                          REPLY_ENDOFNAMES(GetClient(fd)->getHostname(),GetClient(fd)->GetNickName(), channels[j].GetName()),
                      fd);
    else
        _sendResponse(REPLY_JOIN(GetClient(fd)->GetNickName(), GetClient(fd)->GetUserName(), token[i].first, GetClient(fd)->getIpAdd()) +
                          RPL_TOPICIS(GetClient(fd)->GetNickName(), channels[j].GetName(), channels[j].GetTopicName()) +
                          REPLY_NAMREPLY(GetClient(fd)->getHostname(), channels[j].clientChannel_list(), channels[j].GetName(), GetClient(fd)->GetNickName()) +
                          REPLY_ENDOFNAMES(GetClient(fd)->getHostname(),GetClient(fd)->GetNickName(), channels[j].GetName()),
                      fd);
    channels[j].sendTo_all(REPLY_JOIN(GetClient(fd)->GetNickName(), GetClient(fd)->GetUserName(), token[i].first, GetClient(fd)->getIpAdd()), fd);
}

void Server::NotExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int fd)
{
    if (SearchForClients(GetClient(fd)->GetNickName()) >= 10) // ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
    {
        senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many channels\r\n");
        return;
    }
    Channel newChannel;
    newChannel.SetName(token[i].first);
    newChannel.add_admin(*GetClient(fd));
    newChannel.set_createiontime();
    this->channels.push_back(newChannel);
    // notifiy that the client joined the channel
    _sendResponse(REPLY_JOIN(GetClient(fd)->GetNickName(), GetClient(fd)->GetUserName(), newChannel.GetName(), GetClient(fd)->getIpAdd()) +
                      REPLY_NAMREPLY(GetClient(fd)->getHostname(),newChannel.clientChannel_list(), newChannel.GetName(),GetClient(fd)->GetNickName() ) +
                      REPLY_ENDOFNAMES(GetClient(fd)->getHostname(),GetClient(fd)->GetNickName(), newChannel.GetName()),
                  fd);
}

void Server::JOIN(std::string cmd, int fd)
{
    std::vector<std::pair<std::string, std::string> > token;

    if (!ParseJoin(token, cmd, fd)) // ERR_NEEDMOREPARAMS (461) // if the channel name is empty
    {
        senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
        return;
    }
    if (token.size() > 10) // ERR_TOOMANYTARGETS (407) // if more than 10 Channels
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