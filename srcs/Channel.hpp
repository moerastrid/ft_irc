#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
using std::vector;

#include <algorithm>

#include "Client.hpp"

class Client; // Forward declaration;

class Channel {
private:
	vector<Client>	clients;
	string			topic;
	string			name;
	string			password;
	bool			inviteOnly;

public:
	Channel();
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	Channel(string name, string password);


	Client& getJoinedUser(string nickname);

	const string& getTopic() const;
	const string& getName() const;
	const string& getPassword() const;
	const vector<Client>& getClients() const;
	void setTopic(string& topic);

	int removeClient(Client client);
	void addClient(Client client);


	// Channel(/*args go here*/);
};

std::ostream& operator<<(std::ostream& os, const Channel& channel);
bool operator==(const Channel& lhs, const Channel& rhs);

#endif /* end of include guard: CHANNEL_HPP */


