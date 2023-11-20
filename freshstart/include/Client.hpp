#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
using std::string;

#include <ostream>
using std::ostream;

#include <string>
using std::to_string;

#include <poll.h>


#include "Channel.hpp"

class Channel; // Forward declaration

class Client {
private:
	// const int		fd;
	struct pollfd	pfd;
	
	string			nickname;
	string			username;
	string			hostname;
	string			servername;
	string			realname;
	string			password;

public:
	~Client();
	Client(int fd);
	Client(const Client& other);
	Client& operator=(const Client& other);

	int getFD() const;
	const string& getNickname() const;
	const string& getUsername() const;
	const string& getHostname() const;
	const string& getServername() const;
	const string& getRealname() const;
	const string& getPassword() const;
	bool isOperator(Channel& c) const;
	bool isFounder(Channel& c) const;

	void setNickname(string& nickname);
	void setUsername(string& username);
	void setHostname(string& hostname);
	void setServername(string& servername);
	void setRealname(string& realname);
	void setPassword(string& password);
	void makeOperator(Channel& c);
	void takeOperator(Channel& c);

	bool checkEvent(short event);
	bool checkRevent(short revent);

};

bool operator==(const Client& lhs, const Client& rhs);
std::ostream& operator<<(std::ostream& os, const Client& client);
#endif /* end of include guard: CLIENT_HPP */
