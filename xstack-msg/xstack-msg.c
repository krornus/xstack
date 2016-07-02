#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "socket.h"

char funcs[4] = { EXIT, PUSH, POP, PEEK };

int main(int argc, char ** argv)
{
    int sock, len;
    struct sockaddr_un remote;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
        exit(1);

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    if (connect(sock, (struct sockaddr *)&remote, len) == -1) 
        exit(1);

    if (send(sock, funcs + atoi(argv[1]), 1, 0) == -1) 
        exit(1);

    close(sock);

    return 0;
}
