# ifndef BANK_H
# define BANK_H

# include <stdlib.h>
# include <pthread.h>

struct bank_account {

//typedef struct {
	char	accountname[101];
		// 100 characters + null terminator
	float	balance;
		// initialized to 0
	int	in_session;
		// 0 when not 'logged in', 1 when 'logged in'
		// get rid? use mutex array to track?
	int	in_use;
		// 0 when no account associated with spot
};

typedef struct bank_account account;

struct bank {

	int		count;
			// number of currently active accounts
	account		account_array[20];
			// the 20 available accounts
	pthread_mutex_t	bank_mutex;
			// mutex for the entire bank
			// printing vs create new account
	pthread_mutex_t	account_mutex[20];
			// the mutexes for the 20 accounts
			// individual access
};

struct bank bk;
struct bank * b;

struct timer_struct {

	unsigned int	seconds;
	void * (*function_call) (void * obj);

};

# endif
