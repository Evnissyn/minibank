# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>

# include "interface.h"
# include "bank.h"

void * gen_interface(void * fd) {

// for some reason docs use void* -- is this important??

	int 		sd = *(int *)fd;

	free(fd);

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	char		command[8];
	char *		accountname = (char *)malloc(101);
	float *		amount = (float *)malloc(sizeof(float));
	int		accountnum = -1;

//	account *	acc = NULL;


	message_len = sprintf(out_message, "Welcome to the Desert Sands Banking Platform\nWhat would you like to do today?\nWould you like to\n\t[open] a new account?\n\t[start] a session with a preexisting account?\n");

//	sprintf(out_message, "Welcome to the Desert Sands Banking Platform\n
//			What would you like to do today?\n
//			Would you like to\n
//			\t[open] a new account?\n
//			\t[start] a session with a preexisting account?\n");

	write(sd, out_message, message_len);

	while (1) {

		//scanf("%7s", &command);
		bzero(in_message, 200);
		if ( !read(sd, in_message, 199) ) break;
		sscanf(in_message, "%s", command);

		printf("INCOMING MESSAGE: [%s]\n", in_message);

		if ( ! strcmp(command, "open") ) {
			if (accountnum != -1) {
				message_len = sprintf(out_message, "\tYou are currently in a customer session\n");
				write(sd, out_message, message_len);
				continue;
			}

			//scanf("%100s", accountname);
			//sscanf(in_message, "%s %s", command, accountname);

			bzero(in_message, 200);
			if ( !read(sd, in_message, 199) ) break;
			sscanf(in_message, "%s", accountname);

	//		message_len = sprintf(out_message, "opening...[%s]\n", accountname);
	//		write(0, out_message, message_len);

			accountnum = open(accountname, sd);

			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tOpen new account failed\n");
				write(sd, out_message, message_len);
				continue;
			}

	//		acc = &b->account_array[accountnum];

		} else if ( ! strcmp(command, "start") ) {
			if (accountnum != -1) {
				message_len = sprintf(out_message, "\tYou are currently in a customer session\n");	
				write(sd, out_message, message_len);
				continue;
			}

	//		scanf("%100s", accountname);
	//		sscanf(in_message, "%s %s", command, accountname);

			bzero(in_message, 200);
			if ( !read(sd, in_message, 199) ) break;
			sscanf(in_message, "%s", accountname);

			accountnum = start(accountname, sd);

			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tStarting session with preexisting account failed\n");
				write(sd, out_message, message_len);
				continue;
			}

	//		acc = &b->account_array[accountnum];

		} else if ( ! strcmp(command, "credit") ) {
			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tYou are not currently in a customer session\n");	
				write(sd, out_message, message_len);
				continue;
			}

	//		scanf("%f", amount);
	//		sscanf(in_message, "%s %f", command, amount);

			bzero(in_message, 200);
			if ( !read(sd, in_message, 199) ) break;

	//		message_len = sprintf(out_message, "message: [%s]\n", in_message);

	//		write(sd, out_message, message_len);

			sscanf(in_message, "%f", amount);

	//		message_len = sprintf(out_message, "message: [%s]\nread: [%f]\n", in_message, *amount);
	//		write(sd, out_message, message_len);

			credit(accountnum, amount, sd);

		} else if ( ! strcmp(command, "debit") ) {
			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tYou are not currently in a customer session\n");	
				write(sd, out_message, message_len);
				continue;
			}

	//		scanf("%f", amount);
	//		sscanf(in_message, "%s %f", command, amount);

			bzero(in_message, 200);
			if ( !read(sd, in_message, 199) ) break;
			sscanf(in_message, "%f", amount);

			debit(accountnum, amount, sd);

		} else if ( ! strcmp(command, "balance") ) {
			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tYou are not currently in a customer session\n");	
				write(sd, out_message, message_len);
				continue;
			}

			balance(accountnum, sd);

		} else if ( ! strcmp(command, "finish") ) {
			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tYou are not currently in a customer session\n");	
				write(sd, out_message, message_len);
				continue;
			}

			finish(accountnum, sd);
	//		acc = NULL;
			accountnum = -1;

		} else if ( ! strcmp(command, "close") ) {
			if (accountnum == -1) {
				message_len = sprintf(out_message, "\tYou are not currently in a customer session\n");	
				write(sd, out_message, message_len);
				continue;
			}

			close_account(accountnum, sd);
			accountnum = -1;

		} else if ( ! strcmp(command, "exit") ) {
			break;

		} else {
			message_len = sprintf(out_message, "\tInvalid command\n");
			write(sd, out_message, message_len);
			continue;

		}

	}

	close(sd);
	return 0;

}

