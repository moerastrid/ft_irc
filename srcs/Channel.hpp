#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
using std::vector;

#include "Client.hpp"

class Client; // Forward declaration;

class Channel {
private:
	string topic;
	vector<Client> joined;
	vector<Client> operators;
public:
	Channel();
	~Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	// Channel(/*args go here*/);
};

#endif /* end of include guard: CHANNEL_HPP */


