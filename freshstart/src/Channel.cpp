#include "Channel.hpp"

Channel::Channel() {
	this->name = "";
	this->password = "";
	this->topic = "";
	this->inviteOnly = false;
	this->operatorOnly = false;
	this->userLimit = 0;
	this->founderFD = -1;
};

Channel::~Channel() {};

Channel::Channel(const Channel& other) {
	this->name = other.name;
	this->password = other.password;
	this->topic = other.topic;
	this->clients = other.clients;
	this->inviteOnly = other.inviteOnly;
	this->operatorOnly = other.operatorOnly;
	this->userLimit = other.userLimit;
	this->founderFD = other.founderFD;
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		this->name = other.name;
		this->password = other.password;
		this->topic = other.topic;
		this->clients = other.clients;
		this->inviteOnly = other.inviteOnly;
		this->operatorOnly = other.operatorOnly;
		this->userLimit = other.userLimit;
		this->founderFD = other.founderFD;
	}
	return *this;
}

Channel::Channel(string name, string password, int founderFD) {
	this->name = name;
	this->password = password;
	this->topic = "Welcome to channel " + name;
	this->inviteOnly = false;
	this->operatorOnly = false;
	this->userLimit = 0;
	this->founderFD = founderFD;
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
bool Channel::isInviteOnly() const {
	return this->inviteOnly;
}
bool Channel::hasTopicRestricted() const {
	return this->operatorOnly;
}
vector<Client>& Channel::getClients() {
	return this->clients;
}
const vector<Client>& Channel::getClientsConst() const {
	return this->clients;
}
size_t Channel::getUserLimit() const {
	return this->userLimit;
}

// modes: i t k l (o not shown here)
pair<string,string> Channel::getModes() const {
	string set = "+";
	string unset = "-";

	string modeargs;

	if (this->isInviteOnly())
		set += "i";
	else
		unset += "i";

	if (this->hasTopicRestricted())
		set += "t";
	else
		unset += "t";

	if (this->password.size() > 0) {
		set += "k";
		modeargs += this->getPassword();
	}
	else
		unset += "k";

	if (this->userLimit == 0) {
		set += "l";
		modeargs += to_string(this->getUserLimit());
	}
	else
		unset += "l";

	pair<string,string> p = std::make_pair(set + unset, modeargs);
	return p;
}


bool Channel::hasMode(char mode) const {
	if (mode == 'i') {
		return this->isInviteOnly();
	}
	if (mode == 't') {
		return this->hasTopicRestricted();
	}
	if (mode == 'k') {
		return this->password.size() == 0;
	}
	if (mode == 'l') {
		return this->userLimit != 0;
	}
	return false;
}

bool Channel::hasOperator(const Client &client) const {
	if (find(this->operators.begin(), this->operators.end(), client) != this->operators.end()) {
		return true;
	}
	return false;
}
bool Channel::hasFounder(const Client &client) const {
	return client.getFD() == this->founderFD;
}

bool Channel::hasUser(const Client& client) const {
	if (find(clients.begin(), clients.end(), client) != clients.end()) {
		return true;
	}
	return false;
}

int Channel::getFounderFD() const {
	return this->founderFD;
}

void Channel::setTopic(string topic) {
	this->topic = topic;
}
void Channel::setPassword(string password) {
	this->password = password;
}
void Channel::makeInviteOnly() {
	this->inviteOnly = true;
}
void Channel::takeInviteOnly() {
	this->inviteOnly = false;
}
void Channel::makeTopicOperatorOnly() {
	this->operatorOnly = true;
}
void Channel::takeTopicOperatorOnly() {
	this->operatorOnly = false;
}
void Channel::setUserLimit(size_t limit) {
	this->userLimit = limit;
}
void Channel::addMode(char mode, string password, size_t userLimit) {
	if (mode == 'i') {
		this->makeInviteOnly();
	}
	if (mode == 't') {
		this->makeTopicOperatorOnly();
	}
	if (mode == 'k') {
		this->password = password;
	}
	if (mode == 'l') {
		this->userLimit = userLimit;
	}
}

void Channel::removeMode(char mode) {
	if (mode == 'i') {
		this->takeInviteOnly();
	}
	if (mode == 't') {
		this->takeTopicOperatorOnly();
	}
	if (mode == 'k') {
		this->password = "";
	}
	if (mode == 'l') {
		this->userLimit = 0;
	}
}

void Channel::addOperator(const Client &client) {
	if (find(this->operators.begin(), this->operators.end(), client) == this->operators.end()) {
		this->operators.push_back(client);
	}
}

void Channel::removeOperator(const Client &client) {
	vector<Client>::iterator it = find(operators.begin(), operators.end(), client);
	if (it != operators.end()) {
		this->operators.erase(it);
	}
	return;
}

void Channel::addClient(const Client& client) {
	if (find(this->clients.begin(), this->clients.end(), client) == this->clients.end()) {
		this->clients.push_back(client);
	}
}

int Channel::removeClient(const Client& client) {
	vector<Client>::iterator it = find(clients.begin(), clients.end(), client);
	if (it == clients.end()) {
		return 1;
	}
	this->clients.erase(it);
	return 0;
}

Channel Channel::nullchan;

ostream& operator<<(ostream& os, const Channel& channel) {
	os << "Channel(" << channel.getName() << ", [";
	const vector<Client>& clients = channel.getClientsConst();
	for (vector<Client>::const_iterator it = clients.begin(); it != clients.end(); it++) {
		os << *it;
		if (it + 1 != clients.end()) {
			os << ", ";
		}
	}
	os << "])";

	return os;
}

bool operator==(const Channel& lhs, const Channel& rhs) {
	return lhs.getName() == rhs.getName();
}

void Channel::toggleInviteOnly()
{
	if (this->isInviteOnly()) {
		this->takeInviteOnly();
	} else {
		this->makeInviteOnly();
	}
}

const vector<Client> &Channel::getOperators() const {
	return this->operators;
}
