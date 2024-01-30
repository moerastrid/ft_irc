#include "Client.hpp"
#include "Msg.hpp"

Client::Client(int fd) {
	this->pfd.fd = fd;
	this->pfd.events = POLLIN|POLLHUP|POLLRDHUP;
	this->pfd.revents = 0;
	Msg("client - default constructor", "CLASS");
};

Client::~Client() { 
	Msg("client - default destructor", "CLASS");
}

Client::Client(const Client& other) {
	this->nickname = other.nickname;
	this->username = other.username;
	this->hostname = other.hostname;
	this->servername = other.servername;
	this->realname = other.realname;
	this->password = other.password;
	this->pfd = other.pfd;
	Msg("client - copy costructor", "CLASS");
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

// Getters
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
const string& Client::getPassword() const {
	return this->password;
}

/*
In IRC, there are certain messages where the full user!nick@host format (often referred to as the "prefix") is expected. This prefix provides additional information about the user sending the message, including their username and host. Here are some common examples:

    Private Message (PRIVMSG) and Notice (NOTICE):
        The prefix is often included in messages to indicate the sender's identity.
        Example:

        ruby

    :user!nick@host PRIVMSG #channel :Hello, everyone!

Join (JOIN), Part (PART), Quit (QUIT), Kick (KICK), Mode (MODE):

    These messages typically include the full user!nick@host prefix to identify the user performing the action.
    Example:

    ruby

    :user!nick@host JOIN #channel

Topic (TOPIC):

    The user!nick@host prefix is used in messages related to changing or querying the channel topic.
    Example:

    ruby

    :user!nick@host TOPIC #channel :New topic

Nick Change (NICK):

    The user!nick@host prefix is used to indicate the user changing their nickname.
    Example:

    ruby

    :olduser!oldnick@oldhost NICK newnick

Channel Mode (MODE):

    The prefix is often included to identify the user setting or removing channel modes.
    Example:

    ruby

        :user!nick@host MODE #channel +o otheruser

These examples demonstrate scenarios where the full user!nick@host format is commonly used. It's important to note that not all messages require the full prefix, and some may only use the nickname portion. The inclusion of the full prefix provides more detailed information about the sender's identity.
*/

// full name : user!name@host 
const string	Client::getFullName() const {
	std::string temp = this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();
	return(temp);
}

// Setters
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


// Takers
const string Client::takeSendData() { //was GetSendData(). I've changed it to return just one full line, or an empty string if there is no newline, and it also removes the line from the buffer if it has taken one.
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

// Havers (bool)
bool Client::hasSendData() const {
	return this->datatosend.find('\n') != string::npos;
}
bool Client::hasRecvData() const {
	return this->datatorecv.find('\n') != string::npos;
}
bool Client::isOperator(const Channel& c) const {
	return c.hasOperator(*this);
}

bool Client::checkEvent(short event) const {
	return this->pfd.events & event;
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

// Changers
void Client::makeOperator(Channel& c) {
	c.addOperator(*this);
}
void Client::takeOperator(Channel& c) {
	c.removeOperator(*this);
}


// Old
void Client::removeSuccesfullySentDataFromBuffer(size_t nbytes) {
	datatosend.erase(0, nbytes);
}

// Static
Client Client::nullclient = Client(-1);

// Out of Class stuff
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

