#ifndef ENV_HPP
#define ENV_HPP

#include <string.h>

#include <string>
using std::string;

#include <poll.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>

#include "Client.hpp"
#include "Channel.hpp"

class Env {
	public:
		vector<Client>		clients;
		vector<Channel>		channels;

		string				pass;
		string				hostname = "localhost";
		string				ip = "0.0.0.0";
		int					port;

		Env(string pass);
};

#endif /* end of include guard: ENV_HPP */
