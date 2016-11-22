# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>

# include <pthread.h>
# include <string.h>

# include "server.h"
# include "bank.h"
# include "interface.h"
//# include "signals.h"

int server_run() {

	struct addrinfo addrinfo;
	struct addrinfo * result;
	int sd;
	char message [256];
	int on = 1;

	gethostname(message, 256);
	printf("Bank hosted on [%s]\n", message);
	bzero(message,256);

	struct addrinfo request;
	request.ai_flags = AI_PASSIVE;
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
	if ( getaddrinfo ( 0, "8542", &request, &result) != 0) {

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

	} else if ( errno = 0, bind ( sd, result->ai_addr, result->ai_addrlen ) == -1) {
		//	( errno = 0, bind ( sd, result->ai_addr, 
		//			result->ai_addrlen ) == -1) {

		printf("bind epic fail\n");
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;

	} else if ( errno = 0, listen ( sd, 100 ) == -1 ) {

		printf("listen epic fail\n");
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;

	} 

	// set up bank

	b = &bk;
	setup_bank();

	// set up timer to print bank
	// spawn thread specifcally to wait for timer
	pthread_t ansaphone;
	pthread_attr_t ansaphone_attr;

	pthread_attr_init( &ansaphone_attr );
	pthread_attr_setscope( &ansaphone_attr, PTHREAD_SCOPE_SYSTEM );

	unsigned int * t = (unsigned int *)malloc(sizeof(unsigned int));

	if( t == NULL) {
		printf("malloc failed!\n");
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	*t = 20;

	pthread_create( &ansaphone, &ansaphone_attr, timer, t);

	pthread_detach( ansaphone );


	while (1) {

//		struct bank_socket * bs = (struct bank_socket *) malloc (sizeof(struct bank_socket));
//		bs->bptr = b;

		int * fd = (int *)malloc(sizeof(int));

		*fd = accept(sd, (struct sockaddr *) &senderaddr, &ic);

		// spawn new thread for each 

		pthread_attr_init( &ansaphone_attr );
		pthread_attr_setscope( &ansaphone_attr, PTHREAD_SCOPE_SYSTEM );

		pthread_create( &ansaphone, &ansaphone_attr, gen_interface, fd );

		pthread_detach( ansaphone );

//		break;
		// do not need to close socket here - same process still!

	}

}

void setup_bank() {

	b->count = 0;

	int			i;
	account * 		acc;
	pthread_mutex_t	*	mut;

	for(i = 0; i < 20; i++) {
		acc = &(b->account_array[i]);
		acc->in_session = 0;
		acc->in_use = 0;

		mut = &b->account_mutex[i];
		pthread_mutex_init( mut, 0 );
	}

	mut = &b->bank_mutex;
	pthread_mutex_init( mut, 0 );

	return;
}

void * timer (void * times) {

	unsigned int seconds = *(unsigned int *)times;
	free(times);

	while(1) {
		print_bank();
		sleep(seconds);
	}

	return NULL;
}

//void print_bank (struct bank * b) {
void print_bank () {

	// freeze creation of new accounts
	while ( pthread_mutex_trylock(&(b->bank_mutex)) );

	int i;
	int try;
	account * acc;

	printf("\nPrinting bank ... %d open accounts\n", b->count);
	printf("NUM\tACCOUNT NAME\tBALANCE\tIN USE?\n");
	for (i = 0; i < 20; i++ ) {
		acc = &b->account_array[i];

		// print blank if not in use
		if ( ! acc->in_use ) {
			printf("%d\t<empty>\t\t<0>\t\t<N/A>\n", i);
			continue;
		}

		printf("%d\t", i);
		printf("%s\t", acc->accountname);
		printf("%f\t", acc->balance);

//		try = trylock(
		if (acc->in_session) {
			printf("IN SERVICE\n");
		} else {
			printf("\n");
		}
	}

	pthread_mutex_unlock(&b->bank_mutex);

}
