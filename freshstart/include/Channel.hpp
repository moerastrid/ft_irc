#pragma once

#include <vector>
using std::vector;

#include <algorithm>

#include <utility>
using std::pair;

#include "Client.hpp"

class Client; // Forward declaration;

class Channel {
private:
	string			name;
	string			password;
	string			topic;
	vector<Client>	clients;
	vector<Client>	operators;
	vector<Client>	invited;

	bool			inviteOnly;
	bool			operatorOnly;
	size_t			userLimit;
	int				founderFD; // Stores FD;
	

public:
	Channel();
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	Channel(string name, string password, int founderFD);

	Client& getJoinedUser(string nickname);

	const string& getName() const;
	const string& getPassword() const;
	const string& getTopic() const;
	bool isInviteOnly() const;
	bool hasTopicRestricted() const;
	const vector<Client>& getClients() const;
	size_t getUserLimit() const;
	string getModes() const;
	bool hasMode(char mode) const;
	bool hasOperator(const Client& client) const;
	bool hasFounder(const Client& client) const;
	bool hasUser(const Client& client) const;
	int getFounderFD() const;

	void setTopic(string& topic);
	void makeInviteOnly();
	void takeInviteOnly();
	void makeTopicOperatorOnly();
	void takeTopicOperatorOnly();
	void setUserLimit(size_t limit);
	void addMode(char mode, string password, size_t userlimit);
	void removeMode(char mode);

	void addOperator(const Client& client);
	void removeOperator(const Client& client);
	void addClient(const Client& client);
	int removeClient(const Client& client);

};

std::ostream& operator<<(std::ostream& os, const Channel& channel);
bool operator==(const Channel& lhs, const Channel& rhs);
