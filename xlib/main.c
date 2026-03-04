#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#define log_err(msg) do {fprintf(stderr, "%s\n", (msg));} while (0)

int width = 1280;
int height = 720;
int window_open;

int main(int argc, char** args)
{
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        log_err("[Xlib]: Display failed to open!"); 
        exit(1);
    }
    int root = DefaultRootWindow(display);
    int default_screen = DefaultScreen(display);

    int screenBitDepth = 24;
    XVisualInfo visinfo = {};
    if(!XMatchVisualInfo(display, default_screen, screenBitDepth, TrueColor, &visinfo)) {
        log_err("[Xlib]: Visual info not found!");
        exit(1);
    }

    XSetWindowAttributes windowAttr;
    windowAttr.background_pixel = 0;
    windowAttr.colormap = XCreateColormap(display, root, visinfo.visual, AllocNone);
    windowAttr.event_mask = KeyPressMask | KeyReleaseMask
        | ButtonPress | ButtonRelease | PointerMotionMask;
    unsigned long attributeMask = CWEventMask | CWBackPixel | CWColormap;
    Window window = XCreateWindow(display, root, 
 				0, 0,
 				width, height, 0,
 				visinfo.depth, InputOutput,
 				visinfo.visual, attributeMask, &windowAttr);

    if(!window) {
        log_err("[Xlib]: Window creation failed!");
        exit(1);
    }

    XStoreName(display, window, "Hello, XLIB!");
    XMapWindow(display, window);
    XFlush(display);

    window_open = 1;
    while(window_open) 
    {
        XEvent ev = {0};
        while(XPending(display) > 0)
        {
            XNextEvent(display, &ev);
            switch(ev.type)
            {
            case DestroyNotify: 
            {
                XDestroyWindowEvent* e = (XDestroyWindowEvent*)&ev;
                if(e->window == window)
                {
                    window_open = 0;
                }
            }break;
            case KeyPress: 
            {
                XKeyPressedEvent* e = (XKeyPressedEvent*)&ev;
                if (e->keycode == 9) 
                {
                    window_open = 0;
                }
            }break;
            case ButtonPress:
            {
            }break;
            case MotionNotify:
            {
                XPointerMovedEvent* e = (XPointerMovedEvent*)&ev;
                printf("[mouse -> x:%d, y:%d]\n", e->x, e->y);
                fflush(stdout);
            }break;  
            } /*switch*/
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    exit(0);    
}


//   XSetWindowAttributes windowAttr;
//   windowAttr.background_pixel = 0;
//   windowAttr.colormap = XCreateColormap(display, root, 
// 					 visinfo.visual, AllocNone);
//   unsigned long attributeMask = CWBackPixel | CWColormap;
//
//   Window window = XCreateWindow(display, root, 
// 				0, 0,
// 				width, height, 0,
// 				visinfo.depth, InputOutput,
// 				visinfo.visual, attributeMask, &windowAttr);
//
//   if(!window) {
//     printf("Window wasn't created properly\n");
//     exit(1);
//   }
//
//   XStoreName(display, window, "Hello, World!");
//
//   XMapWindow(display, window);
//   XFlush(display);
//
//
//   while(true)
//     {
//     }
//
//   return 0;
// } 
