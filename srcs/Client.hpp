#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
using std::string;

#include "Channel.hpp"

class Channel; // Forward declaration

class Client {
private:


public:
	int				fd;
	string			nickname;
	string			username;
	string			hostname;
	string			servername;
	string			realname;
	vector<Channel>	joined_channels;

	Client();
	~Client();
	Client(const Client& other);
	Client& operator=(const Client& other);
};

std::ostream& operator<<(std::ostream& os, const Client& client);
#endif /* end of include guard: CLIENT_HPP */
