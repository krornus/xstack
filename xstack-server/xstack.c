#include <xstack.h>
#include <unistd.h>
#include <stdio.h>        
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#define DELAY 50

XModifierKeymap *modifiers;
Display *display;
Window root;
macro_stack *head;

int prepare()
{
    int screen;
    int keyboard_handle;

    if(!(display = XOpenDisplay(0)))
        exit(-1);

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
        exit(-1);

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

    while(!(xkev.keycode == 24 && xkev.state == 4))
    {
        if(!is_modifier(xkev.keycode))
        {
            keys->key.keycode = xkev.keycode;
            keys->key.state = xkev.state;
            keys->key.type = xkev.type;

            keys->next = malloc(sizeof(key_list));
            keys = keys->next;
            keys->next = NULL;
        }

        xkev = get_next_event();
    }

    return head;
}


void replay_event(key_list * keys)
{
    while(keys->next != NULL)
    {
        XKeyEvent event;
        event = get_key_event(keys->key);

        printf("replaying %d, %d\n", event.keycode, event.type);

        XSelectInput(display, event.window, KeyPressMask|KeyReleaseMask); 
        XSendEvent(display, event.window, False, KeyPressMask|KeyReleaseMask, (XEvent*)&event); 
        
        printf("sleeping\n");
        usleep(1000*DELAY);

        keys = keys->next;
    }
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

