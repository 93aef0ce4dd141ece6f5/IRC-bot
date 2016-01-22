#ifndef _MAIN_H_
#define _MAIN_H_

#define MAX_MSG_SIZE 4096
#define DEFAULT_MALLOC_SIZE 2048
#define MAX_NAME 31

#define zeroMem(x) memset((x), 0, sizeof (x));
#define sleep(x) Sleep ((x)*1000);

typedef struct _account {
	char u_name[MAX_NAME];		// username
	char n_name[MAX_NAME];		// nickname
	char *admin;				// UNUSED
	char *pword;				// username's password
	char *contact;				// channel/privmsg
	//char **channels;			// channel list
} Account, *pAccount;

typedef struct _message {
	char n_name[MAX_NAME];		// nickname
	char *contact;				// channel/privmsg
	int contactSize;
	char *command;				// $ command
	int commandSize;
	char *param;				// $ command [paramter]
	int paramSize;
	char *msg;					// entire message
	int msgSize;
} Message, *pMessage;

enum {
	WINDOWS,
	POSIX
};

//main
void nonFatal (char *, int);
void fatal (char *, int);
void error (char *);

// initialise.c
pAccount newAccount (int);
SOCKET startClient (char *, char *);
int setupIRC (SOCKET, pAccount);
void getTime (char *);
pMessage newMessage (void);
void strToLower (char *);
// static void nullTerminate (char *s);
void formatMessage (pMessage);

//receive.c
void startRecv (SOCKET, pAccount);
//static void disect (pMessage, char *);
void cleanUp (SOCKET, FILE *, pAccount, pMessage);

//exec.c
void executeAdmin (SOCKET, FILE *, pAccount, pMessage);
int checkAuthorised (char *);
void denyExecute (SOCKET, pMessage);
void executeUser (SOCKET, pMessage, pAccount);
//static const char *getLink (char *p);
void executeDirect (SOCKET, pMessage);

#endif
