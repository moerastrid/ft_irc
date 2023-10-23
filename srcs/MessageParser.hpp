#ifndef MESSAGE_PARSER_HPP
#define MESSAGE_PARSER_HPP

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <algorithm>

#include "Command.hpp"

class MessageParser { // This probably doesn't need to be a class... Unless we also make a "Command" class, then this class can work as a parser that translates an IRC server message string into a structure containing a command and arguments, a structure that an Executor class can "run". Seems like a lot of extra work (for little reward, maybe). But maybe it's worth it. It does encapsulate the backend logic of the project nicely.
private:

public:
	MessageParser();
	~MessageParser();

	Command parse_client_message(void* env, const string& client_message_string, const int active_client_fd);

};

#endif /* end of include guard: MESSAGE_PARSER_HPP */
