#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include "Channel.hpp"

using std::string;

class Client {
private:


public:
	int         fd;
	string      username;
	string      nickname;
	// Channel*    joined_channels;
	Client();
	~Client();
	Client(int fd);
	Client(const Client& other);
	Client& operator=(const Client& other);

	// Client(/*args go here*/);
};

#endif /* end of include guard: CLIENT_HPP */
