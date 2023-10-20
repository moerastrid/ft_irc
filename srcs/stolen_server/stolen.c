#include "stolen.h"

#define MAX_USERS 100

// Utility

// void bzero(char *str, size_t size) {
// 	memset(str, '\0', size);
// }

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	int		i;
	char	*ptr;

	if (!s)
		return (NULL);
	ptr = malloc(len + 1);
	if (!ptr)
		return (NULL);
	while (start-- && *s)
		s++;
	i = 0;
	while (*s && len > 0)
	{
		ptr[i] = *s++;
		len--;
		i++;
	}
	ptr[i] = '\0';
	return (ptr);
}

char *ft_strjoin(char* a, char* b) {
	char *res = calloc(sizeof(char), strlen(a) + strlen(b) + 1);
	char *r = res;

	while (*a) {
		*r++ = *a++;
	}
	while (*b) {
		*r++ = *b++;
	}

	return res;
}

char *ft_strjoin_3(char *a, char *b, char *c) {
	char *temp = ft_strjoin(a, b);
	char *res = ft_strjoin(temp, c);
	free(temp);
	return (res);
}

char *get_next_token(char *str, const char* delim) {
	static char flag = 0;
	char *res = NULL;
	if (str != NULL && flag == 0) {
		res = strtok(str, delim);
		flag = 1;
	}
	else if (flag == 1) {
		res = strtok(NULL, delim);
		if (res == NULL) {
			flag = 0;
		}
	}
	return res;
}

size_t count_nonconsecutive_chars(const char *str, char c) {
	size_t count = 0;

	while (*str != '\0') {
		if (*str == c) {
			while (*str == c && *str != '\0')
				str++;
			count++;
		}
		if (*str != '\0')
			str++;
	}

	return count;
}

int is_valid_nickname_character(char c) {
	if (isalnum(c) || c == '_' || c == '-' || c == '[' || c == ']' || c == '\\' || c == '^' || c == '{' || c == '}')
		return 1;
	return 0;
}

// Returns a malloc-ed array of lines.
// Note: each line MUST end with a newline.
char **split_lines(const char *message) {
	char* msg_cpy = strdup(message);
	size_t num_lines = count_nonconsecutive_chars(message, '\n') + 1; //+1 for NULL terminator;
	char** lines = malloc(sizeof(char *) * num_lines);

	int i = 0;
	char* msg_ptr = msg_cpy;
	while (*msg_ptr != '\0') {
		char *start = msg_ptr;
		while (*msg_ptr != '\n' && *msg_ptr != '\0') {
			msg_ptr++;
		}
		if (*msg_ptr != '\0')
			msg_ptr++;
		lines[i++] = ft_substr(start, 0, msg_ptr - start);
	}
	lines[i] = NULL;

	free(msg_cpy);
	return lines;
}












// SERVER

void	send_all_client(s_env* env, int	nbytes, char* buff, int i) // i = sender FD
{
	int	j; // j == receiver FD;

	j = 0;
	while (j <= env->fdmax)
	{
		if (FD_ISSET(j,&env->master))
			if (j != env->sockfd && j != i) {
				
				// Find the username from the saved users by the socket's fd. 
				char *username = NULL;
				for (int k = 0; k < env->active_clients; k++) {
					if (env->clients[k]->fd == i) {
						username = env->clients[k]->nick;
					}
				}
				// If found, prepend it to the message to show which user sent it, separated by a colon.
				printf("Sending %d bytes to all clients...\n",nbytes);
				if (username != NULL) {
					char* message = ft_strjoin_3(username, ": ", buff);
					send(j, message, strlen(message), 0);
				} else {
					send(j, buff, strlen(buff), 0);
				}
			}
		j++;
	}
}

