/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 05:22:34 by hlabouit          #+#    #+#             */
/*   Updated: 2024/10/03 18:37:27 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <cstring>
#include <libc.h>
#include <sys/attr.h>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/types.h>

#include "Channel.hpp"
#include "Client.hpp"
#include "replies.hpp"

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

class Client;
class Channel;

typedef struct user_data
{
	std::string	username;
	std::string	realname;
} user_data;

class Server {
    private:
        int port;
        std::string password;
        int srv_socket_fd;
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
        void run_Server();

        ssize_t getline_from_client_sock(int client_sock, std::string &input);
        int process_clients_input(std::string &input, int client_sock);
        std::string convert_to_upC(std::string &input);
        bool check_for_nl(char *buff, size_t bytes_received);
        std::string retrieve_command(std::string &upC_input, int client_sock);
        int pass_check(std::string command, std::string input, int client_sock);
        int nick_check(std::string command, std::string input, int client_sock);
        int is_nickname_valid(std::string &nickname, int client_sock);
        int user_check(std::string command, std::string input, int client_sock);
        user_data is_UserRealname_valid(std::string &user_message, int client_sock);
        void update_bool_throughout_multi_auth(std::string &command, int client_sock);
        void process_authentication(std::string command, std::string input, int client_sock);
        void client_registration_greeting(int client_sock);
        void process_each_command(std::string command, std::string input, int client_sock);
        void validate_client_credentials(int client_sock);
        int already_registered_check(int client_sock);
        int not_registered_check(int client_sock);
        int erase_client_data(int client_sock, int itteration);
        static void signal_handler(int signal);
        void signals();
        std::string GetPassword();
        Client *GetClient(int fd);
        Client *GetClientNick(std::string nickname);
        Channel *GetChannel(std::string name);
        void SetPassword(std::string password);
        void JOIN(std::string cmd, int fd);
        int ParseJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
        void ExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd);
        void NotExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int fd);
        int SearchForClients(std::string nickname);
        void senderror(int code, std::string clientname, int fd, std::string msg);
        void senderror(int code, std::string clientname, std::string channelname, int fd, std::string msg);
        void _sendResponse(std::string response, int fd);
    	void	PRIVMSG(std::string cmd, int fd);
    	void	CheckForChannels_Clients(std::vector<std::string> &tmp, int fd);
    	std::string TimeCreation();
    	void Topic(std::string &cmd, int &fd);
    	std::string gettopic(std::string& input);
    	int getpos(std::string &cmd);
        std::vector<std::string> split_cmd(std::string &str);
        void Invite(std::string &cmd, int &fd);
    	void	KICK(std::string cmd, int fd);
        void DeleteChannel(const std::string &channelName);
    	std::string SplitCmdKick(std::string cmd, std::vector<std::string> &channels, std::vector<std::string> &users, int fd);
    	void	PART(std::string cmd, int fd);
    	int		SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);
    	void RemoveClient(int fd);
    	void RemoveChannel(std::string name);
    	void RmChannels(int fd);
    	void	QUIT(std::string cmd, int fd);
    	void 		mode_command(std::string& cmd, int fd);
    	std::string invite_only(Channel *channel, char opera, std::string chain);
    	std::string topic_restriction(Channel *channel ,char opera, std::string chain);
    	std::string password_mode(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
    	std::string operator_privilege(std::vector<std::string> splited, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments);
    	std::string channel_limit(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
    	bool		isvalid_limit(std::string& limit);
    	std::string mode_toAppend(std::string chain, char opera, char mode);
    	std::vector<std::string> splitParams(std::string params);
    	void getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params);
};

#endif