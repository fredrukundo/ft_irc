/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 04:17:11 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 04:55:39 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void FindSubStrQT(std::string cmd, std::string tofind, std::string &str)
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

std::string SplitQuit(std::string cmd)
{
    std::istringstream stm(cmd);
    std::string reason, str;
    stm >> str;
    FindSubStrQT(cmd, str, reason);
    if (reason.empty())
        return std::string("Quit");
    if (reason[0] != ':')
    {
        for (size_t i = 0; i < reason.size(); i++)
        {
            if (reason[i] == ' ')
            {
                reason.erase(reason.begin() + i, reason.end());
                break;
            }
        }
    }
    return reason;
}

void Server::QUIT(std::string cmd, int fd)
{
    std::string reason;
    reason = SplitQuit(cmd);
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].get_client(fd))
        {
            channels[i].remove_client(fd);
            if (channels[i].GetClientsNumber() == 0)
                channels.erase(channels.begin() + i);
            else
            {
                std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd() + " QUIT " + reason + "\r\n";
                channels[i].sendTo_all(rpl);
            }
        }
        else if (channels[i].get_admin(fd))
        {
            channels[i].remove_admin(fd);
            channels[i].promoteClientToAdminIfNone();
            if (channels[i].GetClientsNumber() == 0)
                channels.erase(channels.begin() + i);
            else
            {
                std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd()+ " QUIT " + reason + "\r\n";
                channels[i].sendTo_all(rpl);
            }
        }
    }
    std::cout << RED << "client of socket " << YEL <<  "<" << fd << ">" << RED << " : DISCONNECTED" << WHI<< std::endl;
    close(fd);
}