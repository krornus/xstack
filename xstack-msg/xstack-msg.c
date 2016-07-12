#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "socket.h"


int main(int argc, char ** argv)
{
    int sock, len, cmd;
    struct sockaddr_un remote;

    int opt;

    if (argc != 2)
    {
        printf("usage: xstack-msg exit|push|pop|peek|status\n");
        exit(1);
    }

    if(strcmp("exit", argv[1]) == 0)
        cmd = EXIT;
    else if(strcmp("push", argv[1]) == 0)
        cmd = PUSH;
    else if(strcmp("pop", argv[1]) == 0)
        cmd = POP;
    else if(strcmp("peek", argv[1]) == 0)
        cmd = PEEK;
    else if(strcmp("status", argv[1]) == 0)
        cmd = -1;
    else
    {
        printf("invalid command, usage: xstack-msg exit|push|pop|peek\n");
        exit(1);
    }

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
    {
        perror("could not create socket\n");
        exit(1);
    }

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    if (connect(sock, (struct sockaddr *)&remote, len) == -1) 
    {
        perror("could not connect\n");
        exit(1);
    }

    /* 
       this stops an issue on i3wm where
       keyboard grab fails due to race condition 
    */
    usleep(200000);

    if (cmd != -1 && send(sock, &cmd, 1, 0) == -1) 
    {
        perror("could not send\n");
        exit(1);
    }

    if (cmd == -1)
        printf("xstack-server running normally\n");

    close(sock);

    return 0;
}
