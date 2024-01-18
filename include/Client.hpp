#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
using std::string;

#include <ostream>
using std::ostream;

#include <string>
using std::to_string;

#include <poll.h>
#include <unistd.h>

#include "Channel.hpp"
#include <unistd.h>

class Channel; // Forward declaration

class Client {
private:
	struct pollfd	pfd;

	string			nickname = "";
	string			username = "";
	string			hostname = "";
	string			servername = "";
	string			realname = "";
	string			password = "";

	string			datatosend = ""; // data we need to send to this client
	string			datatorecv = ""; // data we need to recv from this client
	bool			expelled = false;

public:
	static Client	nullclient;

	~Client();
	Client(int fd);
	Client(const Client& other);
	Client& operator=(const Client& other);

	// Getters
	const struct pollfd&	getPFD() const;
	int						getFD() const;
	const string&			getNickname() const;
	const string&			getUsername() const;
	const string&			getHostname() const;
	const string&			getServername() const;
	const string&			getRealname() const;
	const string&			getPassword() const;
	const string			getFullName() const;

	// Setters
	void					setEvents(const short events);
	void					setRevents(const short revents);
	void					addEvent(const short event);
	void					removeEvent(const short event);
	void					expell();

	void					setNickname(const string& nickname);
	void					setUsername(const string& username);
	void					setHostname(const string& hostname);
	void					setServername(const string& servername);
	void					setRealname(const string& realname);
	void					setPassword(const string& password);

	void					addSendData(const string& message);
	void					addRecvData(const string& message);
	void					sendPrivMsg(const Client& sender, const string& message, bool colon);


	// Takers (getters, but these also modifiy the class by deleting what is taken)
	const string			takeSendData();
	const string			takeRecvData();

	// Havers (bool)
	bool					hasSendData() const;
	bool					hasRecvData() const;
	bool					isOperator(const Channel& c) const;
	bool					checkEvent(short event) const;
	bool					checkRevent(short revent) const;
	bool					isRegistered() const;
	const bool&				isExpelled() const;

	// Changers (set values on some other object)
	void					makeOperator(Channel& c);
	void					takeOperator(Channel& c);

	// Old
	void					removeSuccesfullySentDataFromBuffer(size_t	nbytes); //deprecated by takeSendData()
};

bool operator==(const Client& lhs, const Client& rhs);
std::ostream& operator<<(std::ostream& os, const Client& client);
#endif /* end of include guard: CLIENT_HPP */