// opens new account
// returns account number (0 to 19)
// returns -1 if open new account fails for any reason
int open (char * accountname, int sd) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len = -1;

	// check if bank is full

	if (19 < b->count) {
		message_len = sprintf(out_message, "\tbank is full");
		write(sd, out_message, message_len);
		return -1;
	}

	// ??? while loop or mutex to open account if bank is printing
	// mutex on entire bank 
	// (for opening account / printing info purposes)

	pthread_mutex_lock(&b->bank_mutex);

	// check if account exists
	// holds accountnum of first free account

	int i;
	account * acc;

	for (i = 0; i < 20; i++) {
		acc = &b->account_array[i];

		if ( ! strncmp( acc->accountname, accountname, 100 ) ) {
			message_len = sprintf(out_message, "\tAccount already exists\n");
			write(sd, out_message, message_len);
			return -1;
		}

		if ( ( message_len == -1 ) && ( ! acc->in_use ) ) {
			message_len = i;
		}
	}

	// if you get to this point, message_len holds 
	// accountnum of first free account

	i = message_len; 

	acc = &b->account_array[i];

	// account mutex lock
	pthread_mutex_lock(&b->account_mutex[i]);

	strncpy(acc->accountname, accountname, 100);
	acc->accountname[100] = '\0';// set null terminator jic

	acc->balance = 0;
	acc->in_session = 1; // open account means logged into account
	acc->in_use = 1;

	b->count++;	// increase count of open accounts

	// bank mutex unlock
	pthread_mutex_unlock(&b->bank_mutex);

	return i;
}

// returns account number (0 to 19)
// returns -1 if start account fails for any reason
int start ( char * accountname, int sd ) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	// check if account exists
	// mutex lock account
	// return account number

	int i;
	account * acc;

	for (i = 0; i < 20; i++) {
		acc = &b->account_array[i];
		if ( ! strncmp( acc->accountname, accountname, 100 ) ) {
			// if strncmp = 0, same name

			// account mutex lock
			pthread_mutex_lock(&b->account_mutex[i]);
			b->account_array->in_session = 1;

			return i;

		}

	}

	return -1;
}

// return -1 if credit fail
int credit ( int accountnum, float * amount, int sd ) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	if (accountnum == -1) {
		message_len = sprintf(out_message, "\tNot in customer session\n");
		write(sd, out_message, message_len);
		return -1;
	}

//	message_len = sprintf(out_message, "crediting %f\n", *amount);
//	write(sd, out_message, message_len);

	account * acc = &b->account_array[accountnum];

	acc->balance += *amount;

	return 1;
}

int debit ( int accountnum, float * amount, int sd ) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	if (accountnum == -1) {
		message_len = sprintf(out_message, "\tNot in customer session\n");
		write(sd, out_message, message_len);
		return -1;
	}

	account * acc = &b->account_array[accountnum];

	if (acc->balance < *amount) {
		message_len = sprintf(out_message, "\tInvalid transaction: not enough funds\n");
		write(sd, out_message, message_len);
		return -1;
	}

	acc->balance -= *amount;
	return 1;
}

int balance ( int accountnum, int sd ) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	if (accountnum == -1) {
		message_len = sprintf(out_message, "\tNot in customer session\n");
		write(sd, out_message, message_len);
		return -1;
	}

	account * acc = &b->account_array[accountnum];

	message_len = sprintf(out_message, "\tBALANCE: %f\n", acc->balance);
	write(sd, out_message, message_len);

	return 1;
}

int finish ( int accountnum, int sd ) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	if (accountnum == -1) {
		message_len = sprintf(out_message, "\tNot in customer session\n");
		write(sd, out_message, message_len);
		return -1;
	}

	account * acc = &b->account_array[accountnum];

	acc->in_session = 0;

	// account mutex unlock
	pthread_mutex_unlock(&b->account_mutex[accountnum]);


	return 1;

}

int close_account ( int accountnum, int sd) {

	char 		out_message[200];
	char 		in_message[200];
	int		message_len;

	if (accountnum == -1) {
		message_len = sprintf(out_message, "\tNot in customer session\n");
		write(sd, out_message, message_len);
		return -1;
	}

	account * acc = &b->account_array[accountnum];

	// re-initialize account
	bzero(acc->accountname, 100);
	acc->balance = 0;
	acc->in_session = 0;
	acc->in_use = 0;

	// bank's total accounts decrease
	b->count--;

	// account mutex unlock
	pthread_mutex_unlock(&b->account_mutex[accountnum]);


	return 1;
	

}
