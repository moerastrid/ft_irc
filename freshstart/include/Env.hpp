#ifndef ENV_HPP
#define ENV_HPP

#include <string.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>

#include <string>
	using std::string;

#include "Client.hpp"
#include "Channel.hpp"

class Env {
	private:
		deque<Client>		clients;
		deque<Channel>		channels;

		string				pass;
		string				hostname	= "localhost";
		string				ip			= "0.0.0.0";
		int					port;
	
	public:
		Env(int port, string pass);

		//getters
		Client&						getClientByFD(int fd);
		Client&						getClientByNick(const string& nick);
		Channel&					getChannelByName(const string& name);
		deque<Client>::iterator		getItToClientByFD(int fd);
		deque<Client>::iterator		getItToClientByNick(const string& nick);
		deque<Channel>::iterator	getItToChannelByName(const string& name);
		deque<Channel>::iterator	getItToChannels();
		deque<Client>&				getClients();
		deque<Channel>& 			getChannels();
		int							getPort() const;
		const string&				getPass() const;
		const string&				getHostname() const;
		const string&				getIP() const;

		//setters
		void						setHostname(const string &hostname);
		void						setIP(const string &ip);
};

std::ostream& operator<<(std::ostream& os, const Env& e);

#endif /* end of include guard: ENV_HPP */
