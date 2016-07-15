#include <stdlib.h>
#include "xstack-server.h"
#include "hash.h"


XModifierKeymap *modifiers;
Window root;
macro_stack *head;

extern kv_pair *hash[HASH_SIZE];
extern Display *display;
extern config_data settings;

int prepare()
{
    int screen;
    int keyboard_handle;

    modifiers = XGetModifierMapping(display);
    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

    head = NULL;
}

void destruct()
{
    XFreeModifiermap(modifiers);
    XDestroyWindow(display, root);
}

void push()
{

    if(XGrabKeyboard(display, root, False, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess)
    {
        perror("failed to grab keyboard!\n");
        return;
    }

    XAllowEvents(display, SyncBoth, CurrentTime);

    key_list *keys = event_listener();

    macro_stack *item = malloc(sizeof(macro_stack));
    item->keys = keys;
    item->prev = head;
    head = item;

    XUngrabKeyboard(display, CurrentTime);
    XSync(display, False);
}

void pop()
{
    if(head == NULL)
        return;

    macro_stack *item = head;
    replay_event(head->keys);
    head = head->prev;

    free(item->keys);
    free(item);
}

void peek()
{
    if(head == NULL)
        return;

    replay_event(head->keys);
}

key_list * event_listener()
{
    XKeyEvent xkev;

    key_list * head;
    key_list * keys;

    head = malloc(sizeof(key_list));
    keys = head;

    xkev = get_next_event();

    while(!(xkev.keycode == settings.quit.keycode && 
            clean_modifier(xkev.state) == settings.quit.state))
    {
        if(!is_modifier(xkev.keycode))
        {
            if(xkev.keycode == settings.delay.keycode &&
                    clean_modifier(xkev.state) == settings.delay.state)
            {
                keys->key.keycode = -1;

                keys->next = malloc(sizeof(key_list));
                keys = keys->next;
                keys->next = NULL;
            }
            else
            {
                keys->key.keycode = xkev.keycode;
                keys->key.state = xkev.state;
                keys->key.type = xkev.type;

                keys->next = malloc(sizeof(key_list));
                keys = keys->next;
                keys->next = NULL;
            }
        }
        xkev = get_next_event();
    }

    return head;
}

int clean_modifier(int state)
{
    /* only capture shift, alt, win, ctrl modifiers */
    return state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask);
}


void replay_event(key_list * keys)
{
    while(keys->next != NULL)
    {
        if(keys->key.keycode == -1)
        {
            XFlush(display);
            usleep(10000*settings.insert_delay);    
            keys = keys->next;
            continue;
        }

        int keycode, type;

        XKeyEvent event;
        event = get_key_event(keys->key);

        press_modifiers(keys->key.state, True);  

        keycode = keys->key.keycode;
        type = ~(keys->key.type-2)&0x1;

        insert(keycode, type);

        XTestFakeKeyEvent(display, keycode, type, CurrentTime);

        press_modifiers(keys->key.state, False);  
       
        usleep(1000*settings.key_press_delay);

        keys = keys->next;
    }
    
    int i;
    for(i = 0; i < HASH_SIZE; i++)
    {
        if(hash[i] && hash[i]->value)
        {
            XTestFakeKeyEvent(display, hash[i]->key, False, CurrentTime);
            free(hash[i]);
            hash[i] = NULL;
        }
    }

    XFlush(display);
}


XKeyEvent get_next_event()
{
    XEvent ev;
    XWindowEvent(display, root, KeyPressMask|KeyReleaseMask, &ev);

    return ev.xkey;
}

XKeyEvent get_key_event(key_data key)
{
    XKeyEvent event;

    Window win;
    int revert;

    XGetInputFocus(display, &win, &revert);

    event.type = key.type;
    event.display = display;
    event.window = win;
    event.root = root;
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.state = key.state;
    event.keycode = key.keycode;

    return event;
}

int is_modifier(int keycode)
{
    if(!keycode)
        return 0;

    int i;

    for(i = 0; i < 8 * modifiers->max_keypermod; i++)
    {
        if(modifiers->modifiermap[i] == keycode)
        {
            return 1;
        }
    }

    return 0;
}

void press_modifiers(int state, int type)
{
    
    short shift = 0;
    int max = modifiers->max_keypermod;
    int i;
    
    for(i = 0; i < 8 * max; i+=max)
    {
        if((state>>shift)&0x1)
        {
            int keycode = modifiers->modifiermap[i];

            insert(keycode, type);
            XTestFakeKeyEvent(display, modifiers->modifiermap[i], type, CurrentTime);
        }
        shift++;
    }
}

