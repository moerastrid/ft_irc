#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
using std::string;

#include "Channel.hpp"

class Channel; // Forward declaration

class Client {
private:
	const int		fd;
	string			nickname;
	string			username;
	string			hostname;
	string			servername;
	string			realname;

public:

	~Client();
	Client(int fd);
	Client(const Client& other);
	Client& operator=(const Client& other);

	void setNickname(string& nickname);
	void setUsername(string& username);
	void setHostname(string& hostname);
	void setServername(string& servername);
	void setRealname(string& realname);

	int getFD() const;
	const string& getNickname() const;
	const string& getUsername() const;
	const string& getHostname() const;
	const string& getServername() const;
	const string& getRealname() const;
};

bool operator==(const Client& lhs, const Client& rhs);
std::ostream& operator<<(std::ostream& os, const Client& client);
#endif /* end of include guard: CLIENT_HPP */
