#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "main.h"

const char *admins[] = {/* admins here */};

const char *adminCommands[] = {"$help", "$restart", "$die"/*, "$leave", "$join", "$ghost", "$nick"*/};
const char *adminCommandsDesc[] = {"Displays commands", "Restart bot", "Shutdown bot"/*, "Leave <OPTIONAL CHANNEL>", "Join <CHANNEL>"*/};

int my_send (SOCKET s, const char *fmt, ...) {
    char buf[MAX_MSG_SIZE];
    va_list send_args;
    va_start (send_args, fmt);

    vsnprintf (buf, sizeof (buf), fmt, send_args);
    va_end (send_args);

    return send (s, buf, strlen (buf), 0);
}

int checkAuthorised (char *s) {
    int i;
    for (i = 0; i < sizeof (admins)/sizeof (*admins); i++) {
        if (!strncmp (s, admins[i], strlen (s))) {
            return TRUE;
        }
    }

    return FALSE;
}

void executeAdmin (SOCKET s, FILE *f, pAccount a, pMessage m) {const char *contact = m->contact;
    if (!strncmp (m->contact, a->n_name, strlen (a->n_name))) {
        contact = m->n_name;
    }

    if (!strncmp (m->command, "$help", 5)) {
        int i;
        my_send (s, "PRIVMSG %s :%c3Admin commands:%c\r\n", m->n_name, 3, 3);
        for (i = 0; i < sizeof (adminCommands)/sizeof (*adminCommands); i++) {
            my_send (s, "PRIVMSG %s :%c3\t%s\t-\t%s%c\r\n", m->n_name, 3, adminCommands[i], adminCommandsDesc[i], 3);
        }
        return;
    }

    if (!strncmp (m->command, "$restart", 8)) {
        my_send (s, "PRIVMSG %s :%c3Restarting...%c\r\n", contact, 3, 3);
        my_send (s, "QUIT :Manual restart\r\n");
        cleanUp (s, f, a, m);
        ShellExecute (NULL, "open", "ircbot.bat", NULL, NULL, SW_SHOWNORMAL);
        exit (EXIT_SUCCESS);
    } else if (!strncmp (m->command, "$die", 4)) {
        my_send (s, "PRIVMSG %s :%c3Dying...%c\r\n", contact, 3, 3);
        my_send (s, "QUIT :Died\r\n");
        cleanUp (s, f, a, m);
        exit (EXIT_SUCCESS);
    }

}

void executeDirect (SOCKET s, pMessage m) {
    char received[MAX_MSG_SIZE];
    zeroMem (received);

    if (strlen (m->msg)+2 < m->msgSize) {
        strncat (m->msg, "\r\n", 2);
    } else {
        my_send (s, "PRIVMSG %s :%c4Failed to set msg array%c\r\n", m->n_name, 3, 3);
        return;
    }

    send (s, m->msg+1, strlen (m->msg)-1, 0);
    recv (s, received, sizeof (received), 0);   
    my_send (s, "PRIVMSG %s :%c3%s%c\r\n", m->n_name, 3, received, 3);
}

void denyExecute (SOCKET s, pMessage m) {my_send (s, "PRIVMSG %s :%c0,4Unauthorised access has been logged: YOU HAVE NO POWER HERE!%c\r\n", m->n_name, 3, 3);
}

void executeUser (SOCKET s, pMessage m, pAccount a) {
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

        my_send (s, "PRIVMSG %s :%c3%s%s%c\r\n", contact, 3, baselink, m->msg+8, 3);
    } else if (!strncmp (m->command, "@user", 5)) {
        char *baselink = "http://null-byte.wonderhowto.com/community/search/";

        int i;
        for (i = 0; i < strlen (m->msg); i++) {
            if (m->msg[i] == ' ') {
                m->msg[i] = '-';
            }
        }

        my_send (s, "PRIVMSG %s :%c3%s%s%c\r\n", contact, 3, baselink, m->msg+6, 3);
    } else if (!strncmp (m->command, "@poke", 5)) {
        if (strlen (m->param) != 0) {
            if (!strncmp (m->param, a->n_name, strlen (a->n_name))) {
                my_send (s, "PRIVMSG %s :%c3Ouch!%c\r\n", contact, 3, 3);
            } else {
                my_send (s, "PRIVMSG %s :%cACTION poked %s.%c\r\n", contact, 1, m->param, 1);
            }
        }
    } else if (!strncmp (m->command, "@slap", 5)) {
        if (strlen (m->param) != 0) {
            if (!strncmp (m->param, a->n_name, strlen (a->n_name))) {
                my_send (s, "PRIVMSG %s :%c3Ouch!%c\r\n", contact, 3, 3);
            } else {
                my_send (s, "PRIVMSG %s :%cACTION slapped %s with a frozen tuna.%c\r\n", contact, 1, m->msg+strlen (m->command)+1, 1);
            }
        }
    } else if (!strcmp (m->command, "@punch")) {
        char *list[] = {"gut", "face", "jaw", "ribs", "throat", "family jewels"};
        if (strlen (m->param) != 0) {
            if (!strncmp (m->param, a->n_name, strlen (a->n_name))) {
                my_send (s, "PRIVMSG %s :%c3Ouch!%c\r\n", contact, 3, 3);
            } else {
                my_send (s, "PRIVMSG %s :%cACTION punched %s in the %s.%c\r\n", contact, 1, m->msg+strlen (m->command)+1, list[rand() % 5], 1);
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

        my_send (s, "PRIVMSG %s :%c3%s%s%c\r\n", contact, 3, baselink, m->msg+8, 3);
    }
}
