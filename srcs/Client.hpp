#include <string>

#include "Channel.hpp"

using std::string;

class Client {
    private:
        int         fd;
        string      username;
        string      nickname;
        // Channel*    joined_channels;
        
        Client();

    public:
        ~Client();
        Client(const Client& other);
        Client& operator=(const Client& other);
        
        // Client(/*args go here*/);
};