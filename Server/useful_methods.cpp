/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful_methods.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 04:54:20 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

std::string Client::get_client_host(std::string ip)
{
    struct in_addr addr;
    inet_aton(ip.c_str(), &addr);

    struct hostent* hostInfo = gethostbyaddr((const void*)&addr, sizeof(addr), AF_INET);
    if (hostInfo != NULL)
        return std::string(hostInfo->h_name);
    else
    {
        // Use inet_ntop to return the IP address if no hostname is found
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN);
        return std::string(ip_str); // Return the IP address instead
    }
}

void Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
    std::stringstream ss;
    std::string hostname = this->cli->get_client_host(this->cli->getIpAdd()); 
    if (clientname.empty())
    
        ss << ":" << hostname << " " << code << " * " << msg;
    else
        ss << ":" << hostname << " " << code << " * " << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() has faild" << std::endl;
}

void Server::senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
    std::string hostname = this->cli->get_client_host(this->cli->getIpAdd()); 
    std::stringstream ss;
    ss << ":" << hostname << " " << code << " " << clientname << " " << channelname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() has faild" << std::endl;
}

void Server::_sendResponse(std::string response, int fd)
{
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr << "send() has faild" << std::endl;
}

Client *Server::GetClient(int fd)
{

    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetFd() == fd)
            return &this->clients[i];
    }
    return NULL;
}
Client *Server::GetClientNick(std::string nickname)
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetNickName() == nickname)
            return &this->clients[i];
    }
    return NULL;
}

Channel *Server::GetChannel(std::string name)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].GetName() == name)
            return &channels[i];
    }
    return NULL;
}

std::string Server::GetPassword() { return this->password; }

void Server::SetPassword(std::string password) { this->password = password; }
std::string Server::convert_to_upC(std::string &input)
{
    std::string upC_input = input;
    for (unsigned long i = 0; i < input.length(); i++)
        upC_input[i] = (char)toupper(upC_input[i]);
    return (upC_input);
}

bool Server::check_for_nl(char *buff, size_t bytes_received)
{
    for (size_t i = 0; i < bytes_received; ++i)
        if (buff[i] == '\n')
            return true;
    return false;
}

std::vector<std::string> Server::split_cmd(std::string &cmd)
{
    std::vector<std::string> vec;
    std::istringstream stm(cmd);
    std::string token;
    while (stm >> token)
    {
        vec.push_back(token);
        token.clear();
    }
    return vec;
}