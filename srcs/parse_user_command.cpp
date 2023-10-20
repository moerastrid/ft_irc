#include <functional>
using std::function;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <sstream>
using std::istringstream;

#include <iostream>
using std::cout;
using std::endl;

#define FULL_HELP_TEXT "\
**************************Commands**************************\n\
*                      All users:                          *\n\
* /HELP [<Command>]                                        *\n\
*      The command to display the documentation for; if no *\n\
*      argument is given, the list of commands will be     *\n\
*      displyed.                                           *\n\
* /MSG [-channel | -nick] <Target> <message>               *\n\
*      Send a message to a nickname or channel.            *\n\
* /CHANNEL LIST                                            *\n\
************************************************************\n\
*                      Operator:                           *\n\
* /KICK [Channel] [Nick]                                   *\n\
*      Kick a client from the channel                      *\n\
* /INVITE [Channel] [Nick]                                 *\n\
*      Invite a client to the channel                      *\n\
* /TOPIC [Channel] [Topic]                                 *\n\
*      Set channel's topic to topic                        *\n\
* /MODE [Channel] [i] [1/0]                                *\n\
*      Set channel invite-only mode. 1 = true, 0 = false   *\n\
* /MODE [Channel] [t] [1/0]                                *\n\
*      Set channel topic restriction to operators only     *\n\
*      mode. 1 true, 0 false                               *\n\
* /MODE [Channel] [k] [Password]                           *\n\
*      Set the channel password (key) to [Password].       *\n\
*      Leave blank for no password.                        *\n\
* /MODE [Channel] [o] [User] [1/0]                         *\n\
*      Give or take channel operator status to [User]      *\n\
* /MODE [Channel] [l] [max_clients]                        *\n\
*      Set the channel's user limit. 0 for unlimited       *\n\
************************************************************"

#define HELP_HELP \
"/HELP [?Command] \n\
The command to display the documentation for; if no \n\
argument is given, the list of commands will be displayed"

#define MSG_HELP \
"/MSG [Nick] [msg] \n\
Send a private message to [Nick]"

#define KICK_HELP \
"/KICK [Channel] [Nick] \n\
Kick a client from the channel"

#define INVITE_HELP \
"/INVITE [Channel] [Nick] \n\
Invite a client to the channel"

#define TOPIC_HELP \
"/TOPIC [Channel] [Topic] \n\
Set channel's topic to topic"

#define MODE_HELP \
"/mode [Channel] [i] [1/0] \n\
Set channel invite-only mode. 1 = true, 0 = false \n\
\n\
/mode [Channel] [t] [1/0] \n\
Set channel topic restriction to operators only mode. \n\
1 true, 0 false \n\
\n\
/mode [Channel] [k] [Password] \n\
Set the channel password (key) to [Password]. \n\
Leave blank for no password. \n\
\n\
/mode [Channel] [o] [User] [1/0] \n\
Give or take channel operator status to [User] \n\
\n\
/mode [Channel] [l] [max_clients] \n\
Set the channel's user limit. 0 for unlimited"

void parse_help([[maybe_unused]]const std::vector<string>& args) {
	static map<string, string> table;
	table["/help"] = HELP_HELP;
	table["/msg"] = MSG_HELP;
	table["/kick"] = KICK_HELP;
	table["/invite"] = INVITE_HELP;
	table["/topic"] = TOPIC_HELP;
	table["/mode"] = MODE_HELP;

	size_t len = args.size();

	if (len == 0) {
		cout << FULL_HELP_TEXT << endl;
		//send_private_message(server, client, FULL_HELP_TEXT);
	} else if (len == 1) {
		if (table.find(args[0]) != table.end()) {
			cout << table[args[0]] << endl;
		}
		else { // Help command not found
			string out = string("No help for ").append(args[0]);
			cout << out << endl;
			//send_private_message(server, client, out);
		}
		//send_private_message(server, client, table[args[0]])
	} else { // Too many arguments
		cout << "No help for ";
		for (const auto& el : args) {
			cout << el << " ";
		}
		cout << endl;
	}
}

void parse_msg([[maybe_unused]]const std::vector<string>& args) {
}

void parse_kick([[maybe_unused]]const std::vector<string>& args) {
}

void parse_invite([[maybe_unused]]const std::vector<string>& args) {
}

void parse_topic([[maybe_unused]]const std::vector<string>& args) {
}

void parse_mode([[maybe_unused]]const std::vector<string>& args) {
	string mode_options[] = {"i", "t", "k", "o", "l"};
}

void parse_user_command(const string& input) {
	string commands[] = {"/help", "/msg", "/kick", "/invite", "/topic", "/mode"};

	static map<string, function<void(const vector<string>&)>> table;
	table["/help"] = parse_help;
	table["/msg"] = parse_msg;
	table["/kick"] = parse_kick;
	table["/invite"] = parse_invite;
	table["/topic"] = parse_topic;
	table["/mode"] = parse_mode;

	vector<string> args;
	istringstream ss(input);
	string arg;
	string cmd;

	ss >> cmd;

	while (ss >> arg) {
		args.push_back(arg);
	}

	if (table.find(cmd) != table.end()) {
		table[cmd](args); //run the function
	} else { //not found
		//send_private_message(server, client, "the stuff below this line")
		string out = string("Unknown command [").append(cmd).append("]");
		cout << out << endl;
		return;
	}
}

void test(string cmd) {
	static int counter = 0;
	cout << "				====Test " << counter << "====" << endl;
	cout << "Command: " << cmd << endl;

	cout << "output:\n";
	parse_user_command(cmd);
	cout << endl << endl;

	counter++;
}

int main() {
	test("/help");
	test("/help /help");
	test("/help /msg");
	test("/help /kick");
	test("/help /invite");
	test("/help /topic");
	test("/help /mode");

	// Errors: 
	cout << "===========Error tests:===========" << endl;
	test("/hlep");
	test("/help /hlep");
}