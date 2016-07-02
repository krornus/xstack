#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

typedef struct key_data_s {
    int keycode;
    int state;
    int type;
} key_data;

typedef struct list_s {
    key_data key; 
    struct list_s * next;
} list;

int prepare(void);
void destruct(void);
list *push(void);
void pop(void);
list *event_listener(void);
void replay_event(list *keys);
XKeyEvent get_next_event(void);
XKeyEvent get_key_event(key_data key);
int is_modifier(int keycode);

