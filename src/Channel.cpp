#include "Channel.hpp"
#include "Msg.hpp"

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
bool Channel::isInviteOnly() const {
	return this->inviteOnly;
}
bool	Channel::empty() const {
	return (members.empty());
}

bool Channel::hasTopicRestricted() const {
	return this->operatorOnly;
}
vector<Client *>& Channel::getMembers() {
	return this->members;
}
const vector<Client *>& Channel::getMembersConst() const {
	return this->members;
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

void Channel::addOperator(Client &client) {
	if (find(operators.begin(), operators.end(), &client) == operators.end()) {
		this->operators.push_back(&client);
	}
}

void Channel::removeOperator(const Client &client) {
	auto it = find(operators.begin(), operators.end(), &client);
	if (it != operators.end()) {
		this->operators.erase(it);
	}

	if (this->getOperators().empty()) {
		vector<Client *>members = this->getMembers();
		std::vector<Client *>::iterator new_operator_it = members.begin();
		while (new_operator_it != members.end() && *new_operator_it != &client) {
			new_operator_it = next(new_operator_it);
		}
		if (new_operator_it == members.end())
			return;

		Client* new_operator = *new_operator_it;
		this->addOperator(**new_operator_it);

		for (auto member : members) {
			member->sendPrivMsg(client, new_operator->getNickname() + " is the new operator of the '" + this->getName() + "'-channel.\n", true);
		}
	}

	return;
}

void Channel::addMember(Client& client) {
	if (find(this->members.begin(), this->members.end(), &client) == this->members.end()) {
		broadcastToChannel(":" + client.getFullName() + " JOIN :" + this->getName() + "\r\n");
		this->members.push_back(&client);
	}
}

int Channel::removeMember(const Client& client) {
	auto it = find(members.begin(), members.end(), &client);
	if (it == members.end()) {
		return 1;
	}

	this->members.erase(it);
	if (this->hasOperator(client))
		this->removeOperator(client);

	return 0;
}

void	Channel::broadcastToChannel(const string& message) {
	vector<Client *> channel_members = this->getMembers();
	if (channel_members.empty())
		return ;

	for (Client * member : channel_members) {
		member->addSendData(message);
	}
}

void	Channel::sendMessageToChannelMembers(const Client& sender, const string& message, bool colon) {

	vector<Client *> channel_members = this->getMembers();
	if (channel_members.empty())
		return ;

	string prefix = ":" + sender.getFullName() + " PRIVMSG " + this->getName();
	string colon_str = "";
	if (colon)
		colon_str = ":";
	string reply = prefix + " " + colon_str + message;

	for (Client * member : channel_members) {
		if (member != &sender)
			member->addSendData(reply);
	}
}

// void	Channel::sendKickReasonToChannelMembers(const Client& sender, const Client& target, const string& message) {

// 	vector<Client *> channel_members = this->getMembers();

// 	string prefix = ":" + sender.getNickname() + 
// 					"!" + sender.getUsername() + 
// 					"@" + sender.getHostname() + 
// 					" KICK " + this->getName() + " " + target.getNickname();
// 	string reply = prefix + " " + message;

// 	for (Client * member : channel_members) {
// 		if (member != &sender)
// 			member->addSendData(reply);
// 	}
// }


Channel Channel::nullchan;

ostream& operator<<(ostream& os, const Channel& channel) {
	os << "Channel(" << channel.getName() << ", [";
	const vector<Client *>& members = channel.getMembersConst();
	for (vector<Client *>::const_iterator it = members.begin(); it != members.end(); it++) {
		os << *it;
		if (it + 1 != members.end()) {
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

const vector<Client *>& Channel::getOperators() const {
	return this->operators;
}
