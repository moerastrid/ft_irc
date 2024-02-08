/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 11:09:53 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:26:45 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
	using std::vector;
#include <ostream>
	using std::ostream;
#include <algorithm>
	using std::find;
#include <utility>
	using std::pair;
#include <string>
	using std::string;
	using std::to_string;
#include <deque>
	using std::deque;

#include "Client.hpp"
	class Client; // Forward declaration;

#include "Msg.hpp"

class Channel {
	private:
		string				name;
		string				password;
		string				topic;
		vector<Client *>	members;
		vector<Client *>	operators;
		vector<Client *>	invited;

		bool				inviteOnly;
		bool				operatorOnly = true;
		size_t				userLimit;

	public:
		static Channel		nullchan;

		Channel();
		~Channel();
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		Channel(string name, string password);

		const string&			getName() const;
		const string&			getPassword() const;
		const string&			getTopic() const;
		vector<Client *>&		getMembers();
		const vector<Client *>&	getMembersConst() const;
		const vector<Client *>&	getOperators() const;
		size_t					getUserLimit() const;
		pair<string,string>		getModes() const;

		bool	empty() const;
		bool	isInviteOnly() const;
		bool	hasTopicRestricted() const;
		bool	hasOperator(const Client& client) const;
		bool	hasMember(const Client& client) const;
		bool	hasInvited(const Client &client) const;

		void	setPassword(const string& password);
		void	setTopic(const string& topic);
		void	setUserLimit(size_t limit);

		void	makeTopicOperatorOnly();
		void	makeInviteOnly();
		void	addOperator(Client& client);
		void	addMember(Client& client);
		void	addInvited(Client& client);

		void	takeTopicOperatorOnly();
		void	takeInviteOnly();
		bool	removeOperator(const Client& client);
		bool	removeMember(const Client& client);
		bool	removeInvited(const Client& client);

		void	sendMessageToOtherMembers(const Client& sender, const string& message);
		void	broadcastToChannel(const string& message);
};

bool operator==(const Channel& lhs, const Channel& rhs);
//std::ostream& operator<<(std::ostream& os, const Channel& channel);
