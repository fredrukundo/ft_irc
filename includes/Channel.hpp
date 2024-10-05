/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 12:21:30 by frukundo          #+#    #+#             */
/*   Updated: 2024/10/03 04:52:33 by hlabouit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Server.hpp"

class Client;
class Channel
{
private:

	int invit_only;
	int topic;
	int key;
	int limit;
	bool topic_restriction;
	std::string name;
	std::string time_creation;
	std::string password;
	std::string created_at;
	std::string topic_name;
	std::vector<Client> clients;
	std::vector<Client> admins;
	std::vector<std::pair<char, bool> > modes;
public:
	Channel();
	~Channel();
	Channel(Channel const &src);
	Channel &operator=(Channel const &src);

	void SetInvitOnly(int invit_only);
	void SetTopic(int topic);
	void SetKey(int key);
	void SetLimit(int limit);
	void SetTopicName(std::string topic_name);
	void SetPassword(std::string password);
	void SetName(std::string name);
	void SetTime(std::string time);
	void set_topicRestriction(bool value);
	void setModeAtindex(size_t index, bool mode);
	void set_createiontime();

	int GetInvitOnly();
	int GetTopic();
	int GetKey();
	int GetLimit();
	int GetClientsNumber();
	bool Gettopic_restriction() const;
	bool getModeAtindex(size_t index);
	bool clientInChannel(std::string &nick);
	std::string GetTopicName();
	std::string GetPassword();
	std::string GetName();
	std::string GetTime();
	std::string get_creationtime();
	std::string getModes();
	std::string clientChannel_list();
	Client *get_client(int fd);
	Client *get_admin(int fd);
	Client *GetClientInChannel(std::string name);

	void add_client(Client newClient);
	void add_admin(Client newClient);
	void remove_client(int fd);
	void remove_admin(int fd);
	bool change_clientToAdmin(std::string& nick);
	bool change_adminToClient(std::string& nick);
	
	void sendTo_all(std::string rpl1);
	void sendTo_all(std::string rpl1, int fd);
	void update_nickname(std::string old_nickname, std::string new_nickname);
	void promoteClientToAdminIfNone();
};

#endif
