/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 12:53:59 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 20:29:30 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Static
Client Client::nullclient = Client(-1);

Client::~Client() { 
	Msg("client - default destructor", "CLASS");
}
Client::Client(int fd) {
	this->pfd.fd = fd;
	this->pfd.events = POLLIN|POLLHUP|POLLRDHUP;
	this->pfd.revents = 0;
	Msg("client - constructor(FD)", "CLASS");
}
Client::Client() {
	this->pfd.fd = 0;
	this->pfd.events = POLLIN|POLLHUP|POLLRDHUP;
	this->pfd.revents = 0;
	Msg("client - default constructor", "CLASS");
}
Client::Client(const Client& other) {
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
	this->password = other.password;
	this->pfd = other.pfd;
	Msg("client - copy constructor", "CLASS");
}
Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->nickname = other.nickname;
		this->username = other.username;
		this->hostname = other.hostname;
		this->servername = other.servername;
		this->realname = other.realname;
		this->password = other.password;
		this->pfd = other.pfd;
	}
	Msg("client - assignation operator", "CLASS");
	return *this;
}

const struct pollfd	&Client::getPFD() const {
	return this->pfd;
}
int Client::getFD() const {
	return this->pfd.fd;
}
const string& Client::getNickname() const {
	return this->nickname;
}
const string& Client::getUsername() const {
	return this->username;
}
const string& Client::getHostname() const {
	return this->hostname;
}
const string& Client::getRealname() const {
	return this->realname;
}
const string& Client::getPassword() const {
	return this->password;
}
const string	Client::getFullName() const {
	return this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();
}

void Client::setEvents(const short events) {
	this->pfd.events = events;
}
void Client::setRevents(const short revents) {
	this->pfd.revents = revents;
}
void Client::addEvent(const short event) {
	this->pfd.events |= event;
}
void Client::removeEvent(const short event) {
	this->pfd.events &= ~event; // bitwise AND with bitwise negation (~).
}

void Client::expell() {
	this->expelled = true;
}

void Client::setNickname(const string& nickname) {
	this->nickname = nickname;
}
void Client::setUsername(const string& username) {
	this->username = username;
}
void Client::setHostname(const string& hostname) {
	this->hostname = hostname;
}
void Client::setServername(const string& servername) {
	this->servername = servername;
}
void Client::setRealname(const string& realname) {
	this->realname = realname;
}
void Client::setPassword(const string& password) {
	this->password = password;
}

void Client::addSendData(const string& message) {
	if (!message.empty()) {
		addEvent(POLLOUT);
		this->datatosend += message;
	}
}
void Client::addRecvData(const string& message) {
	this->datatorecv += message;
}

const string Client::takeSendData() {
	string res = "";
	size_t pos = this->datatosend.find('\n');
	if (pos != string::npos) {
		res = this->datatosend.substr(0, pos + 1);
		this->datatosend = this->datatosend.substr(pos + 1);
	}
	return res;
}
const string Client::takeRecvData() {
	string res = "";
	size_t pos = this->datatorecv.find('\n');
	if (pos != string::npos) {
		res = this->datatorecv.substr(0, pos + 1);
		this->datatorecv = this->datatorecv.substr(pos + 1);
	}
	return res;
}

bool Client::hasSendData() const {
	return this->datatosend.find('\n') != string::npos;
}
bool Client::hasRecvData() const {
	return this->datatorecv.find('\n') != string::npos;
}
bool Client::checkRevent(short revent) const {
	return this->pfd.revents & revent;
}
bool Client::isRegistered() const {
	return !username.empty() && !nickname.empty();
}
const bool&	Client::isExpelled() const {
	return(this->expelled);
}

// Out of Class
bool operator==(const Client &lhs, const Client &rhs) {
	return 	lhs.getFD() == rhs.getFD() &&
			lhs.getNickname() == rhs.getNickname() &&
			lhs.getUsername() == rhs.getUsername();
}

ostream& operator<<(ostream& os, const Client& client) {
	string str = string("Client(") + to_string(client.getFD()) + ", " + client.getNickname() + ", " + client.getUsername() + ", " + client.getHostname() + ", " + client.getRealname() + ")";
	os << str;
	return os;
}

