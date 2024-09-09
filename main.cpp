/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/08/04 03:50:42 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include"Server.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout<< "bad arguments" << std::endl;
        exit(EXIT_FAILURE);   
    }
    try
    {
        Server Server(atoi(av[1]), av[2]);
    }
    catch (const std::exception &excp)
    {
        std::cout<< excp.what() << std::endl;
    }
}
