/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 12:19:54 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 04:52:40 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Channel.hpp"

class Client
{
private:
	int fd;
	std::string ipadd;
	
	bool isOperator;
	bool registered;
	std::string nickname;
	std::string username;
	bool logedin;
	std::string buffer;
	std::vector<std::string> ChannelsInvite;
    std::string assembled_message;
public:
	Client();
	Client(std::string nickname, std::string username, int fd);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &src);
	
	int GetFd();
	bool getRegistered();
	bool GetInviteChannel(std::string &ChName);
	std::string GetNickName();
	bool 		GetLogedIn();
	std::string GetUserName();
	std::string getIpAdd();
	std::string get_client_host(std::string ip);
	std::string getAm();

	void SetFd(int fd);
	void SetNickname(std::string& nickName);
	void setLogedin(bool value);
	void SetUsername(std::string& username);
	void setRegistered(bool value);
	void setIpAdd(std::string ipadd);
	void setAm(const std::string &pm);
	
	void AddChannelInvite(std::string &chname);
	void RmChannelInvite(std::string &chname);
};

#endif