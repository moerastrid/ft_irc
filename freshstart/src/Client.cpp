#include "Client.hpp"

Client::Client(int fd) {
	this->pfd.fd = fd; // effectively const
	this->pfd.events = POLLIN|POLLHUP|POLLRDHUP;
};

Client::~Client() {}

Client::Client(const Client& other) {
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
	this->pfd = other.pfd;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		this->nickname = other.nickname;
		this->username = other.username;
		this->hostname = other.hostname;
		this->servername = other.servername;
		this->realname = other.realname;
		this->pfd = other.pfd;
	}
	return *this;
}

void Client::setEvents(const short& events) {
	this->pfd.events = events;
}

void Client::setRevents(const short& revents) {
	this->pfd.revents = revents;
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
void Client::makeOperator(Channel& c) {
	c.addOperator(*this);
}
void Client::takeOperator(Channel& c) {
	c.removeOperator(*this);
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
const string& Client::getServername() const {
	return this->servername;
}
const string& Client::getRealname() const {
	return this->realname;
}
const string& Client::getDataToSend() const {
	return this->datatosend;
}
bool Client::isOperator(const Channel& c) const {
	return c.hasOperator(*this);
}
bool Client::isFounder(const Channel& c) const {
	return c.hasFounder(*this);
}

bool Client::checkEvent(short event) {
	return this->pfd.events & event;
}

bool Client::checkRevent(short revent) {
	return this->pfd.revents & revent;
}

void Client::addSendData(const string& message) {
	this->datatosend += message;
}

Client Client::nullclient = Client(-1);

ostream& operator<<(ostream& os, const Client& client) {
	string str = string("Client(") + to_string(client.getFD()) + ", " + client.getNickname() + ", " + client.getUsername() + ", " + client.getHostname() + ", " + client.getServername() + ", " + client.getRealname() + ")";
	os << str;
	return os;
}

bool operator==(const Client &lhs, const Client &rhs) {
	return 	lhs.getFD() == rhs.getFD() &&
			lhs.getNickname() == rhs.getNickname() &&
			lhs.getUsername() == rhs.getUsername();
}


void	Client::removeSuccesfullySentDataFromBuffer(size_t	nbytes) {
	datatosend.erase(0, nbytes);
}