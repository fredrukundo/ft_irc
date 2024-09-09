/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/09/09 07:16:26 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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

#include "includes/Channel.hpp"
#include "includes/Client.hpp"
#include "includes/replies.hpp"
#include "NR.hpp"

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

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


typedef struct poll_stc
{
    int fd;
    short events;
    short revents;
} poll_stc;

typedef struct user_data
{
    std::string username;
    std::string realname;
} user_data;

class Server
{
private:
    int port;
    std::string password;
    int server_fd;
    
    bool is_pass_set;
    bool is_nick_set;
    bool is_user_set;
    int which_client;
    std::vector<int> clients_who_set_pass;
    std::vector<pollfd> multi_connections;
    std::vector<int> authenticated_clients;
    std::map<int, user_data> users;
    std::map<int, std::string> nicknames;
    
    Client *cli;
    std::vector<Client> clients;
    std::vector<Channel> channels;

public:
    Server(int which_port, std::string which_password);

    int process_clients_input( std::string &input, int client_sock);
    int pass_check(std::string command, std::string input, unsigned long i, int client_sock);
    int nick_check(std::string command, std::string input, unsigned long i, int client_sock);
    int user_check(std::string command, std::string input, unsigned long i, int client_sock);
    int alreadyRegistered_check(int client_sock);
    int notRegistered_check(int client_sock);
    int erase_client_data(int client_sock, int itteration);

    //---------------//Getters
    std::string GetPassword();
    Client *GetClient(int fd);
    Client *GetClientNick(std::string nickname);
    Channel *GetChannel(std::string name);
    //---------------//Setters
    void SetPassword(std::string password);

    //---------------------------//JOIN CMD
    void JOIN(std::string cmd, int fd);
    int ParseJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
    void ExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd);
    void NotExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int fd);
    int SearchForClients(std::string nickname);

      //---------------//Send Methods
    void senderror(int code, std::string clientname, int fd, std::string msg);
    void senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg);
    void _sendResponse(std::string response, int fd);

    //---------------------------//PRIVMSG CMD
	void	PRIVMSG(std::string cmd, int fd);
	void	CheckForChannels_Clients(std::vector<std::string> &tmp, int fd);

    //---------------------------//TOPIC CMD
	std::string TimeCreation();
	void Topic(std::string &cmd, int &fd);
	std::string gettopic(std::string& input);
	int getpos(std::string &cmd);
	//void Invite(std::string &cmd, int &fd);
    
    std::vector<std::string> split_cmd(std::string &str);
    
    //---------------------------//INVITE CMD
    void Invite(std::string &cmd, int &fd);

    //---------------------------//CKIK CMD
	void	KICK(std::string cmd, int fd);
	std::string SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd);
    	//---------------------------//PART CMD
	void	PART(std::string cmd, int fd);
	int		SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);

    //---------------//Remove Methods
	void RemoveClient(int fd);
	void RemoveChannel(std::string name);
	//void RemoveFds(int fd);
	void RmChannels(int fd);

    //---------------------------//QUITE CMD
	void	QUIT(std::string cmd, int fd);

    	//---------------------------//MODE CMD
	void 		mode_command(std::string& cmd, int fd);
	std::string invite_only(Channel *channel, char opera, std::string chain);
	std::string topic_restriction(Channel *channel ,char opera, std::string chain);
	std::string password_mode(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	std::string operator_privilege(std::vector<std::string> splited, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments);
	std::string channel_limit(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	bool		isvalid_limit(std::string& limit);
	std::string appendModeChange(std::string currentModes, char modeOPerator, char modeFlag);
	std::vector<std::string> splitParams(std::string params);
	void getCmdArgs(std::string input,std::string& CmdName, std::string& ChToSet ,std::string &params);

};

#endif