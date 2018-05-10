#pragma once
#include "readable.h"
#include <opencv2/opencv.hpp>

#ifndef _WIN32
#include <X11/Xlib.h>
#endif

namespace Eye {
/* run the remote bee eye viewer */
void
runEyeViewer(Readable &recv, bool showoverlay)
{
#ifdef _WIN32
    cv::Size oversz(1024, 768);
#else
    Display *disp = XOpenDisplay(nullptr);
    Screen *scrn = DefaultScreenOfDisplay(disp);
    cv::Size oversz(scrn->width, scrn->height);
#endif

    std::cout << "Screen resolution: " << oversz << std::endl;

    int xoff;
    cv::Mat overlay, overlayinner, iminner, mask;
    if (showoverlay) {
        int w = (int)round((float) oversz.height * 970.0 / 1048.0);
        xoff = (oversz.width - w) / 2;

        cv::Mat overlayfull = cv::imread("honeycomb_overlay.png", 3);
        cv::resize(overlayfull, overlay, oversz, 0, 0, cv::INTER_CUBIC);

        overlayinner = overlay(cv::Range::all(), cv::Range(xoff, xoff + w));
        mask = (overlayinner == 0);
        iminner.create(overlayinner.size(), overlayinner.type());
    } else {
        std::cout << "Image overlay disabled" << std::endl;
    }

    // to store frame
    cv::Mat view;

    // set opencv window to display full screen
    cvNamedWindow("bee view", CV_WINDOW_NORMAL);
    setWindowProperty(
            "bee view", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    // display remote camera input on loop until user presses escape
    while (true) {
        // read frame over socket
        recv.read(view);

        // bad JPEGs give a 0x0 frame
        if (view.rows == 0 && view.cols == 0) {
            std::cerr << "Warning: Could not process JPEG" << std::endl;
            continue;
        }

        if (showoverlay) {
            cv::resize(view, iminner, iminner.size());

            iminner.copyTo(overlayinner, mask);

            // show image
            cv::imshow("bee view", overlay);
        } else {
            cv::imshow("bee view", view);
        }

        // quit when user presses esc
        if ((cv::waitKey(1) & 0xff) == 27)
            break;
    }
}
}
