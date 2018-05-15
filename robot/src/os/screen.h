#pragma once

#include <opencv2/opencv.hpp>

namespace OS::Screen {
extern "C"
{
#ifndef _WIN32
#include <X11/Xlib.h>
#endif
}

using XScreen = Screen;

bool
getResolution(cv::Size &resolution)
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

    resolution.width = screen->width;
    resolution.height = screen->height;
    XCloseDisplay(display);
    return true;
#endif
}
}
