/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/09/09 07:17:05 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <string>
#include <sys/poll.h>

int Server::process_clients_input(std::string &input, int client_sock)
{

    std::string upC_input = input;
    for (unsigned long i = 0; i < input.length(); i++)
        upC_input[i] = (char)toupper(upC_input[i]);
    unsigned long i = 0;
    while (std::isspace(upC_input[i]))
        i++;
    std::string command = upC_input.substr(i, 5);

    std::string cmd_after_auth;
    std::stringstream ss(upC_input);
    ss >> cmd_after_auth;

    if (pass_check(command, input, i, client_sock) == 0)
        return (0);
    if (nick_check(command, input, i, client_sock) == 0)
        return (0);
    if (user_check(command, input, i, client_sock) == 0)
    {
        // std::vector<int>::iterator itr = std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock);
        // if (itr != this->authenticated_clients.end())
        //     return (0);
        std::cout << "IS_PASS_S    [" << this->is_pass_set << "]" << client_sock << std::endl;
        std::cout << "IS_NICK_S    [" << this->is_nick_set << "]" << client_sock << std::endl;
        std::cout << "IS_USER_S    [" << this->is_user_set << "]" << client_sock << std::endl;
        Client(nicknames[client_sock], this->users[client_sock].username, client_sock);
        if (this->is_pass_set == true && this->is_nick_set == true && this->is_user_set == true)
        {
            this->is_pass_set = false;
            this->is_nick_set = false;
            this->is_user_set = false;
            this->authenticated_clients.push_back(client_sock);
            std::cerr << "REGISTRED SUCCESSFULLY: " << client_sock << std::endl;
            std::string message = ":IRC-localhost " + std::string(RPL_WELCOME) + " " + this->nicknames[client_sock] + " :Welcome to the IRC-localhost Network, " + this->nicknames[client_sock] + "[!" + this->users[client_sock].username + "@IRC-localhost] !\r\n";
            send(client_sock, message.c_str(), message.length(), 0);
            message = ":IRC-localhost " + std::string(RPL_YOURHOST) + " " + this->nicknames[client_sock] + " :Your host is IRC-localhost, running version 1.0 !\r\n";
            send(client_sock, message.c_str(), message.length(), 0);
            message = ":IRC-localhost " + std::string(RPL_CREATED) + " " + this->nicknames[client_sock] + " :This server was created 2024-07-24 !\r\n";
            send(client_sock, message.c_str(), message.length(), 0);
            message = ":IRC-localhost " + std::string(RPL_MYINFO) + " " + this->nicknames[client_sock] + " :Host: IRC-IRC-localhost, Version: 1.0, User mode: none, Channel modes: o, t, k, i !\r\n";
            send(client_sock, message.c_str(), message.length(), 0);
            return (0);
        }
    }

    if (cmd_after_auth == "QUIT" || cmd_after_auth == "JOIN" || cmd_after_auth == "INVITE" || cmd_after_auth == "KICK" || cmd_after_auth == "MODE" || cmd_after_auth == "PART" || cmd_after_auth == "PRIVMSG" || cmd_after_auth == "PONG" || cmd_after_auth == "TOPIC" || cmd_after_auth == "LIST")
    {
        if (this->notRegistered_check(client_sock) == -1)
        {
            std::cout << "not registered: " << "\n";
            return (-1);
        }
        else
        {
            if (cmd_after_auth == "JOIN")
            {
                JOIN(input, client_sock);
            }
            else if (cmd_after_auth == "PRIVMSG")
            {
                PRIVMSG(input, client_sock);
            }
            else if (cmd_after_auth == "TOPIC")
            {
                Topic(input, client_sock);
            }
            else if (cmd_after_auth == "INVITE")
            {
                Invite(input, client_sock);
            }
            else if (cmd_after_auth == "KICK")
            {
                KICK(input, client_sock);
            }
            else if (cmd_after_auth == "PART")
            {
                PART(input, client_sock);
            }
            else if (cmd_after_auth == "QUIT")
            {
                QUIT(input, client_sock);
            }
            else if (cmd_after_auth == "MODE")
            {
                // mode_command(input, client_sock);
            }
            else if (cmd_after_auth == "PONG")
            {
                return 0;
            }
        }
    }
    else
    {
        std::string reply = ":IRC-localhost " + std::string(ERR_UNKNOWNCOMMAND) + " " + this->nicknames[client_sock] + " :Unknown command\r\n";
        send(client_sock, reply.c_str(), reply.length(), 0);
        return (0);
    }
    return (0);
}
ssize_t getline_form_Csocket(int client_sock, std::string &output)
{
    output.clear();
    char buffer;
    ssize_t n;
    while ((n = recv(client_sock, &buffer, 1, 0)) > 0)
    {
        if (buffer == '\n')
            break;
        if (buffer == '\r')
            continue;
        output += buffer;
    }
    return (n);
}

