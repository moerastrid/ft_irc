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
	struct pollfd	pfd;

	string			nickname;
	string			username;
	string			hostname;
	string			servername;
	string			realname;
	string			password;
	bool			sending;
	string			datatosend; // to do astrid : []\n[]\n[]\n[]\n


public:
	static Client	nullclient;
	string			recvbuf;

	~Client();
	Client(int fd);
	Client(const Client& other);
	Client& operator=(const Client& other);

	const struct pollfd&	getPFD() const;
	int						getFD() const;
	const string&			getNickname() const;
	const string&			getUsername() const;
	const string&			getHostname() const;
	const string&			getServername() const;
	const string&			getRealname() const;
	const string&			getPassword() const;
	const string&			getDataToSend() const;
	bool					isOperator(const Channel& c) const;
	bool					isFounder(const Channel& c) const;

	void					setEvents(const short& events);
	void					setRevents(const short& revents);
	void					setNickname(const string& nickname);
	void					setUsername(const string& username);
	void					setHostname(const string& hostname);
	void					setServername(const string& servername);
	void					setRealname(const string& realname);
	void					setPassword(const string& password);
	void					makeOperator(Channel& c);
	void					takeOperator(Channel& c);

	bool					checkEvent(short event);
	bool					checkRevent(short revent);

	void					addSendData(const string& message);
	void					removeSuccesfullySentDataFromBuffer(size_t	nbytes);
};

bool operator==(const Client& lhs, const Client& rhs);
std::ostream& operator<<(std::ostream& os, const Client& client);
#endif /* end of include guard: CLIENT_HPP */