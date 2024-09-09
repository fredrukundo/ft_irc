/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 12:19:54 by frukundo          #+#    #+#             */
/*   Updated: 2024/08/29 14:46:29 by frukundo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Server.hpp"
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
public:
	Client();
	Client(std::string nickname, std::string username, int fd);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &src);
	//---------------//Getters
	int GetFd();
	bool getRegistered();
	bool GetInviteChannel(std::string &ChName);
	std::string GetNickName();
	bool 		GetLogedIn();
	std::string GetUserName();
	std::string getIpAdd();
	std::string getHostname();
	//---------------//Setters
	void SetFd(int fd);
	void SetNickname(std::string& nickName);
	void setLogedin(bool value);
	void SetUsername(std::string& username);
	void setRegistered(bool value);
	void setIpAdd(std::string ipadd);
	//---------------//Methods
	void AddChannelInvite(std::string &chname);
	void RmChannelInvite(std::string &chname);
};

#endif