int Server::alreadyRegistered_check(int client_sock)
{
    if (!this->authenticated_clients.empty())
    {
        if (std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock) != authenticated_clients.end())
        {
            std::string reply = ":IRC=localhost " + std::string(ERR_ALREADYREGISTERED) + " :you may not register !\r\n";
            send(client_sock, reply.c_str(), reply.length(), 0);
            return (-1);
        }
    }
    return (0);
}

int Server::notRegistered_check(int client_sock)
{
    std::vector<int>::iterator itr = std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock);
    if (itr == this->authenticated_clients.end())
    {
        std::string reply = ":IRC=localhost " + std::string(ERR_NOTREGISTERED) + " :You have not registered !\r\n";
        send(client_sock, reply.c_str(), reply.length(), 0);
        return (-1);
    }
    return (0);
}

int Server::pass_check(std::string command, std::string input, unsigned long i, int client_sock)
{
    if (command.find("PASS") != std::string::npos && (command[4] == '\t' || command[4] == '\r' || command[4] == ' '))
    {
        this->cli = GetClient(client_sock);

        std::string sub_input = input.substr(i + 5, input.length());
        i = 0;
        while (std::isspace(sub_input[i]))
            i++;
        if (i == sub_input.length())
        {
            // :IRC-localhost 461  :Not enough parameters !
            std::string reply = "IRC-localhost " + std::string(ERR_NEEDMOREPARAMS) + " :Not enough parameters !\r\n";
            send(client_sock, reply.c_str(), reply.length(), 0);
            return (0);
        }
        else if (sub_input == this->password)
        {
            if (this->alreadyRegistered_check(client_sock) == -1)
                return (0);
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
            if (this->alreadyRegistered_check(client_sock) == -1)
                return (0);
            std::string reply = "IRC-localhost " + std::string(ERR_PASSWDMISMATCH) + " :Password incorrect !\r\n";
            send(client_sock, reply.c_str(), reply.length(), 0);
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
    }
    else if (command == "PASS")
    {
        std::string reply = "IRC-localhost " + std::string(ERR_NEEDMOREPARAMS) + " :Not enough parameters !\r\n";
        send(client_sock, reply.c_str(), reply.length(), 0);
        return (0);
    }
    return (-1);
}

int Server::nick_check(std::string command, std::string input, unsigned long i, int client_sock)
{

    if (command.find("NICK") != std::string::npos && (command[4] == '\t' || command[4] == '\r' || command[4] == ' '))
    {
        this->cli = GetClient(client_sock);
        std::string sub_input = input.substr(i + 5, input.length());
        i = 0;
        while (std::isspace(sub_input[i]))
            i++;
        if (i == sub_input.length())
        {
            std::string reply = "IRC-localhost " + std::string(ERR_NONICKNAMEGIVEN) + " :No nickname given !\r\n";
            send(client_sock, reply.c_str(), reply.length(), 0);
            return (0);
        }
        else
        {
            std::string nickname = sub_input.substr(i, input.length());
            if (!this->nicknames.empty())
            {
                std::map<int, std::string>::iterator itr = this->nicknames.begin();
                while (itr != this->nicknames.end())
                {
                    if (itr->second == nickname && itr->first != client_sock)
                    {
                        std::string reply = "IRC-localhost " + std::string(ERR_NICKNAMEINUSE) + " :Nickname is already in use !\r\n";
                        send(client_sock, reply.c_str(), reply.length(), 0);
                        return (0);
                    }
                    itr++;
                }
            }
            size_t pos = nickname.find_first_of("\t\r :#&");
            if (pos != std::string::npos)
            {
                std::string reply = "IRC-localhost " + std::string(ERR_ERRONEUSNICKNAME) + " :Erroneus nickname !\r\n";
                send(client_sock, reply.c_str(), reply.length(), 0);
                return (0);
            }

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
            if (!this->authenticated_clients.empty() && std::find(authenticated_clients.begin(), authenticated_clients.end(), client_sock) != authenticated_clients.end())
            {
                cli->SetNickname(nickname);
                this->nicknames[client_sock] = nickname;
                this->is_nick_set = false;
                std::cout << "NICK IS      [" << this->nicknames[client_sock] << "]" << client_sock << std::endl;
                return (0);
            }
            else if (this->is_pass_set == true && this->which_client == client_sock)
            {
                cli->SetNickname(nickname);
                this->nicknames[client_sock] = nickname;
                this->is_nick_set = true;
                std::cout << "NICK IS      [" << this->nicknames[client_sock] << "]" << client_sock << std::endl;
                return (0);
            }
        }
    }
    else if (command == "NICK")
    {
        std::string reply = ":IRC=localhost " + std::string(ERR_NONICKNAMEGIVEN) + " :No nickname given !\r\n";
        send(client_sock, reply.c_str(), reply.length(), 0);
        return (0);
    }
    return (-1);
}

int Server::user_check(std::string command, std::string input, unsigned long i, int client_sock)
{
    if (command.find("USER") != std::string::npos && (command[4] == '\t' || command[4] == '\r' || command[4] == ' '))
    {
        this->cli = GetClient(client_sock);

        std::string sub_input = input.substr(i + 5, input.length());
        i = 0;
        while (std::isspace(sub_input[i]))
            i++;
        if (i == sub_input.length())
        {
            std::string reply = "IRC-localhost " + std::string(ERR_NEEDMOREPARAMS) + " :Not enough parameters !\r\n";
            send(client_sock, reply.c_str(), reply.length(), 0);
            return (0);
        }
        else
        {
            std::string user_message = sub_input.substr(i, input.length());
            std::stringstream ss(user_message);
            user_data user;
            std::string user_args;
            int count = 0;
            while (ss >> user_args)
            {
                count++;
                if (count == 1)
                    user.username = user_args;
                if (count == 4)
                {
                    size_t position = user_message.find(user_args);
                    user.realname = user_message.substr(position + 1, user_message.length());
                    if (user_args[0] != ':' || user.realname.empty())
                    {
                        std::string reply = "IRC-localhost " + std::string(ERR_NEEDMOREPARAMS) + " :Not enough parameters !\r\n";
                        send(client_sock, reply.c_str(), reply.length(), 0);
                        return (0);
                    }
                    if (this->alreadyRegistered_check(client_sock) == -1)
                        return (0);
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
                    if (this->is_pass_set == true && this->which_client == client_sock)
                    {
                        cli->SetUsername(user.username);
                        this->users[client_sock] = user;
                        this->is_user_set = true;
                        std::cout << "REAL N IS    [" << this->users[client_sock].realname << "]" << client_sock << std::endl;
                        std::cout << "USER N IS    [" << this->users[client_sock].username << "]" << client_sock << std::endl;
                        return (0);
                    }
                }
            }
            if (count < 4)
            {
                std::string reply = "IRC-localhost " + std::string(ERR_NEEDMOREPARAMS) + " :Not enough parameters !\r\n";
                send(client_sock, reply.c_str(), reply.length(), 0);
                return (0);
            }
        }
    }
    else if (command == "USER")
    {
        std::string reply = "IRC-localhost " + std::string(ERR_NEEDMOREPARAMS) + " :Not enough parameters !\r\n";
        send(client_sock, reply.c_str(), reply.length(), 0);
        return (0);
    }
    return (-1);
}

Server::Server(int which_port, std::string which_password)
{
    Client cli;
    this->port = which_port;
    this->password = which_password;
    this->which_client = -1;
    this->is_pass_set = false;
    this->is_nick_set = false;
    this->is_user_set = false;

    sockaddr_in server_address;
    int srv_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_socket_fd == -1)
    {
        std::cerr << "socket() has failed to create a socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(which_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int option = 1;
    if (setsockopt(srv_socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
    {
        std::cerr << "setsockopt() has failed to set options for the socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (bind(srv_socket_fd, (sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "bind() has failed to associate the socket with the specified address and port" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(srv_socket_fd, 20) == -1)
    {
        std::cerr << "listen() has failed to prepare the socket to accept incoming connections" << std::endl;
        exit(EXIT_FAILURE);
    }

    pollfd server_pollfd = {srv_socket_fd, POLLIN | POLLHUP | POLLNVAL | POLLERR, 0};
    this->multi_connections.push_back(server_pollfd);

    // char buffer[10000] = {0};

    signal(SIGPIPE, SIG_IGN);
    while (true)
    {
        int poll_count = poll(&this->multi_connections[0], this->multi_connections.size(), -1);
        if (poll_count == 0)
        {
            std::cerr << "poll() has failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < this->multi_connections.size(); ++i)
        {
            // std::cout<< "{{" << this->multi_connections[i].events << "}}" << std::endl;
            if (this->multi_connections[i].revents & POLLIN)
            {
                // std::cout<< "i        =========== " << i << std::endl;
                // std::cout<< "SRVSOCK  =========== " << srv_socket_fd << std::endl;
                // std::cout<< "SIIIIZEE =========== " << this->multi_connections.size() << std::endl;
                // std::cout<< "====================== " << std::endl;

                if (this->multi_connections[i].fd == srv_socket_fd)
                {
                    sockaddr_in client_address;
                    socklen_t client_address_len = sizeof(client_address);
                    int client_socket_fd = accept(srv_socket_fd, (sockaddr *)&client_address, &client_address_len);
                    if (client_socket_fd == -1)
                    {
                        std::cerr << "accept() has failed to accept incoming connections from clients" << std::endl;
                        exit(EXIT_FAILURE);
                    }

                    // Get client IP address
                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_address.sin_addr, ip_str, INET_ADDRSTRLEN);
                    std::cout << "Client IP: " << ip_str << std::endl;

                    // Store the IP address in the client object
                    cli.SetFd(client_socket_fd);
                    cli.setIpAdd(ip_str);
                    clients.push_back(cli);

                    pollfd client_pollfd = {client_socket_fd, POLLIN | POLLHUP | POLLNVAL | POLLERR, 0};
                    this->multi_connections.push_back(client_pollfd);
                    std::cout << "NEW CONNECTION: " << client_socket_fd << std::endl;
                }
                else
                {
                    int client_socket_fd = this->multi_connections[i].fd;
                    std::string input = "";
                    // puts("gfvdfgdfgt");
                    int n = getline_form_Csocket(client_socket_fd, input);
                    if (n <= 0)
                    {
                        if (n == 0)
                            std::cout << client_socket_fd << ": DISCONNECTED" << std::endl;
                        else
                            std::cerr << "recv() has failed to receive data from client" << std::endl;

                        this->erase_client_data(client_socket_fd, i);
                        --i;
                    }
                    process_clients_input(input, client_socket_fd);
                    input = "";
                }
            }
            else if (this->multi_connections[i].revents & POLLHUP)
                this->erase_client_data(this->multi_connections[i].fd, i);
            else if (this->multi_connections[i].revents & POLLNVAL)
                this->erase_client_data(this->multi_connections[i].fd, i);
            else if (this->multi_connections[i].revents & POLLERR)
                this->erase_client_data(this->multi_connections[i].fd, i);
        } // on which sock the event occur, srv or clt
    }
}

int Server::erase_client_data(int client_sock, int itteration)
{
    if (itteration >= 0)
    {
        close(client_sock);
        this->multi_connections.erase(this->multi_connections.begin() + itteration);
        std::cout << "CLOSED and ERASED in POLLFDS:   " << client_sock << std::endl;
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
    return (0);
}

//---------------//Send Methods
void Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << ":localhost " << code << " " << clientname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() faild" << std::endl;
}

void Server::senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << ":localhost " << code << " " << clientname << " " << channelname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() faild" << std::endl;
}

void Server::_sendResponse(std::string response, int fd)
{
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr << "Response send() faild" << std::endl;
}
//---------------//Send Methods

//---------------//Getters

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
    std::cout << "channels number: " << this->channels.size() << "\n";
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].GetName() == name)
            return &channels[i];
    }
    return NULL;
}

//---------------//Getters
std::string Server::GetPassword() { return this->password; }
//---------------//Setters

void Server::SetPassword(std::string password) { this->password = password; }

//---------------//Setters

// ----------// Split command helper function
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

//---------------//Remove Methods
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
            std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT Quit\r\n";
            channels[i].sendTo_all(rpl);
        }
    }
}
//---------------//Remove Methods