#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "Msg.hpp"
#include "Client.hpp"
#include "Executor.hpp"
#include "Env.hpp"

#include <fstream>

#define BUFSIZE 512

#include <poll.h>

class CustomOutputStream : public ostream {
public:
	CustomOutputStream(ostream& output) : ostream(output.rdbuf()), output_stream(output) {}

	template <typename T>
	CustomOutputStream& operator<<(const T& value) {
		for (const auto& el : value) {
			if (el == '\r')
				output_stream << "\\r";
			else if (el == '\n')
				output_stream << "\\n";
			else
				output_stream << el;
		}
		return *this;
	};

	// Override the << operator for endl
	CustomOutputStream& operator<<(ostream& (*manipulator)(ostream&)) {
		manipulator(output_stream);
		return *this;
	};

private:
	ostream& output_stream;
};

class Server {
	private :

		const string	name = "REAL TALK IRC";
		const string	slogan = "Now we're talking";
		Env& 			env;

		struct sockaddr_in		sockin;
		struct pollfd			sockfd;

		void	setUp();
		void	setInfo();
		int		setPoll();
		// void	incomingConnection();
		void	addConnection();
		void	closeConnection(const int fd);
		// string	receive(int fd);
		bool	receivefromClient(Client &c);
		bool	sendtoClient(Client &c);

	public :

		static CustomOutputStream customOut; //#TODO delete all CustomOutput related code before handing in.
		static CustomOutputStream customErr;
		// static std::ofstream outputfile;

		Server(Env& env);								// default constructor
		~Server();										// default destructor
		Server(const Server &src);						// copy constructor
		Server &operator=(const Server &src);			// = sign operator
		Server(const int port, const string pass);		// constructor (PORT, pass)
		
		void						run(Executor& ex);
		const string				getIP() const;
		int							getPort() const;
		const string				getName() const;
		const string				getHostname() const;
		struct pollfd**				getPollFDS() const;
		Client* 					getClientByFD(int fd);
		vector<Client>::iterator	getItToClientByFD(int fd);
};

std::ostream& operator<<(std::ostream& os, const Server& server);
