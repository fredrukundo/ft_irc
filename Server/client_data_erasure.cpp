/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_data_erasure.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 16:06:54 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

int Server::erase_client_data(int client_sock, int itteration)
{
    if (itteration >= 0)
    {
        close(client_sock);
        this->multi_connections.erase(this->multi_connections.begin() + itteration);
    }
    if (!this->authenticated_clients.empty())
    {
        std::vector<int>::iterator itr = std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock);
        if (itr != this->authenticated_clients.end())
            authenticated_clients.erase(itr);
    }
    if (!this->clients_who_set_pass.empty())
    {
        std::vector<int>::iterator itr = std::find(clients_who_set_pass.begin(), clients_who_set_pass.end(), client_sock);
        if (itr != this->clients_who_set_pass.end())
            clients_who_set_pass.erase(itr);
    }
    if (!this->nicknames.empty())
    {
        std::map<int, std::string>::iterator itr = this->nicknames.find(client_sock);
        if (itr != this->nicknames.end())
            this->nicknames.erase(itr);
    }
    if (!this->users.empty())
    {
        std::map<int, user_data>::iterator itr = this->users.find(client_sock);
        if (itr != this->users.end())
            users.erase(itr);
    }
    RmChannels(client_sock);
    for (size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].get_client(client_sock))
        {
            channels[i].remove_client(client_sock);
            if (channels[i].GetClientsNumber() == 0)
                channels.erase(channels.begin() + i);
        }
        else if (channels[i].get_admin(client_sock))
        {
            channels[i].remove_admin(client_sock);
            if (channels[i].GetClientsNumber() == 0)
                channels.erase(channels.begin() + i);
        }
    }
    
    return (0);
}

void Server::RemoveClient(int fd)
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetFd() == fd)
        {
            this->clients.erase(this->clients.begin() + i);
            return;
        }
    }
}
void Server::RemoveChannel(std::string name)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].GetName() == name)
        {
            this->channels.erase(this->channels.begin() + i);
            return;
        }
    }
}
void Server::RmChannels(int fd)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        int flag = 0;
        if (channels[i].get_client(fd))
        {
            channels[i].remove_client(fd);
            flag = 1;
        }
        else if (channels[i].get_admin(fd))
        {
            channels[i].remove_admin(fd);
            channels[i].promoteClientToAdminIfNone();
            flag = 1;
        }
        if (channels[i].GetClientsNumber() == 0)
        {
            channels.erase(channels.begin() + i);
            i--;
            continue;
        }
        if (flag)
        {
            std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@" + GetClient(fd)->getIpAdd()+ " QUIT " + "\r\n";
            channels[i].sendTo_all(rpl);
        }
    }
}
void Server::DeleteChannel(const std::string &channelName) {
    for (size_t i = 0; i < channels.size(); ++i) {

        if ( channels[i].GetName() == channelName) {
            channels.erase(channels.begin() + i);
            return;
        }
    }
}

