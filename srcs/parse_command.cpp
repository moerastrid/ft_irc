#include <string>
using std::string;

#include <string.h>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include "Client.hpp"

#define HELP_TEXT \
"/**************************Commands**************************\
 *                      All users:                          *\
 * /help [?Command]                                         *\
 *      Get this help text.                                 *\
 *      Or enter a command to get detailed instructions     *\
 * /msg [Nick] \"msg\"                                        *\
 *      Send a private message to [Nick]                    *\
 *                                                          *\
 ************************************************************\
 *                      Operator:                           *\
 * /kick [Nick]                                             *\
 *      Kick a client from a channel                        *\
 * /invite [Nick]                                           *\
 *      Invite a client to a channel                        *\
 * /topic [Channel] [Topic]                                 *\
 *      Set channel's topic to topic                        *\
 * /mode [Channel] [i] [1/0]                                *\
 *      Set channel invite-only mode. 1 = true, 0 = false   *\
 * /mode [Channel] [t] [1/0]                                *\
 *      Set channel topic restriction to operators only     *\
 *      mode. 1 true, 0 false                               *\
 * /mode [Channel] [k] [Password]                           *\
 *      Set the channel password (key) to [Password].       *\
 *      Leave blank for no password.                        *\
 * /mode [Channel] [o] [User] [1/0]                         *\
 *      Give/take channel operator status to [User]         *\
 * /mode [Channel] [l] [max_clients]                        *\
 *      Set the channel's user limit. 0 for unlimited       *\
 ************************************************************/"

void help(const string& arg) {
    if (arg.compare("") == 0)
    cout << HELP_TEXT << endl;

    // Put implementation for other help pages here. 
}

void send_private_message(const Client& sender, const Client& receiver) {

}

void parse_command(const string& cmd) {
    string commands[] = {"/help", "/msg", "/kick", "/invite", "/topic", "/mode"};
    vector<void (*)(void *)> funcs;

    funcs.push_back(reinterpret_cast<void (*)(void *)>(help));
    funcs.push_back(reinterpret_cast<void (*)(void *)>(send_private_message));


    string mode_options[] = {"i", "t", "k", "o", "l"};

    string first = strtok((char*)cmd.c_str(), string(" ").c_str());

    for (const auto& el : commands) {
        if (first.compare(el) == 0) {

        }
    }
}



int main() {
    parse_command("/help je moeder");
}