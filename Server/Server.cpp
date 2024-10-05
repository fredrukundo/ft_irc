/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 18:06:06 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void Server::signal_handler(int signal)
{
    (void)signal;
    std::cout << RED << "SERVER IS OFF" << WHI << std::endl;
    exit(EXIT_FAILURE);
}

void Server::signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
}

Server::Server(int which_port, std::string which_password)
{
    this->port = which_port;
    this->password = which_password;
    this->which_client = -1;
    this->is_pass_set = false;
    this->is_nick_set = false;
    this->is_user_set = false;
}

void Server::run_Server()
{
    this->signals();
    Client cli;
    
    sockaddr_in server_address;
    srv_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_socket_fd == -1)
    {
        std::cerr << "socket() has failed to create a socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(srv_socket_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "fcntl() has failed the set the socket in non-blocking mode" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(this->port); //htons() is a function used to convert the port number from host byte order to network byte order
    server_address.sin_addr.s_addr = INADDR_ANY;

    int option = 1;
    if (setsockopt(srv_socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)//This option allows a socket to bind to a port that is in the TIME_WAIT state, allowing address reuse.
    {
        std::cerr << "setsockopt() has failed to set options for the socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (bind(srv_socket_fd, (sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "bind() has failed to associate the socket with the specified address and port" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(srv_socket_fd, SOMAXCONN) == -1)
    {
        std::cerr << "listen() has failed to prepare the socket to accept incoming connections" << std::endl;
        exit(EXIT_FAILURE);
    }

    pollfd server_pollfd = {srv_socket_fd, POLLIN, 0};
    this->multi_connections.push_back(server_pollfd);

    std::cout << GRE << "SERVER IS ON" << WHI << std::endl;
    while (true)
    {
        //poll_fds.data() returns a pointer to the first element in the poll_fds vector
        int poll_count = poll(&this->multi_connections[0], this->multi_connections.size(), -1);
        if (poll_count == -1)
        {
            std::cerr << "poll() has failed to poll occuring events" << std::endl;
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < this->multi_connections.size(); ++i)
        {
            if (this->multi_connections[i].revents & POLLIN)
            {
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
                    if (fcntl(client_socket_fd, F_SETFL, O_NONBLOCK) == -1)
                    {
                        std::cerr << "fcntl() has failed the set the socket in non-blocking mode" << std::endl;
                        exit(EXIT_FAILURE);
                    }

                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_address.sin_addr, ip_str, INET_ADDRSTRLEN);
                    cli.SetFd(client_socket_fd);
                    cli.setIpAdd(ip_str);
                    cli.get_client_host(cli.getIpAdd());
                    clients.push_back(cli);

                    pollfd client_pollfd = {client_socket_fd, POLLIN | POLLHUP | POLLNVAL | POLLERR, 0};
                    this->multi_connections.push_back(client_pollfd);
                    std::cout << GRE << "client of socket " << YEL <<  "<" << client_socket_fd << ">" << GRE << " : CONNECTED" << WHI<< std::endl;
                }
                else
                {
                    int client_socket_fd = this->multi_connections[i].fd;
                    std::string input = "";
                    ssize_t n = this->getline_from_client_sock(client_socket_fd, input);
                    if (n <= 0)
                    {
                        std::cout << RED << "client of socket " << YEL <<  "<" << client_socket_fd << ">" << RED << " : DISCONNECTED" << WHI<< std::endl;
                        this->erase_client_data(client_socket_fd, i);
                        --i;
                    }
                }
            }
            else if (this->multi_connections[i].revents & (POLLHUP | POLLNVAL | POLLERR))
            {
                std::cout << RED << "client of socket " << YEL <<  "<" << multi_connections[i].fd << ">" << RED << " : DISCONNECTEDPULHUP" << WHI<< std::endl;
                this->erase_client_data(multi_connections[i].fd, i);
                --i;   
            }
        }
    }
}
