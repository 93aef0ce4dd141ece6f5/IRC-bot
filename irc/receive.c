#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "main.h"

void cleanUp (SOCKET s, FILE *f, pAccount p, pMessage m) {
    shutdown (s, SD_BOTH);
    closesocket (s);
    WSACleanup();
    fclose (f);
    if (p != NULL) {
        free (p);
    }

    if (m != NULL) {
        free (m->contact);
        free (m->command);
        free (m->param);
        free (m->msg);
        free (m);
    }
}

static void disect (pMessage m, char *s) {
    char *token;
    const char delims[] = " !:";

    if ((token = strstr (s+1, ":")) != NULL) {
        if (strlen (token) > m->msgSize) {
            m->msg = realloc (m->msg, m->msgSize*2);
            m->msgSize *= 2;
        } else if ((strlen (token)*2) < m->msgSize && m->msgSize > DEFAULT_MALLOC_SIZE) {
            m->msg = realloc (m->msg, m->msgSize/2);
            m->msgSize /= 2;
        }
        strncpy (m->msg, token+1, m->msgSize-1);
    }

    if ((token = strtok (s+1, delims)) != NULL) {
        strncpy (m->n_name, token, MAX_NAME-1);
    }
    
    if ((token = strtok (NULL, delims)) != NULL) {
    }

    if ((token = strtok (NULL, delims)) != NULL) {
    }

    if ((token = strtok (NULL, delims)) != NULL) {
        if (strlen (token) > m->contactSize) {
            m->contact = realloc (m->contact, m->contactSize*2);
            m->contactSize *= 2;
        } else if ((strlen (token)*2) < m->contactSize && m->contactSize > DEFAULT_MALLOC_SIZE) {
            m->contact = realloc (m->contact, m->contactSize/2);
            m->contactSize /= 2;
        }
        strncpy (m->contact, token, m->contactSize-1);
    }

    if ((token = strtok (NULL, delims)) != NULL) {
        if (strlen (token) > m->commandSize) {
            m->command = realloc (m->command, m->commandSize*2);
            m->commandSize *= 2;
        } else if ((strlen (token)*2) < m->commandSize && m->commandSize > DEFAULT_MALLOC_SIZE) {
            m->command = realloc (m->command, m->commandSize/2);
            m->commandSize /= 2;
        }
        strncpy (m->command, token, m->commandSize-1);
    }

    if ((token = strtok (NULL, delims)) != NULL) {
        if (strlen (token) > m->paramSize) {
            m->param = realloc (m->param, m->paramSize*2);
            m->paramSize *= 2;
        } else if ((strlen (token)*2) < m->paramSize && m->paramSize > DEFAULT_MALLOC_SIZE) {
            m->param = realloc (m->param, m->paramSize/2);
            m->paramSize /= 2;
        }
        strncpy (m->param, token, m->paramSize-1);
    } else {
        memset (m->param, 0, m->paramSize);
    }
}

void startRecv (SOCKET s, pAccount account) {
    int recvStat, authed = 0;
    char output[MAX_MSG_SIZE];
	char date[50];
    FILE *file_log;

    //int err = 0;

    zeroMem (date);
    getTime (date);
    snprintf (output, sizeof (output) - 1, "log/%s_log_%s.txt", account->contact, date);
    if ((file_log = fopen (output, "w+")) == NULL) {
        perror ("Log File");
        fclose (file_log);
    }

    pMessage message = newMessage();
    if (message == NULL) {
        fatal ("Message struct failed", POSIX);
        cleanUp (s, file_log, account, message);
    }

    while (TRUE) {
        zeroMem (output);
        if ((recvStat = recv (s, output, sizeof (output), 0)) == SOCKET_ERROR) {
            nonFatal ("Receive", WINDOWS);
            ShellExecute (NULL, "open", "ircbot.bat", NULL, NULL, SW_SHOWNORMAL);
            cleanUp (s, file_log, account, message);
            exit (EXIT_FAILURE);
        } else if (recvStat == 0) {
            fprintf (stderr, "Connection closed by server\n");
            cleanUp (s, file_log, account, message);
            ShellExecute (NULL, "open", "ircbot.bat", NULL, NULL, SW_SHOWNORMAL);
            exit (EXIT_SUCCESS);
        }

        if (!strncmp (output, "PING", 4)) {
            output[1] = 'O';
            send (s, output, strlen (output), 0);
        } else {
            fprintf (file_log, "%s", output);
            fseek (file_log, 0, SEEK_END);
            printf ("%s", output);
            // authenticate before extracting messages
            if (!authed) {
                char setContact[strlen(account->contact)+70];
                if (strstr (output, "You are now identified") != NULL) {
                    if (strstr (account->contact, "#") != NULL) {
                        snprintf (setContact, strlen (account->contact) + 7, "JOIN %s\r\n", account->contact);
                    } else {
                        snprintf (setContact, strlen (account->contact) + strlen (account->n_name) + 26, "PRIVMSG %s :%c3%s reporting.%c\r\n", account->contact, 3, account->n_name, 3);
                    }
                    if (send (s, setContact, strlen (setContact), 0) == SOCKET_ERROR) {
                        cleanUp (s, file_log, account, message);
                        fatal ("Send contact", WINDOWS);
                    }
                    authed = 1;
                }
            // normal receive
            } else {
                // extract elements from messages
                disect (message, output);
                formatMessage (message);

                // user has requested command
                if (!strncmp (message->command, "$", 1)) {
                    // check if authorised
                    if (checkAuthorised (message->n_name)) {
                        // execute command
                        executeAdmin (s, file_log, account, message);
                    } else {
                        // log unauthorised attempts
                        fprintf (file_log, "[!] Illegal Access: [%s]\n\n", message->n_name);
                        // send denial message
                        denyExecute (s, message);
                    }
                } else if (!strncmp (message->command, "~", 1)) {
                    // check if authorised
                    if (checkAuthorised (message->n_name)) {
                        // execute command
                        executeDirect (s, message);
                    } else {
                        // log unauthorised attempts
                        fprintf (file_log, "[!] Illegal Access: [%s]\n\n", message->n_name);
                        // send denial message
                        denyExecute (s, message);
                    }
                } else if (!strncmp (message->command, "@", 1)) {
                    executeUser (s, message, account);
                }
            }
        }
    }

    cleanUp (s, file_log, account, message);
}