void	new_client(s_env* env)
{
	int		newfd;
	// Welcome message should only be sent AFTER receiving USER and NICK messages from the client.
	char	welcom[] = ":localhost 001 tnuyten :Welcome to the Internet Relay Network Neus!tnuyten@localhost\n";
	// char	userprompt[] = "Please enter your username: ";

	//Accept the connection and save the socket FD and more. 
	env->addr_size = sizeof env->their_addr;
	newfd = accept(env->sockfd, (struct sockaddr *)&env->their_addr, &env->addr_size);
	dprintf(2, "Accepted connection on fd %d\n", newfd);
	if (newfd > env->fdmax)
		env->fdmax = newfd;
	FD_SET(newfd, &env->master);

	// Create a new user struct and add it to env to keep track of the user's data. (Data to be received later).
	s_user* user = calloc(sizeof (s_user), 1);
	user->fd = newfd;
	user->nick = NULL;
	user->user = NULL;
	env->clients[env->active_clients++] = user;

	//Send welcome messages to the new client. (REQUIRED to maintain connection, but needs to happen later)
	dprintf(2, "%s\n", "Sending welcome message");
	send(newfd, welcom, sizeof welcom, 0);
}

void send_CAP_response([[maybe_unused]]s_env* env, int fd, [[maybe_unused]]char**args) {
	static char CAPmessage[] = ":localhost CAP NAK :-";
	dprintf(2, "Sending CAP response: [%s]\n", CAPmessage);
	send(fd, CAPmessage, sizeof CAPmessage, 0);
}

s_user* get_client_by_fd(s_env* env, int fd) {
	for (int i = 0; i < env->active_clients; i++) {
		if (env->clients[i]->fd == fd)
			return env->clients[i];
	}
	return NULL;
}

void send_server_error(s_env* env, int fd, int error_code) {
	dprintf(2, "Sending server error %d: ", error_code);

	char message[10000];
	s_user* client = get_client_by_fd(env, fd);
	sprintf(message, ":%s %d %s :%s\n", env->server_address, error_code, client->nick, errors[error_code]);
	send(fd, message, sizeof message, 0);
	dprintf(2, "[%s]\n", message);
}

void register_nickname(s_env* env, int fd, char** args) {
	char *nickname = args[0];
	if (args[0] == NULL || strlen(args[0]) == 0) {
		send_server_error(env, fd, ERR_NONICKNAMEGIVEN);
		return;
	}
	dprintf(2, "nickname: [%s]\n", ft_substr(nickname, 0, strlen(nickname) - 1));
	for (size_t i = 0; i < strlen(nickname) - 1; i++) { //-1 to cut off newline, but gives error when strlen = 0 (and should give error when strlen == 1 (empty name_))
		if (!is_valid_nickname_character(nickname[i])) {
			send_server_error(env, fd, ERR_ERRONEUSNICKNAME);
			return;
		}
	}

	for (int i = 0; i < env->active_clients; i++) {
		char *clientnick = env->clients[i]->nick;
		if (clientnick && strcmp(nickname, clientnick) == 0) {
			send_server_error(env, fd, ERR_NICKCOLLISION);
			return;
		}
	}
	s_user* client = get_client_by_fd(env, fd);
	client->nick = args[0];
	// send success response to client
	// #TODO improve
	char successmessage[10000];
	sprintf(successmessage, ":localhost 001 %s :Welcome to the Internet Relay Network %s!%s@localhost", nickname, nickname, client->user);
	dprintf(2, "Sending register_nickname success response: [%s]\n", successmessage);
	send(fd, successmessage, sizeof successmessage, 0);
}

// void register_user(s_env* env, int fd, char** args) {

// }

						// Now the server should respond to the NICK and USER messages. 
						/*
							The server should respond with one of the following responses:
							NICK response:
							NICKNAME in use: If the chosen nickname is already in use, 
								the server will typically respond with an error message like 
								433 ERR_NICKNAMEINUSE.
								Example: [:server-name 433 * chosen-nickname :Nickname is already in use.]
							Nickname changed: If the nickname change is successful, the server 
								will respond with a confirmation message, indicating that the 
								nickname has been changed.
								Example: [:server-name 001 your-nickname :Welcome to the IRC Network your-nickname!user@hostname]
							USER response: 
							USER Command: Similarly, when the client sends the USER command to register user information, 
								the server should acknowledge this by responding with a confirmation message.
								Example: [:server-name 001 your-nickname :Welcome to the IRC Network your-nickname!user@hostname
						*/
