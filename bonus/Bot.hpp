/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 20:36:04 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 06:59:51 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include "../includes/Server.hpp"

class Bot {
    private:
        std::string bot_nick;
        std::string client_nick;
        std::string server_IP;
        std::string server_PW;
        int server_port;
        std::map<std::string, std::string> OnePiece_arcs;
    public:
        Bot(const std::string &which_bN, const std::string &which_cN, const std::string &which_sIP, std::string which_sPW, int which_sPT);
        void run_Bot();
        int connect_Bot_to_server();
        std::string check_arc(std::string &triger);
        void authenticate_Bot(int bot_sock);
        void send_to_sock(int bot_sock, std::string &message);
        static void bot_signal_handler(int signal);
        void bot_signals();
};

#endif