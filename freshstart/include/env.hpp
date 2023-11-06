#pragma once

#include <string>
using std::string;

#include "Client.hpp"
#include "Channel.hpp"

class env {
	public:
		string				server_address;
		vector<Client>		clients;
		vector<Channel>		channels;
		string				server_password;
};

