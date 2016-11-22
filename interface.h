# include "bank.h"

# ifndef INTERFACE_H
# define INTERFACE_H

void * gen_interface(void * fd);
int open(char * accountname, int sd);
int start(char * accountname, int sd);
int credit(int accoutnum, float *amount, int sd);
int debit(int accoutnum, float *amount, int sd);
int balance(int accountnum, int sd);
int finish (int accountnum, int sd);
int close_account (int accountnum, int sd);

# endif
