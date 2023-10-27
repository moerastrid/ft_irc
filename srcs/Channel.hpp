#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
using std::vector;

#include "Client.hpp"

class Client; // Forward declaration;

class Channel {
private:
public:
	string			topic;
	string			name;
	string			password;
	vector<Client>	joined;
	Channel();
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);

	Client& getJoinedUser(string nickname);

	// Channel(/*args go here*/);
};

#endif /* end of include guard: CHANNEL_HPP */


