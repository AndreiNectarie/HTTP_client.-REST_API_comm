#ifndef _HELPERS_
#define _HELPERS_

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <nlohmann/json.hpp>


#define SERVER_IP "34.246.184.49"
#define SERVER_PORT 8080
#define HOST "34.246.184.49:8080"
#define BUFLEN 1600
#define LINELEN 1024
#define JSON "application/json"
#define REGISTER_API "/api/v1/tema/auth/register"
#define LOGIN_API "/api/v1/tema/auth/login"
#define LIBRARY_API "/api/v1/tema/library/access"
#define BOOKS_API "/api/v1/tema/library/books"
#define LOGOUT_API "/api/v1/tema/auth/logout"


#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

// starts a connection with the server
int start_connection();

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

#endif
