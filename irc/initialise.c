#define _WIN32_WINNT 0x501

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "main.h"

pAccount newAccount (int n) {
	pAccount a = malloc (sizeof (*a));
	if (a == NULL) {
		fatal ("[!] Account struct malloc", POSIX);
	}

	memset (a->u_name, 0, MAX_NAME);
	memset (a->n_name, 0, MAX_NAME);
	a->admin = NULL;
	a->pword = NULL;
	a->contact = NULL;
	/*
	a->channels = malloc (sizeof (a->channels) * n);

	int i;
	for (i = 0; i < n; i++) {
		a->channels[i] = NULL;
	}
	*/
	return a;
}

pMessage newMessage (void) {
	pMessage m = malloc (sizeof (*m));
	if (m == NULL) {
		fatal ("[!] Message struct malloc", POSIX);
	}
	
	memset (m->n_name, 0, MAX_NAME);

	m->contact = malloc (DEFAULT_MALLOC_SIZE);
	if (m->contact == NULL) {
		return NULL;
	}
	m->contactSize = DEFAULT_MALLOC_SIZE;

	m->command = malloc (DEFAULT_MALLOC_SIZE);
	if (m->command == NULL) {
		return NULL;
	}
	m->commandSize = DEFAULT_MALLOC_SIZE;

	m->param = malloc (DEFAULT_MALLOC_SIZE);
	if (m->param == NULL) {
		return NULL;
	}
	m->paramSize = DEFAULT_MALLOC_SIZE;

	m->msg = malloc (DEFAULT_MALLOC_SIZE);
	if (m->msg == NULL) {
		return NULL;
	}
	m->msgSize = DEFAULT_MALLOC_SIZE;

	return m;
}

SOCKET startClient (char *addr, char *port) {
    WSADATA wsa;
    SOCKET s;
    int gai;
    struct addrinfo hints, *res;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (WSAStartup (MAKEWORD (2, 2), &wsa)) {
        fatal ("WSAStartup", WINDOWS);
    }
    
    if ((gai = getaddrinfo (addr, port, &hints, &res))) {
        fprintf(stderr, "[!] Get info error: %s\n", gai_strerror (gai));
        exit (EXIT_FAILURE);
    }
    
    if ((s = socket (res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET) {
        fatal ("Socket", WINDOWS);
    }
    
    puts ("[*] Connecting...");
    if (connect (s, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR) {
        fatal ("Connect", WINDOWS);
    }
    
    return s;
}

int setupIRC (SOCKET s, pAccount account) {
    char setNick[strlen(account->n_name) + 9];
    zeroMem (setNick);
    char setUser[strlen(account->u_name) + 21];
    zeroMem (setUser);

    snprintf (setNick, sizeof (setNick) - 1, "NICK %s\r\n", account->n_name);
    snprintf (setUser, sizeof (setUser) - 1, "USER %s 0 * :dtm\r\n", account->u_name);

    if (send (s, setNick, strlen (setNick), 0) == SOCKET_ERROR) {
        fatal ("Send nickname", WINDOWS);
    }

    if (send (s, setUser, strlen (setUser), 0) == SOCKET_ERROR) {
        fatal ("Send username", WINDOWS);
    }

    return EXIT_SUCCESS;
}

void getTime (char *buf) {
    int i;
    time_t local_time = time (NULL);
    struct tm *timeinfo = malloc (sizeof (struct tm));

    time (&local_time);
    timeinfo = localtime (&local_time);
    strncpy (buf, asctime (timeinfo), 24);
    free (timeinfo);

    for (i = 0; i < strlen (buf); i++) {
        if (buf[i] == ' ') {
            buf[i] = '_';
        } else if (buf[i] == ':') {
            buf[i] = '-';
        }
    }
}

void strToLower (char *s) {
	if (s != NULL) {
		int i;
		
		for (i = 0; i < strlen (s); i++) {
			if (isalpha (s[i]) && isupper (s[i])) {
				s[i] = tolower (s[i]);
			}
		}
	}
}

static void nullTerminate (char *s) {
	if (s != NULL) {
		int i;

		for (i = 0; i < strlen (s); i++) {
			if (s[i] == '\r' || s[i] == '\n') {
				s[i] = '\0';
				break;
			}
		}
	}
}

void formatMessage (pMessage m) {
	strToLower (m->n_name);
	strToLower (m->contact);
	strToLower (m->command);
	strToLower (m->param);
	strToLower (m->msg);

	nullTerminate (m->n_name);
	nullTerminate (m->contact);
	nullTerminate (m->command);
	nullTerminate (m->param);
	nullTerminate (m->msg);
}
