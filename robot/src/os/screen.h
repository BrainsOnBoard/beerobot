namespace OS::Screen {
extern "C"
{
#ifndef _WIN32
#include <X11/Xlib.h>
#endif
}

using XScreen = Screen;

bool
getResolution(int *width, int *height)
{
#ifdef _WIN32
    // TODO: implement this function on Windows
    return false;
#else
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        return false;
    }
    XScreen *screen = DefaultScreenOfDisplay(display);
    if (!screen) {
        XCloseDisplay(display);
        return false;
    }

    *width = screen->width;
    *height = screen->height;
    XCloseDisplay(display);
    return true;
#endif
}
}
