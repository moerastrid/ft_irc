/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 11:13:19 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 19:35:28 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Static
Channel Channel::nullchan;

// Public
Channel::Channel() {
	this->name = "";
	this->password = "";
	this->topic = "";
	this->inviteOnly = false;
	this->operatorOnly = true;
	this->userLimit = 0;
	Msg("channel - default constructor", "CLASS");
};

Channel::~Channel() {
	Msg("channel - default destructor", "CLASS");
};

Channel::Channel(const Channel& other) {
	this->name = other.name;
	this->password = other.password;
	this->topic = other.topic;
	this->members = other.members;
	this->inviteOnly = other.inviteOnly;
	this->operatorOnly = other.operatorOnly;
	this->userLimit = other.userLimit;
	Msg("channel - copy constructor", "CLASS");
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		this->name = other.name;
		this->password = other.password;
		this->topic = other.topic;
		this->members = other.members;
		this->inviteOnly = other.inviteOnly;
		this->operatorOnly = other.operatorOnly;
		this->userLimit = other.userLimit;
	}
	return *this;
	Msg("channel - assignation operator", "CLASS");
}

Channel::Channel(string name, string password) {
	this->name = name;
	this->password = password;
	this->topic = "Welcome to channel " + name;
	this->inviteOnly = false;
	this->operatorOnly = true;
	this->userLimit = 0;
	Msg("channel - constructor(name, pass)", "CLASS");
}

const string& Channel::getName() const {
	return this->name;
}
const string& Channel::getPassword() const {
	return this->password;
}
const string& Channel::getTopic() const {
	return this->topic;
}
vector<Client *>& Channel::getMembers() {
	return this->members;
}
const vector<Client *>& Channel::getMembersConst() const {
	return this->members;
}
const vector<Client *>& Channel::getOperators() const {
	return this->operators;
}
size_t Channel::getUserLimit() const {
	return this->userLimit;
}
pair<string,string> Channel::getModes() const { // modes: i t k l (o not shown here)
	string set = "+";
	string modeargs;

	if (this->isInviteOnly())
		set += "i";
	if (this->hasTopicRestricted())
		set += "t";
	if (this->password.size() > 0) {
		set += "k";
		modeargs += this->getPassword();
	}
	if (this->userLimit != 0) {
		set += "l";
		modeargs += to_string(this->getUserLimit());
	}
	pair<string,string> p = std::make_pair(set, modeargs);
	return p;
}

bool	Channel::empty() const {
	return (members.empty());
}
bool Channel::isInviteOnly() const {
	return this->inviteOnly;
}
bool Channel::hasTopicRestricted() const {
	return this->operatorOnly;
}
bool Channel::hasOperator(const Client &client) const {
	if (find(this->operators.begin(), this->operators.end(), &client) != this->operators.end()) {
		return true;
	}
	return false;
}
bool Channel::hasMember(const Client& client) const {
	if (find(this->members.begin(), this->members.end(), &client) != this->members.end()) {
		return true;
	}
	return false;
}
bool	Channel::hasInvited(const Client &client) const {
	if (find(this->invited.begin(), this->invited.end(), &client) != this->invited.end()) {
		return true;
	}
	return false;
}

void Channel::setPassword(const string& password) {
	this->password = password;
}
void Channel::setTopic(const string& topic) {
	this->topic = topic;
}
void Channel::setUserLimit(size_t limit) {
	this->userLimit = limit;
}

void Channel::makeTopicOperatorOnly() {
	this->operatorOnly = true;
}
void Channel::makeInviteOnly() {
	this->inviteOnly = true;
}
void Channel::addOperator(Client &client) {
	if (find(operators.begin(), operators.end(), &client) == operators.end()) {
		this->operators.push_back(&client);
	}
}
void Channel::addMember(Client& client) {
	if (find(this->members.begin(), this->members.end(), &client) == this->members.end()) {
		this->members.push_back(&client);
	}
	if (this->hasInvited(client)) {
		this->removeInvited(client);
	} 
}
void	Channel::addInvited(Client& client) {
	if (find(this->invited.begin(), this->invited.end(), &client) == this->invited.end()) {
		this->invited.push_back(&client);
	}
}

void Channel::takeTopicOperatorOnly() {
	this->operatorOnly = false;
}
void Channel::takeInviteOnly() {
	this->inviteOnly = false;
}
bool Channel::removeOperator(const Client& client) {
	auto it = find(operators.begin(), operators.end(), &client);
	if (it == operators.end()) {
		return false;
	}
	this->operators.erase(it);
	return true;
}
bool Channel::removeMember(const Client& client) {
	auto it = find(members.begin(), members.end(), &client);
	if (it == members.end()) {
		return false;
	}

	this->members.erase(it);
	if (this->hasOperator(client))
		this->removeOperator(client);

	if (this->hasInvited(client)) {
		this->removeInvited(client);
	} 

	return true;
}
bool Channel::removeInvited(const Client& client) {
	auto it = find(invited.begin(), invited.end(), &client);
	if (it == invited.end()) {
		return false;
	}
	this->invited.erase(it);
	return true;
}

void	Channel::sendMessageToOtherMembers(const Client& sender, const string& message) {

	vector<Client *> channel_members = this->getMembers();
	if (channel_members.empty())
		return ;

	for (Client * member : channel_members) {
		if (member->getFD() != sender.getFD())
			member->addSendData(message);
	}
}
void	Channel::broadcastToChannel(const string& message) {
	vector<Client *> channel_members = this->getMembers();
	if (channel_members.empty())
		return ;

	for (Client * member : channel_members) {
		member->addSendData(message);
	}
}

bool operator==(const Channel& lhs, const Channel& rhs) {
	return lhs.getName() == rhs.getName();
}

// ostream& operator<<(ostream& os, const Channel& channel) {
// 	os << "Channel(" << channel.getName() << ", [";
// 	const vector<Client *>& members = channel.getMembersConst();
// 	for (vector<Client *>::const_iterator it = members.begin(); it != members.end(); it++) {
// 		os << *it;
// 		if (it + 1 != members.end()) {
// 			os << ", ";
// 		}
// 	}
// 	os << "])";

// 	return os;
// }