#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
using std::vector;

#include <algorithm>

#include "Client.hpp"

class Client; // Forward declaration;

class Channel {
private:
	string			name;
	string			password;
	string			topic;
	bool			inviteOnly;
	vector<Client>	clients;
	size_t			userLimit;

public:
	Channel();
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	Channel(string name, string password);


	Client& getJoinedUser(string nickname);

	const string& getName() const;
	const string& getPassword() const;
	const string& getTopic() const;
	bool getInviteStatus() const;
	const vector<Client>& getClients() const;
	size_t getUserLimit() const;

	void setTopic(string& topic);
	void makeInviteOnly();
	void takeInviteOnly();
	void setUserLimit(size_t limit);

	int removeClient(Client client);
	void addClient(Client client);
};

std::ostream& operator<<(std::ostream& os, const Channel& channel);
bool operator==(const Channel& lhs, const Channel& rhs);

#endif /* end of include guard: CHANNEL_HPP */


