/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 12:53:56 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:29:32 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ostream>
	using std::ostream;

#include <string>
	using std::string;
	using std::to_string;

#include <poll.h>

#include "Channel.hpp"
	class Channel; // Forward declaration

#include "Msg.hpp"

class Client {
	private:
		struct pollfd	pfd;

		string			nickname = "";
		string			username = "";
		string			hostname = "";
		string			servername = "";
		string			realname = "";
		string			password = "";
		string			datatosend = ""; // data we need to send to this client
		string			datatorecv = ""; // data we need to recv from this client
		bool			expelled = false;

	public:
		static Client	nullclient;

		~Client();
		Client(int fd);
		Client(const Client& other);
		Client& operator=(const Client& other);

		const struct pollfd&	getPFD() const;
		int						getFD() const;
		const string&			getNickname() const;
		const string&			getUsername() const;
		const string&			getHostname() const;
	//	const string&			getServername() const;
		const string&			getRealname() const;
		const string&			getPassword() const;
		const string			getFullName() const;	// full name : user!name@host 

		void					setEvents(const short events);
		void					setRevents(const short revents);
		void					addEvent(const short event);
		void					removeEvent(const short event);
		void					expell();

		void					setNickname(const string& nickname);
		void					setUsername(const string& username);
		void					setHostname(const string& hostname);
		void					setServername(const string& servername);
		void					setRealname(const string& realname);
		void					setPassword(const string& password);

		void					addSendData(const string& message);
		void					addRecvData(const string& message);

		const string			takeSendData();
		const string			takeRecvData();

		bool					hasSendData() const;
		bool					hasRecvData() const;
	//	bool					isOperator(const Channel& c) const;
	//	bool					checkEvent(short event) const;
		bool					checkRevent(short revent) const;
		bool					isRegistered() const;
		const bool&				isExpelled() const;

	//	void					makeOperator(Channel& c);
	//	void					takeOperator(Channel& c);
};

bool operator==(const Client& lhs, const Client& rhs);
//std::ostream& operator<<(std::ostream& os, const Client& client);
