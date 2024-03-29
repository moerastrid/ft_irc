/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Env.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 13:31:22 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 18:20:48 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>

#include <deque>
	using std::deque;
#include <ostream>
	using std::ostream;
#include <string>
	using std::string;

#include "Client.hpp"
#include "Channel.hpp"
#include "Msg.hpp"

class Env {
	private:
		deque<Client *>		clients;
		deque<Channel>		channels;

		string				pass;
		string				hostname	= "localhost";
		string				ip			= "0.0.0.0";
		int					port;

		Env();
		Env(const Env &src);
		Env &operator=(const Env &src);
	
	public:
		~Env();
		Env(int port, string pass);

		Client&						getClientByFD(int fd) const;
		Client&						getClientByNick(const string& nick);
		Channel&					getChannelByName(const string& name);
		deque<Client *>::iterator	getItToClientByFD(int fd);
		deque<Channel>::iterator	getItToChannelByName(const string& name);
		const deque<Client *>&		getClients() const;
		deque<Channel>& 			getChannels();
		int							getPort() const;
		const string&				getPass() const;
		const string&				getHostname() const;
		const string&				getIP() const;

		void						setHostname(const string &hostname);
		void						setIP(const string &ip);

		void						addClient(int fd);
		void						removeClient(int fd);
		void						reset();
};

std::ostream& operator<<(std::ostream& os, const Env& e);
