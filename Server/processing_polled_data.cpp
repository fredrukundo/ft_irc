/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processing_polled_data.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 14:19:29 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cctype>

std::string Server::retrieve_command(std::string &input, int client_sock)
{
    std::string upC_input = this->convert_to_upC(input);
    unsigned long i = 0;
    while (std::isspace(upC_input[i]))
        i++;
    std::stringstream ss(upC_input);
    std::string command = "";
    std::string nmp = "";
    ss >> command;
    if (command == "PASS" || command == "NICK" || command == "USER" || command == "QUIT" || command == "JOIN" || command == "INVITE" || command == "KICK"
        || command == "MODE" || command == "PART" || command == "PRIVMSG" || command == "PONG" || command == "TOPIC" || command == "LIST")
    {
        if (!(ss >> nmp))
            if (ss.eof())
            {
                if (command == "NICK")
                    this->senderror(431, this->nicknames[client_sock], client_sock, " :No nickname given !\r\n");
                else
                    this->senderror(461, this->nicknames[client_sock], client_sock, " :Not enough parameters !\r\n");
                return ("");
            }
        command = upC_input.substr(i, command.length());
        return (command);
    }
    else
    {
        this->senderror(421, this->nicknames[client_sock], client_sock, " :Unknown command !\r\n");
        return ("");
    }
    return ("");
}

int Server::process_clients_input(std::string &input, int client_sock)
{
    input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
    if (input.empty())
        return (0);
    std::string command = this->retrieve_command(input, client_sock);
    if (command == "")
        return (0);
    this->process_authentication(command, input, client_sock);
    this->process_each_command(command, input, client_sock);
    return (0);
}

ssize_t Server::getline_from_client_sock(int client_sock, std::string &input)
{
    input.clear();
    char buffer[1024];
    memset(buffer, 0 , sizeof(buffer));
    ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0)
    {
        if (bytes_received < 0)
            std::cerr << "recv() has failed to receive data from client" << std::endl;
        return bytes_received;
    }
    std::string temp_message;
    this->cli = GetClient(client_sock);

    temp_message = this->cli->getAm();
    buffer[bytes_received] = '\0';
    for (ssize_t i = 0; i < bytes_received; ++i)
    {
        if (buffer[i] != '\0')
            temp_message += buffer[i];
    }
    size_t pos = 0;
    while ((pos = temp_message.find("\r\n")) != std::string::npos)
    {
        input = temp_message.substr(0, pos);
        process_clients_input(input, client_sock);
        temp_message = temp_message.substr(pos + 2);
    }
    while ((pos = temp_message.find("\n")) != std::string::npos)
    {
        input = temp_message.substr(0, pos);
        process_clients_input(input, client_sock);
        temp_message = temp_message.substr(pos + 1);
    }
    this->cli->setAm(temp_message);
    return bytes_received;
}

void Server::process_each_command(std::string command, std::string input, int client_sock)
{
    if (command == "JOIN" || command == "PRIVMSG" || command == "TOPIC" || command == "INVITE" || command == "KICK" || command == "PART" || command == "QUIT" || command == "MODE" || command == "PONG")
    {
        if (this->not_registered_check(client_sock) == -1)
            return;
        if (command == "JOIN")
        {
            this->JOIN(input, client_sock);
        }
        else if (command == "PRIVMSG")
        {
            this->PRIVMSG(input, client_sock);
        }
        else if (command == "TOPIC")
        {
            this->Topic(input, client_sock);
        }
        else if (command == "INVITE")
        {
            this->Invite(input, client_sock);
        }
        else if (command == "KICK")
        {
            this->KICK(input, client_sock);
        }
        else if (command == "PART")
        {
            this->PART(input, client_sock);
        }
        else if (command == "QUIT")
        {
            this->QUIT(input, client_sock);
        }
        else if (command == "MODE")
        {
            this->mode_command(input, client_sock);
        }
        else if (command == "PONG")
        {
            return;
        }
    }
    return;
}
