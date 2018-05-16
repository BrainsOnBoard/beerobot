#pragma once

#ifdef _WIN32
#include "windows_include.h"
#pragma comment(lib, "user32.lib")
#endif
#include <opencv2/opencv.hpp>

namespace OS::Screen {
#ifndef _WIN32
extern "C" {
#include <X11/Xlib.h>
using XScreen = Screen;
}
#endif

cv::Size
getResolution()
{
#ifdef _WIN32
    return cv::Size(GetSystemMetrics(SM_CXSCREEN),
                    GetSystemMetrics(SM_CYSCREEN));
#else
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        return cv::Size();
    }
    XScreen *screen = DefaultScreenOfDisplay(display);
    XCloseDisplay(display);
    if (!screen) {
        return cv::Size();
    }

    return cv::Size(screen->width, screen->height);
#endif
}
}
