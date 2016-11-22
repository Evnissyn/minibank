# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>

# include <pthread.h>
# include <string.h>

# include "client.h"

//int client_run(char * machine) {

int main(int argc, char ** argv){

	if (argc < 2) {
		printf("Need more, try again\n");
		return 0;
	}

	struct addrinfo addrinfo;
	struct addrinfo * result;
	int sd;
	char message [256];
	int on = 1;

	struct addrinfo request;
	request.ai_flags = 0;
	request.ai_family = AF_INET;
	request.ai_socktype = SOCK_STREAM;
	request.ai_protocol = 0;
	request.ai_addrlen = 0;
	request.ai_addr = NULL;
	request.ai_canonname = NULL;
	request.ai_next = NULL;

	struct sockaddr_in senderaddr;
	socklen_t ic;


//	if ( getaddrinfo ( 0, port, &addrinfo, &result) != 0) {
	if ( getaddrinfo ( argv[1], "8542", &request, &result) != 0) {
		// [name of machine you are connecting to], [port number]

		printf("getaddrinfo epic fail\n");
		printf("error: %d\n", errno);
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;

	} else if (errno = 0, (sd = socket ( result->ai_family, result->ai_socktype, result->ai_protocol) ) == -1) {
		//	(errno = 0, (sd = socket ( result->ai_family, 
		//				result->ai_socktype, 
		//				result->ai_protocol) ) == -1) {

		printf("socket epic fail\n");
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;

	} else if ( errno = 0, setsockopt ( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1) {
		//	( setsockopt ( sd, SOL_SOCKET, SO_REUSEADDR, 
		//			&on, sizeof(on) ) == -1) {

		printf("setsockopt epic fail\n");
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;

//	} else if ( errno = 0, bind ( sd, result->ai_addr, result->ai_addrlen ) == -1) {
		//	( errno = 0, bind ( sd, result->ai_addr, 
		//			result->ai_addrlen ) == -1) {

		printf("bind epic fail\n");
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;

	} 


	while (errno = 0, (connect (sd, result->ai_addr, result->ai_addrlen) ) == -1);

	// spawn thread to read from server / write to user
	// this thread used to read from user / write to server

	pthread_t 	from_server;
	pthread_attr_t 	from_server_attr;

	pthread_attr_init (&from_server_attr);
	pthread_attr_setscope (&from_server_attr, PTHREAD_SCOPE_SYSTEM);

	int * sdmall = (int *)malloc(sizeof(int));
	*sdmall = sd;

	pthread_create(&from_server, &from_server_attr, read_server, sdmall);
	pthread_detach(from_server);

	client_timer(sd);

}

void * client_timer(int sd) {

	char	out_message[200];
	int	message_len = 0;

	while (1) {

		sleep(2);
		bzero(out_message, 200);
		scanf("%199s", out_message);
		message_len = strlen(out_message);
		write(sd, out_message, message_len);
	}

	return NULL;
}

void * read_server(void * obj) {

//	sleep(100);

	int sd = *(int *)obj;
	free(obj);

	char	in_message[200];
	int	message_len;

	while (1) {

		bzero(in_message, 200);
		read(sd, in_message, 199);
//		printf("%s\n", in_message);

		write(0, in_message, strlen(in_message));
	}

	return NULL;
}
