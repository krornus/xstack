#include <xstack.h>
#include <unistd.h>
#include <stdio.h>        
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>


XModifierKeymap *modifiers;
Display *display;
Window root;

int prepare()
{
    int screen;
    int keyboard_handle;

    if(!(display = XOpenDisplay(0)))
        exit(-1);

    modifiers = XGetModifierMapping(display);
    screen = DefaultScreen(display);
    root = RootWindow(display, screen);
}

void destruct()
{
    XFreeModifiermap(modifiers);
    XDestroyWindow(display, root);
}

list * push()
{
    if(XGrabKeyboard(display, root, False, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess)
        exit(-1);

    XAllowEvents(display, SyncBoth, CurrentTime);

    list * keys = event_listener();

    XUngrabKeyboard(display, CurrentTime);
    XSync(display, False);
}

void pop()
{
    //replay_event(display, root, keys);

}


list * event_listener()
{
    XKeyEvent xkev;

    list * head;
    list * keys;

    head = malloc(sizeof(list));
    keys = head;

    xkev = get_next_event();

    while(xkev.keycode != 24 && xkev.state !=4)
    {
        if(!is_modifier(xkev.keycode))
        {
            keys->key.keycode = xkev.keycode;
            keys->key.state = xkev.state;
            keys->key.type = xkev.type;

            keys->next = malloc(sizeof(list));
            keys = keys->next;
            keys->next = NULL;
        }

        xkev = get_next_event();
    }

    return head;
}


void replay_event(list * keys)
{
    int i = 0;

    while(keys)
    {
        XKeyEvent event;
        event = get_key_event(keys->key);

        XSendEvent(event.display, event.window, False, KeyPressMask|KeyReleaseMask, (XEvent *) &event); 

        i++;
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

    event.type = key.type;
    event.display = display;
    event.window = root;
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

