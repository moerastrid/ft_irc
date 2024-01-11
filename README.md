# ft_irc
Now we're talking! Internet real chat project of 42 / codam, made by tnuyten and ageels. Several IRC clients exist, we used IRSSI as a reference. 



To start the server, make the program and run "./ircserv [port] [password]".

* Port: The port number on which our IRC server will be listening to for incoming IRC connections.
* Password: The connection password. It will be needed by any IRC client that tries to connect to our server.



Our IRC server has the following features implemented:

	* KICK - Eject a client from the channel
 
	* INVITE - Invite a client to a channel
 
	* TOPIC - Change or view the channel topic
 
	* MODE - Change the channel's mode:
		- i: Set/remove Invite-only channel
		- t: Set/remove the restrictions of the TOPIC command to channel operators
		- k: Set/remove the channel key (password)
		- o: Give/take channel operator privilege
		- l: Set/remote the user limit to channel