void parse_incoming_message(s_env* env, char* message, int fd, [[maybe_unused]]int nbytes) {
	static char* expected[] = {"CAP", "NICK"};// "USER", "MODE", "PING", "PRIVMSG", "WHOIS", "JOIN", "KICK", "QUIT" };
	size_t EXPECTED_LEN = 2;
	void (*functions[])(s_env*, int, char**) = {send_CAP_response, register_nickname}; //#TODO add rest of functions

	char** lines = split_lines(message);

	for (int i = 0; lines[i] != NULL; i++) {
		char *line = lines[i];
		size_t num_args = count_nonconsecutive_chars(line, ' ');
		char *command = get_next_token(line, " ");
		char **args = malloc(sizeof(char *) * num_args);
		char *arg;
		for (int i = 0; (arg = get_next_token(line, " ")) != NULL; i++) {
			args[i] = strdup(arg);
		}

		dprintf(2, "Parsing command: [%s] with args: ", command);
		for (size_t i = 0; i < num_args; i++) {
			if (i != num_args - 1)
				dprintf(2, "[%s] ", args[i]);
			else
				dprintf(2, "[%s]\n", args[i]);
		}

		for (size_t i = 0; i < EXPECTED_LEN; i++) {
			if (strcmp(expected[i], command) == 0) {
				functions[i](env, fd, args);
			}
		}

	// 	// free(line); Don't really want to bother with free, shouldn't be an issue in C++.
	// 	free(args);
	}
	// // free(lines);
}

void	read_fd(s_env* env)
{
	int		i;
	char 	buff[256];
	int		nbytes;

	i = 0;
	while (i <= env->fdmax)
	{
		if (FD_ISSET(i,&env->fdreads))
		{
			if (i == env->sockfd) {//Incoming request on master socket implies a new connection.
				dprintf(2, "Establishing new client connection\n");
				new_client(env);
			}
			else { //Incoming request elsewhere implies chat message or command.
				bzero(buff, 256);
				nbytes = recv(i, buff, sizeof buff, 0);
				if (nbytes <= 0)
				{
					close(i);
					FD_CLR(i,&env->master);
					dprintf(2, "Closed connection to client %d\n", i);
				}
				else {
					dprintf(2, "Incoming message: [%s] on fd %d\n", buff, i);
					parse_incoming_message(env, buff, i, nbytes);
					// send_all_client(env, nbytes, buff, i);
				}
			}
		}
		i++;
	}
}

int	main(void)
{
	struct addrinfo	hints;
	struct addrinfo	*res;
	s_env			env;

	env.server_address = "localhost";

	bzero(env.handshakes, 256 * sizeof(int));
	bzero(&hints, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo("127.0.0.1", "3490", &hints, &res);
	// getaddrinfo("10.11.1.11", "3490", &hints, &res);

	env.sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(env.sockfd, res->ai_addr, res->ai_addrlen);
	listen(env.sockfd, 10);
	FD_ZERO(&env.fdreads);
	FD_ZERO(&env.fdwrites);
	FD_ZERO(&env.master);
	FD_SET(env.sockfd, &env.master);
	env.fdmax = env.sockfd;

	env.clients = calloc(sizeof(s_user *), MAX_USERS);
	env.active_clients = 0;

	while (1)
	{
		env.fdreads = env.master;
		env.fdwrites = env.master;
		printf("============START OF ROUND==========\n");
		// struct timeval tv;
		// tv.tv_sec = 1;
		// tv.tv_usec = 0;
		select(env.fdmax + 1, &env.fdreads, NULL, NULL, NULL);
		read_fd(&env);
		printf("\n");
	}
}