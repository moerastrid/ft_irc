#ifndef ENV_HPP
#define ENV_HPP

#include <string>
using std::string;

#include "Client.hpp"
#include "Channel.hpp"

class env {
	public:
		string				server_address;
		vector<Client>		clients;
		vector<Channel>		channels;
};

#endif /* end of include guard: ENV_HPP */
