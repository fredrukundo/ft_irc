/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 14:16:55 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void Server::client_registration_greeting(int client_sock)
{
    std::time_t currentTime = std::time(NULL);
    std::tm* utcTime = std::gmtime(&currentTime);
    char buffer[512];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", utcTime);
    std::string local_time = buffer;
    this->_sendResponse(REPLY_WELCOME(this->nicknames[client_sock]), client_sock);
    this->_sendResponse(REPLY_YOURHOST(this->nicknames[client_sock], this->cli->get_client_host(this->cli->getIpAdd())), client_sock);
    this->_sendResponse(REPLY_CREATED(this->nicknames[client_sock], local_time), client_sock);
}

void Server::validate_client_credentials(int client_sock)
{
    if (this->is_pass_set == true && this->is_nick_set == true && this->is_user_set == true)
    {
        this->is_pass_set = false;
        this->is_nick_set = false;
        this->is_user_set = false;
        this->authenticated_clients.push_back(client_sock);
        this->client_registration_greeting(client_sock);
    }
    return;
}

void Server::process_authentication(std::string command, std::string input, int client_sock)
{
    if (command == "PASS")
    {
        this->pass_check(command, input, client_sock);
    }
    else if (command == "NICK")
    {
        this->nick_check(command, input, client_sock);
        this->validate_client_credentials(client_sock);
    }
    else if (command == "USER")
    {
        this->user_check(command, input, client_sock);
        this->validate_client_credentials(client_sock);
    }
    return;
}

int Server::already_registered_check(int client_sock)
{
    if (!this->authenticated_clients.empty())
    {
        if (std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock) != authenticated_clients.end())
        {
            this->senderror(462, this->nicknames[client_sock], client_sock, " :you may not register !\r\n");
            return (-1);
        }
    }
    return (0);
}

int Server::not_registered_check(int client_sock)
{
    std::vector<int>::iterator itr = std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock);
    if (itr == this->authenticated_clients.end())
    {
        this->senderror(451, this->nicknames[client_sock], client_sock, " :You have not registered !\r\n");
        return (-1);
    }
    return (0);
}

int Server::pass_check(std::string command, std::string input, int client_sock)
{
    this->cli = GetClient(client_sock);
    std::string given_password = input.substr(this->convert_to_upC(input).find(command) + command.length() + 1, input.length());
        if (this->already_registered_check(client_sock) == -1)
            return (0);
    if (given_password == this->password)
    {
        this->is_pass_set = true;
        cli->setRegistered(true);
        this->which_client = client_sock;
        std::vector<int>::iterator itr = std::find(clients_who_set_pass.begin(), clients_who_set_pass.end(), client_sock);
        if (itr == this->clients_who_set_pass.end())
            this->clients_who_set_pass.push_back(client_sock);
        return (0);
    }
    else
    {
        this->senderror(464, this->nicknames[client_sock], client_sock, " :Password incorrect !\r\n");
        this->is_pass_set = false;
        this->which_client = -1;
        if (this->is_nick_set == true)
        {
            this->is_nick_set = false;
            this->erase_client_data(client_sock, -1);
        }
        if (this->is_user_set == true)
        {
            this->is_user_set = false;
            this->erase_client_data(client_sock, -1);
        }
        return (0);
    }
    return (-1);
}

int Server::is_nickname_valid(std::string &nickname, int client_sock)
{
    if (!this->nicknames.empty())
    {
        for (std::map<int, std::string>::iterator itr = this->nicknames.begin(); itr != this->nicknames.end(); itr++)
        {
            if (itr->second == nickname && itr->first != client_sock)
            {
                this->senderror(433, this->nicknames[client_sock], client_sock, " :Nickname is already in use !\r\n");
                return (-1);
            }
        }
    }
    size_t pos = nickname.find_first_of(" :,*?!@.\t\r");
    if (pos != std::string::npos)
    {
        this->senderror(432, this->nicknames[client_sock], client_sock, " :Erroneus nickname !\r\n");
        return (-1);
    }
    return (0);
}

