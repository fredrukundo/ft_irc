/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 13:23:11 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "includes/Server.hpp"
#include <sstream>

void check_arguments(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout<< "bad arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    double port;
    std::stringstream ss(av[1]);
    std::string password = av[2];
    if (password.empty())
    {
        std::cout<< "invalid password !" << std::endl;
        exit(EXIT_FAILURE);
    }
    ss >> port;
    if (ss.fail() || !ss.eof() || port < 1024 || port > 65535)
    {
        std::cout<< "invalid port range !" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int ac, char **av)
{
    try
    {
        check_arguments(ac, av);
        Server Server(atoi(av[1]), av[2]);
        Server.run_Server();
        
    }
    catch (const std::exception &excp)
    {
        std::cerr<< excp.what() << std::endl;
    }
}
