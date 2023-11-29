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
		const string			name	= "REAL TALK IRC";
		const string			slogan	= "Now we're talking";
		Env& 					e;
		struct sockaddr_in		sockin;
		struct pollfd			sockfd;

		void	setUp();
		void	setInfo();
		int		setPoll();
		void	addConnection();
		void	closeConnection(const int fd);
		bool	receivefromClient(Client &cl);
		bool	sendtoClient(Client &cl);

	public :
		static CustomOutputStream customOut; //#TODO delete all CustomOutput related code before handing in.
		static CustomOutputStream customErr;

		Server(Env& env);								// constructor (env)
		~Server();										// default destructor
		Server(const Server &src);						// copy constructor
		Server &operator=(const Server &src);			// = sign operator
		
		void						run(Executor& ex);
		const string				getName() const;

		class ServerException : public std::runtime_error {
		public:
			ServerException(const std::string& message) : std::runtime_error(message) {}
		};
};

std::ostream& operator<<(std::ostream& os, const Server& server);
