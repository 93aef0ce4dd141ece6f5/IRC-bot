#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "main.h"

const char *admins[] = {};

const char *adminCommands[] = {"$help", "$restart", "$die"/*, "$leave", "$join", "$ghost", "$nick"*/};
const char *adminCommandsDesc[] = {"Displays commands", "Restart bot", "Shutdown bot"/*, "Leave <OPTIONAL CHANNEL>", "Join <CHANNEL>"*/};

/*
const char *userCommands[] = {"$link*", "$help"};
const char *userCommandsDesc[] = {"Prints link", "Displays commands"};

const char *links[] = {"https://youtube.com/", "http://null-byte.wonderhowto.com/"};
const char *linkShort[] = {"yt", "nb"};
const char *linkDesc[] = {"Displays YouTube link", "Displays Null-Byte link"};
*/

int checkAuthorised (char *s) {
    int i;
    for (i = 0; i < sizeof (admins)/sizeof (*admins); i++) {
        if (!strncmp (s, admins[i], strlen (s))) {
            return TRUE;
        }
    }

    return FALSE;
}

void executeAdmin (SOCKET s, FILE *f, pAccount a, pMessage m) {
    char output[MAX_MSG_SIZE];
    zeroMem (output);

    const char *contact = m->contact;
    if (!strncmp (m->contact, a->n_name, strlen (a->n_name))) {
        contact = m->n_name;
    }

    if (!strncmp (m->command, "$help", 5)) {
        int i;
        snprintf (output, sizeof (output), "PRIVMSG %s :%c3Admin commands:%c\r\n", m->n_name, 3, 3);
        send (s, output, strlen (output), 0);
        for (i = 0; i < sizeof (adminCommands)/sizeof (*adminCommands); i++) {
            snprintf (output, sizeof (output), "PRIVMSG %s :%c3\t%s\t-\t%s%c\r\n", m->n_name, 3, adminCommands[i], adminCommandsDesc[i], 3);
            send (s, output, strlen (output), 0);
        }
        return;
    }

    if (!strncmp (m->command, "$restart", 8)) {
        snprintf (output, sizeof (output), "PRIVMSG %s :%c3Restarting...%c\r\n", contact, 3, 3);
        send (s, output, strlen (output), 0);
        snprintf (output, sizeof (output), "QUIT :Manual restart\r\n");
        send (s, output, strlen (output), 0);
        cleanUp (s, f, a, m);
        ShellExecute (NULL, "open", "ircbot.bat", NULL, NULL, SW_SHOWNORMAL);
    } else if (!strncmp (m->command, "$die", 4)) {
        snprintf (output, sizeof (output), "PRIVMSG %s :%c3Dying...%c\r\n", contact, 3, 3);
        send (s, output, strlen (output), 0);
        snprintf (output, sizeof (output), "QUIT :Died\r\n");
        send (s, output, strlen (output), 0);
        cleanUp (s, f, a, m);
    }

    exit (EXIT_SUCCESS);
}

void executeDirect (SOCKET s, pMessage m) {
    char output[MAX_MSG_SIZE];
    char received[MAX_MSG_SIZE];
    zeroMem (output);
    zeroMem (received);

    if (strlen (m->msg)+2 < m->msgSize) {
        strncat (m->msg, "\r\n", 2);
    } else {
        snprintf (output, sizeof (output), "PRIVMSG %s :%c4Failed to set msg array%c\r\n", m->n_name, 3, 3);
        send (s, output, strlen (output), 0);
        return;
    }

    send (s, m->msg+1, strlen (m->msg)-1, 0);
    recv (s, received, sizeof (received), 0);   
    snprintf (output, sizeof (output), "PRIVMSG %s :%c3%s%c\r\n", m->n_name, 3, received, 3);
    send (s, output, strlen (output), 0);
}

void denyExecute (SOCKET s, pMessage m) {
    char output[MAX_MSG_SIZE];

    snprintf (output, sizeof (output), "PRIVMSG %s :%c0,4Unauthorised access has been logged: YOU HAVE NO POWER HERE!%c\r\n", m->n_name, 3, 3);
    send (s, output, strlen (output), 0);
}

void executeUser (SOCKET s, pMessage m, pAccount a) {
    char output[MAX_MSG_SIZE];
    zeroMem (output);

    const char *contact = m->contact;
    if (!strncmp (m->contact, a->n_name, strlen (a->n_name))) {
        contact = m->n_name;
    }

    if (!strncmp (m->command, "@search", 7)) {
        char *baselink = "http://null-byte.wonderhowto.com/search/";

        int i;
        for (i = 0; i < strlen (m->msg); i++) {
            if (m->msg[i] == ' ') {
                m->msg[i] = '-';
            }
        }

        snprintf (output, sizeof (output), "PRIVMSG %s :%c3%s%s%c\r\n", contact, 3, baselink, m->msg+8, 3);
        send (s, output, strlen (output), 0);
    } else if (!strncmp (m->command, "@poke", 5)) {
        if (strlen (m->param) != 0) {
            if (!strncmp (m->param, a->n_name, strlen (a->n_name))) {
                snprintf (output, sizeof (output), "PRIVMSG %s :%c3Ouch!%c\r\n", contact, 3, 3);
                send (s, output, strlen (output), 0);
            } else {
                snprintf (output, sizeof (output), "PRIVMSG %s :%cACTION poked %s.%c\r\n", contact, 1, m->param, 1);
                send (s, output, strlen (output), 0);
            }
        }
    } else if (!strncmp (m->command, "@github", 7)) {
        char *baselink = "https://github.com/search?q=";

        int i;
        for (i = 0; i < strlen (m->msg); i++) {
            if (m->msg[i] == ' ') {
                m->msg[i] = '+';
            }
        }

        snprintf (output, sizeof (output), "PRIVMSG %s :%c3%s%s%c\r\n", contact, 3, baselink, m->msg+8, 3);
        send (s, output, strlen (output), 0);
    }
}
