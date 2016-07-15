// dont also include in xstack.c
#include <libconfig.h>
#include "xstack.h" 

int main(int argc, char ** argv);
void perform(char func);
int sock_dir(char *name);
void write_log(char * msg);
void parse_conf();
void get_key_settings(config_t *cfg, int *keycode, char *name, int def);

typedef struct config_data_t {
    int key_press_delay;
    int insert_delay;
    
    key_data quit;
    key_data delay;
} config_data;
