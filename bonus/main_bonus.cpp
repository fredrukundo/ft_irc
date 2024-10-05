/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 20:36:04 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 13:01:27 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"Bot.hpp"

void check_bonus_arguments(int ac, char **av)
{
    if (ac != 6)
    {
        std::cout<< "bad arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    double port;
    std::stringstream ss(av[5]);
    std::string password = av[4];
    std::string client_nick = av[2];
    if (password.empty())
    {
        std::cout<< "invalid password !" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (client_nick.empty())
    {
        std::cout<< "empty arguments!" << std::endl;
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
        check_bonus_arguments(ac, av);
        Bot Bot(av[1], av[2], av[3], av[4], atoi(av[5]));
        Bot.run_Bot();
    }
    catch (const std::exception &excp)
    {
        std::cerr<< excp.what() << std::endl;
    }
}