#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "stolen.h"

#define MAX_USERS 100

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

void	send_to_client(s_env* env, int i, char* buff)
{
	
}

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
				// for (int k = 0; k < env->active_clients; k++) {
				// 	if (env->clients[k]->fd == i) {
				// 		username = env->clients[k]->name;
				// 	}
				// }
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
	char	welcom[] = "Welcome to the Internet Relay Network nick!user@host\n";
	char	userprompt[] = "Please enter your username: ";

	//Accept the connection and save the socket FD and more. 
	env->addr_size = sizeof env->their_addr;
	newfd = accept(env->sockfd, (struct sockaddr *)&env->their_addr, &env->addr_size);
	dprintf(2, "Accepted connection on fd %d\n", newfd);
	if (newfd > env->fdmax)
		env->fdmax = newfd;
	FD_SET(newfd, &env->master);

	// Original, receives a message from the client and prints it on the server. 
		// char 	buff[1024]; bzero(buff, 1024);
		// unsigned int nbytes = recv(newfd, buff, sizeof buff, 0);
		// printf("Message received from client: [%s] (%d bytes)\n", buff, nbytes);


	// Receive a first message from the client, which we set as their username. Max length 30. Put in while loop to demand a non-empty name. 
	// char	username[30]; bzero(username, 30);
	// s_user* user = calloc(sizeof (s_user), 1);
	// send(newfd, userprompt, strlen(userprompt), 0);
	// unsigned int nbytes = recv(newfd, username, sizeof username, 0); // BLOCKS (still)
	// username[nbytes > 0 ? nbytes - 1 : nbytes] = '\0'; // Remove newline from username
	// printf("Username received from client: [%s] (%d bytes)\n", username, nbytes); //Had het idee om hier de username te vragen, voor nu.
	
	// Save the user's data in the list of clients in env.
	// user->name = calloc(nbytes, sizeof(char));
	// user->fd = newfd;
	// strcpy(user->name, username);
	// printf("user->name: %s\n", user->name);
	// env->clients[env->active_clients++] = user;

	//Send some messages to the new client.
	send(newfd, welcom, sizeof welcom, 0);
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
				if(nbytes <= 0)
				{
					close(i);
					FD_CLR(i,&env->master);
					dprintf(2, "Closed connection to client %d", i);
				}
				else {
					printf("Incoming message: [%s]\n", buff);
					if (env->handshakes[i] == 0) {
						char message[] = ":localhost CAP NAK :-";
						dprintf(2, "Sending CAP response: [%s]\n", message);
						send(i, message, sizeof message, 0);
						env->handshakes[i]++;

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
]
						*/
					} else  {
						send_all_client(env, nbytes, buff, i);
					}
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
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		select(env.fdmax + 1, &env.fdreads, NULL, NULL, NULL);
		read_fd(&env);
		printf("\n");
	}
}