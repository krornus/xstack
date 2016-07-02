#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

#include "socket.h"
#include "xstack.h"

int main(int argc, char ** argv);
void perform(char func);
void write_log(char * msg);

int sock;

int main(int argc, char ** argv)
{
    int client_sock, t, len;
    struct sockaddr_un local, remote;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        exit(1);

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);

    unlink(local.sun_path);

    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if (bind(sock, (struct sockaddr *)&local, len) == -1) 
        exit(1);

    if (listen(sock, 1) == -1) 
        exit(1);
    
    prepare();
    
    if(daemon(0,0) == -1)
    {
        write_log("DAEMON FAILURE\n");
        exit(1);
    }

    write_log("DAEMON SUCCESS\n");
    
    while(1)
    {
        char buf[1];
        short func;

        t = sizeof(remote);
        if ((client_sock = accept(sock, (struct sockaddr *)&remote, &t)) == -1) 
            exit(1);

        recv(client_sock, buf, 1, 0);
        close(client_sock);

        write_log("RECIEVED MESSAGE\n");

        perform(buf[0]);
    }

    return 0;
}

void perform(char func)
{
    write_log("perform() ENTRY\n");
    switch(func)
    {
        case EXIT:
            write_log("EXIT CALLED\n");
            close(sock);
            destruct();
            exit(0);
            break;
        case PUSH:
            /* LISTEN */
            write_log("PUSH CALLED\n");
            push();
            break;
        case POP:
            /* REPLAY */
            write_log("POP CALLED\n");
            break;
        case PEEK:
            write_log("PEEK CALLED\n");
            /* PEEK */
            break;
    }
}

void write_log(char * msg)
{
    FILE *log_f;
    log_f = fopen("/home/spowell/programming/c/xstack/log", "a");
    fputs(msg, log_f);
    fclose(log_f);
}
