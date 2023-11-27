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
	private:
		vector<Client>		clients;
		vector<Channel>		channels;

		string				pass;
		string				hostname = "localhost";
		string				ip = "0.0.0.0";
		int					port;
	
	public:
		Env(int port, string pass);
		const Client& getClientByFD(int fd) const;
		vector<Client>& getClients();
};

#endif /* end of include guard: ENV_HPP */
