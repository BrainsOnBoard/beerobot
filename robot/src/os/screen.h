#pragma once

#include "windows_include.h"
#include <opencv2/opencv.hpp>

namespace OS::Screen {
#ifndef _WIN32
extern "C" {
#include <X11/Xlib.h>
using XScreen = Screen;
}
#endif

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
