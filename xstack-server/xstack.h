#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

typedef struct key_data_s {
    int keycode;
    int state;
    int type;
} key_data;

typedef struct key_list_s {
    key_data key; 
    struct key_list_s * next;
} key_list;

typedef struct macro_stack_s {
    key_list * keys;
    struct macro_stack_s *prev;
} macro_stack;

int prepare(void);
void destruct(void);
void push(void);
void pop(void);
void peek(void);
key_list *event_listener(void);
void replay_event(key_list *keys);
XKeyEvent get_next_event(void);
XKeyEvent get_key_event(key_data key);
int is_modifier(int keycode);

