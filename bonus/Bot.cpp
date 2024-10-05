/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 20:36:04 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 13:20:14 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"Bot.hpp"

Bot::Bot(const std::string &which_bN, const std::string &which_cN, const std::string &which_sIP, std::string which_sPW, int which_sPT) : bot_nick(which_bN), client_nick(which_cN), server_IP(which_sIP), server_PW(which_sPW)
{
    this->server_port = which_sPT;
}

void Bot::send_to_sock(int bot_sock, std::string &message)
{
    if (send(bot_sock, message.c_str(), message.size(), 0) == -1)
        std::cerr << "send() has faild" << std::endl;
    message.clear();
}

void Bot::authenticate_Bot(int bot_sock)
{
    std::string registration_message;
    registration_message = "PASS " + this->server_PW + "\r\n";
    this->send_to_sock(bot_sock, registration_message);
    usleep(500);
    registration_message = "NICK " + bot_nick + "\r\n"; 
    this->send_to_sock(bot_sock, registration_message);
    usleep(500); 
    registration_message = "USER " + bot_nick + " 0 * :" + bot_nick + "\r\n";
    this->send_to_sock(bot_sock, registration_message);
    usleep(500);
}

int Bot::connect_Bot_to_server()
{
    sockaddr_in server_address;
    int bot_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (bot_sock == -1)
    {
        std::cerr << "socket() has failed to create a socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(this->server_port);
    if (inet_pton(AF_INET, this->server_IP.c_str(), &server_address.sin_addr) <= 0)
    {
        std::cerr << "invalid ip address!" << std::endl;
        close(bot_sock);
        return (-2);
    }
    if (connect(bot_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "connect() has failed to connect bot to the server!" << std::endl;
        close(bot_sock);
        return(-1);
    }
    std::cout<< GRE << "BOT CONNECTED" << WHI << std::endl;
    return(bot_sock);

}

void Bot::run_Bot()
{
    int bot_sock = this->connect_Bot_to_server();
    if (bot_sock < 0)
        return;
    std::cout<< std::endl << "•••••••••••••••••••••••••••• Welcome To OnePiece Arcs Bot ••••••••••••••••••••••••••••" << std::endl;
	std::cout<< "➜ To Enable Bot Features: PRIVMSG BOT :<Arc> ⤸" << std::endl;
	std::cout<< ":Baratie" << std::endl;
	std::cout<< ":ArlongPark" << std::endl;
	std::cout<< ":Alabasta" << std::endl;
	std::cout<< ":Skypiea" << std::endl;
	std::cout<< ":Water7" << std::endl;
	std::cout<< std::endl;
    this->authenticate_Bot(bot_sock);
    std::string message = "";
    std::string triger = "";
    char buffer[1024];
    this->bot_signals();
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(bot_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            if (bytes_received < 0)
                std::cerr << "recv() has failed to receive data from client" << std::endl;
            break;
        }
        message = buffer;
        std::cout << message << std::endl;
        std::stringstream ss(message);
        while (ss >> triger){};
        message = this->check_arc(triger);
        if (!message.empty())
            this->send_to_sock(bot_sock, message);
    }
}

std::string Bot::check_arc(std::string &triger)
{
    this->OnePiece_arcs[":Baratie"] = 
        "The crew arrives at the Baratie, a floating restaurant, where they meet Sanji, "
        "the cook who dreams of finding the All Blue. Luffy fights Don Krieg, "
        "a powerful pirate, while Zoro challenges Dracule Mihawk, the world's greatest swordsman. "
        "Sanji joins the Straw Hat crew after a touching farewell to the Baratie.";

    this->OnePiece_arcs[":ArlongPark"] = 
        "Nami's past is revealed as the Straw Hat crew heads to Arlong Park to defeat the fish-man pirate Arlong, "
        "who has been terrorizing Nami's village. Luffy and the crew stand up against Arlong, freeing Nami from her tragic past. "
        "After a fierce battle, Luffy destroys Arlong Park, and Nami officially joins the Straw Hat Pirates.";

    this->OnePiece_arcs[":Alabasta"] = 
        "The crew assists Princess Vivi in saving her kingdom of Alabasta from Baroque Works and its leader, Crocodile. "
        "Luffy battles Crocodile three times, ultimately defeating him with his sheer willpower and a clever strategy. "
        "The civil war is stopped, and Vivi chooses to stay behind and lead her people. Nico Robin, formerly part of Baroque Works, "
        "joins the crew as their archaeologist.";

    this->OnePiece_arcs[":Skypiea"] = 
        "The crew reaches Skypiea, an island in the sky, where they get caught up in a rebellion against the self-proclaimed God Enel, "
        "who possesses the power of the Goro Goro no Mi (Lightning). Luffy's immunity to lightning allows him to defeat Enel in a dramatic battle, "
        "and he rings the golden bell, signaling the end of Enel’s reign and revealing the City of Gold.";

    this->OnePiece_arcs[":Water7"] = 
        "The Straw Hats arrive in Water 7, seeking repairs for their ship, the Going Merry. However, tensions arise when the shipwrights deem the Merry unsalvageable. "
        "The crew faces internal conflict, leading to Usopp's temporary departure after a heated duel with Luffy. Meanwhile, Nico Robin is captured by the World Government's "
        "CP9 organization. The arc introduces Franky, a shipwright who eventually becomes a crucial ally.";
        
    std::map<std::string, std::string>::iterator itr = this->OnePiece_arcs.find(triger);
    std::string message = "";
    if (itr != this->OnePiece_arcs.end())
    {
        message = "PRIVMSG " + client_nick + " :" + OnePiece_arcs[triger] + "\r\n";
        return (message);
    }
    return ("");
}

void Bot::bot_signal_handler(int signal)
{
    (void)signal;
    std::cout<< RED << "BOT DISCONNECTED" << WHI << std::endl;
    exit(EXIT_FAILURE);
}

void Bot::bot_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, bot_signal_handler);
}
