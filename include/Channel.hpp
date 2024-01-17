#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
using std::vector;

#include <ostream>
using std::ostream;

#include <algorithm>
using std::find;

#include <utility>
using std::pair;

#include <string>
using std::to_string;

#include <deque>
using std::deque;

#include "Client.hpp"

class Client; // Forward declaration;

class Channel {
private:
	string				name;
	string				password;
	string				topic;
	vector<Client *>	members;
	vector<Client *>	operators; // merge with clients?
	vector<Client *>	invited;

	bool				inviteOnly;
	bool				operatorOnly = true;
	size_t				userLimit;

public:
	static Channel		nullchan;

	Channel();
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	Channel(string name, string password);

	//Client&			getJoinedUser(string nickname);

	const string&			getName() const;
	const string&			getPassword() const;
	const string&			getTopic() const;
	vector<Client *>&		getMembers();
	const vector<Client *>&	getMembersConst() const;
	const vector<Client *>&	getOperators() const;
	size_t					getUserLimit() const;
	pair<string,string>		getModes() const;

	bool	empty() const;
	bool	isInviteOnly() const;
	bool	hasTopicRestricted() const;
	bool	hasMode(char mode) const;
	bool	hasOperator(const Client& client) const;
	bool	hasMember(const Client& client) const;

	void	setPassword(string password);
	void	setTopic(string topic);
	void	toggleInviteOnly();
	void	setUserLimit(size_t limit);

	void	makeTopicOperatorOnly();
	void	makeInviteOnly();
	void	addMode(char mode, string password, size_t userlimit);
	void	addOperator(Client& client);
	void	addMember(Client& client);

	void	takeTopicOperatorOnly();
	void	takeInviteOnly();
	void	removeMode(char mode);
	void	removeOperator(const Client& client);
	int		removeMember(const Client& client);

	void	sendMessageToChannelMembers(const Client& sender, const string& message, bool colon);
};

std::ostream& operator<<(std::ostream& os, const Channel& channel);
bool operator==(const Channel& lhs, const Channel& rhs);

#endif /* end of include guard: CHANNEL_HPP */


