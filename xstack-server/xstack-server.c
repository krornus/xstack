#include <stdio.h>
#include <libconfig.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>
#include "socket.h"
#include "xstack-server.h"

int sock;
Display * display;
config_data settings;

int main(int argc, char ** argv)
{

    int client_sock, t, len;
    struct sockaddr_un local, remote;

    if(!(display = XOpenDisplay(0)))
    {
        perror("failed to open display!\n");
        exit(-1);
    }

    parse_conf();

    sock_dir(SOCK_DIR);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        exit(1);

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);

    unlink(local.sun_path);

    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if (bind(sock, (struct sockaddr *)&local, len) == -1) 
        exit(2);

    if (listen(sock, 1) == -1) 
        exit(3);
    
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
            printf("exiting\n");
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
            pop();
            break;
        case PEEK:
            write_log("PEEK CALLED\n");
            peek();
            /* PEEK */
            break;
    }
}

int sock_dir(char *name)
{
    struct stat st = {0};

    if (stat(name, &st) == -1)
            //TODO: make permissions minimal
            mkdir(name, 0755);
}

void write_log(char * msg)
{
    FILE *log_f;
    log_f = fopen("/home/spowell/programming/c/xstack/log", "a");
    fputs(msg, log_f);
    fclose(log_f);
}


void parse_conf()
{
    char *file = "/.xstackrc";
    char *path = getenv("HOME");

    strcat(path, file);

    if(access(path, F_OK|R_OK) == -1)
    {
        /* CANNOT ACCESS CONF */
        settings.quit.state = QUIT_MOD_DEFAULT;
        settings.quit.keycode = QUIT_KEYCODE_DEFAULT;
        settings.delay.state = DELAY_MOD_DEFAULT;
        settings.delay.keycode = DELAY_KEYCODE_DEFAULT;
        settings.key_press_delay = KEY_DELAY_DEFAULT;
        settings.key_press_delay = INSERT_DELAY_DEFAULT;
        return;
    }

    config_t *cfg;
    cfg = malloc(sizeof(config_t));

    config_init(cfg);

    if(!config_read_file(cfg, path))
    {
        fprintf(stderr, 
                "error parsing config, line: %d\n%s\n", 
                config_error_line(cfg),
                config_error_text(cfg));
        config_destroy(cfg);
        exit(1);
    }

    /* quit key */
    get_key_settings(cfg, &(settings.quit.keycode), "quit_key", QUIT_KEYCODE_DEFAULT);
    get_key_settings(cfg, &(settings.delay.keycode), "delay_key", DELAY_KEYCODE_DEFAULT);
    
    if(!config_lookup_int(cfg, "quit_modifier", &(settings.quit.state)))
        settings.quit.state = QUIT_MOD_DEFAULT;

    if(!config_lookup_int(cfg, "delay_modifier", &(settings.delay.state)))
        settings.delay.state = DELAY_MOD_DEFAULT;

    if(!config_lookup_int(cfg, "key_press_delay", &(settings.key_press_delay)))
        settings.key_press_delay = KEY_DELAY_DEFAULT;

    if(!config_lookup_int(cfg, "insert_delay", &(settings.insert_delay)))
        settings.insert_delay = INSERT_DELAY_DEFAULT;
}

void get_key_settings(config_t *cfg, int *keycode, char *name, int def)
{
    const char *keystr; 
    *keycode = -1;

    if(config_lookup_string(cfg, name, &keystr))
    {
        KeySym ksym;
        ksym = XStringToKeysym(keystr);
        *keycode = (int)XKeysymToKeycode(display, ksym);
    }

    if(*keycode == -1)
    {
        config_lookup_int(cfg, name, keycode);
    }

    if(*keycode == -1)
        *keycode = def;
}
