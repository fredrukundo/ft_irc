/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/09/09 05:42:42 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

#include <iostream>
#include <sstream>
#include <libc.h>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/types.h>

#include "includes/Client.hpp"
#include "includes/Channel.hpp"
#include "includes/replies.hpp"
#include "NR"

const std::string ERR_NEEDMOREPARAMS = "461";
const std::string ERR_ALREADYREGISTERED = "462";
const std::string ERR_PASSWDMISMATCH = "464";
const std::string ERR_NONICKNAMEGIVEN = "431";
const std::string ERR_ERRONEUSNICKNAME = "432";
const std::string ERR_NICKNAMEINUSE = "433";
const std::string ERR_UNKNOWNCOMMAND = "421";
const std::string ERR_NOTREGISTERED = "451";

const std::string RPL_WELCOME = "001";
const std::string RPL_YOURHOST = "002";
const std::string RPL_CREATED = "003";
const std::string RPL_MYINFO = "004";

class Client;
class Channel;


class Server
{
private:
    static bool Signal;
    
    int port;
    std::string password;
    
    std::map<int, std::string> nicknames;

    std::vector<Client> clients;
    std::vector<Channel> channels;
    int server_fdsocket;
    std::vector<struct pollfd> fds;

    struct sockaddr_in add;
	struct sockaddr_in cliadd;
	struct pollfd new_cli;
   

public:
    Server();
    ~Server();
    Server(Server const &src);
    Server &operator=(Server const &src);

    //---------------//Close and Signal Methods
	static void SignalHandler(int signum);
	void close_fds();
	//---------------//Server Methods
	void init(int port, std::string pass);
	void accept_new_client();
	void set_sever_socket();

    //---------------//Getters
    int GetFd();
    int GetPort();
    std::string GetPassword();
    Client *GetClient(int fd);
    Client *GetClientNick(std::string nickname);
    Channel *GetChannel(std::string name);
    //---------------//Setters
    void SetFd(int server_fdsocket);
    void SetPort(int port);
    void SetPassword(std::string password);
    void AddClient(Client newClient);
    void AddChannel(Channel newChannel);
    void AddFds(pollfd newFd);

    //---------------//Remove Methods
    void RemoveClient(int fd);
    void RemoveChannel(std::string name);
    void RemoveFds(int fd);
    void RmChannels(int fd);

    void set_username(std::string& username, int fd);
	void set_nickname(std::string cmd, int fd);
    //---------------//Authentification Methods
	
	bool nickNameInUse(std::string& nickname);
	bool is_validNickname(std::string& nickname);
	void client_authen(int fd, std::string pass);
    
    //---------------------------//JOIN CMD
    void JOIN(std::string cmd, int fd);
    int SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
    void ExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd);
    void NotExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int fd);
    int SearchForClients(std::string nickname);

    //---------------//Send Methods
    void senderror(int code, std::string clientname, int fd, std::string msg);
    void senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg);
    void _sendResponse(std::string response, int fd);

    //---------------//Parsing Methods
    std::vector<std::string> split_recivedBuffer(std::string str);
    std::vector<std::string> split_cmd(std::string &str);
    void parse_exec_cmd(std::string &cmd, int fd);
    bool notregistered(int fd);

    void reciveNewData(int fd);

    //---------------------------//PRIVMSG CMD
	void	PRIVMSG(std::string cmd, int fd);
	void	CheckForChannels_Clients(std::vector<std::string> &tmp, int fd);
};

#endif
