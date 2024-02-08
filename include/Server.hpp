/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 18:15:53 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 19:34:27 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

#include <cstring>
#include <fstream>
#include <string>
	using std::string;
#include <vector>
	using std::vector;

#include "Msg.hpp"
#include "Client.hpp"
#include "Executor.hpp"
#include "Env.hpp"

#define BUFSIZE 512

class Executor; // Forward declaration

class Server {
	private :
		const string			name	= "REAL TALK IRC";
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

		bool	comm_pollin(Executor& ex, Client &client);
		void	comm_pollout(Client &client);

	public :
		Server(Env& env);
		~Server();
		
		void			run(Executor& ex);
		const string	getName() const;

		class ServerException : public std::runtime_error {
		public:
			ServerException(const std::string& message) : std::runtime_error(message) {}
		};
};

std::ostream& operator<<(std::ostream& os, const Server& server);