void Server::update_bool_throughout_multi_auth(std::string &command, int client_sock)
{
    if (command == "NICK")
    {
        if (!this->clients_who_set_pass.empty() && std::find(clients_who_set_pass.begin(), clients_who_set_pass.end(), client_sock) != clients_who_set_pass.end())
        {
            this->is_pass_set = true;
            this->which_client = client_sock;
            if (!this->users.empty())
            {
                std::map<int, user_data>::iterator itr = this->users.find(client_sock);
                if (itr != this->users.end() && !itr->second.username.empty() && !itr->second.realname.empty())
                    this->is_user_set = true;
                else
                    this->is_user_set = false;
            }
        }
        else
            this->is_pass_set = false;
    }
    else if (command == "USER")
    {
        if (!this->clients_who_set_pass.empty() && std::find(clients_who_set_pass.begin(), clients_who_set_pass.end(), client_sock) != clients_who_set_pass.end())
        {
            this->is_pass_set = true;
            this->which_client = client_sock;
            if (!this->nicknames.empty())
            {
                std::map<int, std::string>::iterator itr = this->nicknames.find(client_sock);
                if (itr != this->nicknames.end() && !itr->second.empty())
                    this->is_nick_set = true;
                else
                    this->is_nick_set = false;
            }
        }
        else
            this->is_pass_set = false;
    }
    return;
}

int Server::nick_check(std::string command, std::string input, int client_sock)
{
    this->cli = GetClient(client_sock);
    std::string nickname = input.substr(this->convert_to_upC(input).find(command) + command.length() + 1, input.length());
    if (this->is_nickname_valid(nickname, client_sock) == -1)
        return (0);
    this->update_bool_throughout_multi_auth(command, client_sock);
    if (!this->authenticated_clients.empty() && std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock) != authenticated_clients.end())
    {
        std::string oldnick = cli->GetNickName();
        cli->SetNickname(nickname);
        this->nicknames[client_sock] = nickname;
        std::string message =  REPLY_NICKCHANGE(oldnick, nickname); 
        _sendResponse(message, client_sock);
        this->is_nick_set = false;
        for (size_t i = 0; i < this->channels.size(); i++)
        {
            if (this->channels[i].GetClientInChannel(oldnick))
            {
                this->channels[i].update_nickname(oldnick,nickname);
                this->channels[i].sendTo_all(message, client_sock);
            }
        }
        return (0);
    }
    else if (this->is_pass_set == true && this->which_client == client_sock)
    {
        cli->SetNickname(nickname);
        this->nicknames[client_sock] = nickname;
        this->is_nick_set = true;
        return (0);
    }
    return (-1);
}

user_data Server::is_UserRealname_valid(std::string &user_message, int client_sock)
{
    user_data user;
    std::stringstream ss(user_message);
    std::string user_args;
    int count = 0;
    int flag = 0;
    while (ss >> user_args)
    {
        count++;
        if (count == 1)
            user.username = user_args;
        if (count == 4)
        {
            if (this->already_registered_check(client_sock) == -1)
            {
                flag = 1;
                break;
            }
            if (user_args[0] != ':')
                user.realname = user_args;
            else if (user_args[0] == ':')
            {
                size_t position = user_message.find(user_args);
                user.realname = user_message.substr(position + 1, user_message.length());
            }
            return (user);
        }
    }
    if (count < 4)
    {
        this->senderror(461, this->nicknames[client_sock], client_sock, " :Not enough parameters !\r\n");
        flag = 1;
    }
    if (flag == 1)
    {
        user.username = "";
        user.realname = "";
    }
    return (user);
}

int Server::user_check(std::string command, std::string input, int client_sock)
{
    this->cli = GetClient(client_sock);
    std::string user_message = input.substr(this->convert_to_upC(input).find(command) + command.length() + 1, input.length());
    user_data user = this->is_UserRealname_valid(user_message, client_sock);
    if (user.username.empty() && user.realname.empty())
        return 0;
    this->update_bool_throughout_multi_auth(command, client_sock);
    if (this->is_pass_set == true && this->which_client == client_sock)
    {
        cli->SetUsername(user.username);
        this->users[client_sock] = user;
        this->is_user_set = true;
        return (0);
    }
    return (-1);
}