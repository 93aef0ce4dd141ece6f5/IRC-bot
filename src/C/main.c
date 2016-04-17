/*
 *
 * to-do:
 * - multiple channels support
 * - sendMail function
 * - mySend function
 * - track 3 unauthorised attempts then add to ignore list
 *
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

 #include "main.h"

void error (char *str) {
    fprintf (stderr, "[!] %s\n", str);
    exit (EXIT_FAILURE);
}

void nonFatal (char *str, int type) {
    //#ifdef DEBUG
    if (type == WINDOWS) {
        int err = 0;
        if ((err = (int)GetLastError())) {
            fprintf (stderr, "[!] %s error: %d\n", str, err);
        }
    } else if (type == POSIX) {
        perror (str);
    }
    //#endif
}

void fatal (char *str, int type) {
    //#ifdef DEBUG
    if (type == WINDOWS) {
        int err = 0;
        if ((err = (int)GetLastError())) {
            fprintf (stderr, "[!] %s error: %d\n", str, err);
        }
    } else if (type == POSIX) {
        perror (str);
    }
    //#endif
    exit (EXIT_FAILURE);
}
void printUsage (char *prog) {
    fprintf (stderr, "Usage: %s -c [CHANNEL | CONTACT] -i [UNIQUE IDENTIFIER] -n [USERNAME] -p [PASSWORD]\n"
                    "\t-u (OPTIONAL) [USERNAME]\n"
                    "\t-n (OPTIONAL) [NICKNAME]\n"
                    "\t-p (OPTIONAL) [PASSWORD] : Required to authify your nickname\n"
                    "\t-c [CHANNEL] : Required joining channels\n"
                    //"\t-h [CHANNEL(S)] : Required to join channel(s)\n"
                    "\t-a (OPTIONAL) [ADMIN] : Required to run only under your credentials\n", prog);
    exit (EXIT_FAILURE);
}

int main (int argc, char *argv[]) {
    if (argc <= 1) {
        printUsage (argv[0]);
    }

    int opt, uflag = 0, pflag = 0;
    SOCKET s;

    srand (time (NULL)*MAX_NAME*MAX_MSG_SIZE);

    pAccount account = newAccount (1);

    while ((opt = getopt (argc, argv, "h:c:a:n:p:u:")) != -1) {
        switch (opt) {
            case 'c':
                account->contact = optarg;
                break;
            case 'a':
                account->admin = optarg;
                break;
            case 'u':
                uflag = 1;
                strncpy (account->u_name, optarg, MAX_NAME-1);
                break;
            case 'n':
                strncpy (account->n_name, optarg, MAX_NAME-1);
                break;
            case 'p':
                pflag = 1;
                account->pword = optarg;
                break;
            case 'h':
                fprintf (stderr, "Not yet implemented\n");
                break;
            default:
                printUsage (argv[0]);
        }
    }

    if (account->contact == NULL) {
        error ("Contact required");
    }

    if (account->admin == NULL) {
        account->admin = account->contact;
    }

    puts ("[*] Setting up connexion...");
    s = startClient ("irc.freenode.net", "6665");

    puts ("[*] Setting up IRC...");
    setupIRC (s, account);

    if (uflag && pflag) {
        printf ("[*] Authenticating \"%s\" with \"%s\"\n", account->u_name, account->pword);
        char *auth = malloc (strlen (account->pword) + 30);
        if (auth == NULL) {
            shutdown (s, SD_BOTH);
            closesocket (s);
            WSACleanup();
            fatal ("[!] Auth malloc", POSIX);
        }
        memset (auth, 0, strlen (account->pword) + 30);
        snprintf (auth, strlen (account->pword) + 30 - 1, "PRIVMSG NickServ :IDENTIFY %s\r\n", account->pword);
        send (s, auth, strlen (auth), 0);
        free (auth);
    }

    strToLower (account->u_name);
    strToLower (account->n_name);

    puts ("[*] Starting receiver...");
    startRecv (s, account);
    
    shutdown (s, SD_BOTH);
    closesocket (s);
    WSACleanup();
    free (account);

    return EXIT_SUCCESS;
}
