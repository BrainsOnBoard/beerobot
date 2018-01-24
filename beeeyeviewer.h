#pragma once
#include "readable.h"
#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>

using namespace cv;

/* run the remote bee eye viewer */
void run_eye_viewer(Readable &recv, bool showoverlay)
{
    Display* disp = XOpenDisplay(NULL);
    Screen* scrn = DefaultScreenOfDisplay(disp);
    Size oversz(scrn->width, scrn->height);
    cout << "Screen resolution: " << oversz << endl;

    int xoff;
    Mat overlay, overlayinner, iminner, mask;
    if (showoverlay) {
        int w = round((float) oversz.height * 970.0 / 1048.0);
        xoff = (oversz.width - w) / 2;

        Mat overlayfull = imread("honeycomb_overlay.png", 3);
        resize(overlayfull, overlay, oversz, 0, 0, INTER_CUBIC);

        overlayinner = overlay(Range::all(), Range(xoff, xoff + w));
        mask = (overlayinner == 0);
        iminner.create(overlayinner.size(), overlayinner.type());
    } else {
        cout << "Image overlay disabled" << endl;
    }

    // to store frame
    Mat view;

    // set opencv window to display full screen
    cvNamedWindow("bee view", CV_WINDOW_NORMAL);
    setWindowProperty("bee view", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    // display remote camera input on loop until user presses escape
    while (true) {
        // read frame over socket
        recv.read(&view);

        // bad JPEGs give a 0x0 frame
        if (view.rows == 0 && view.cols == 0) {
            cout << "Warning: Could not process JPEG" << endl;
            continue;
        }

        if (showoverlay) {
            resize(view, iminner, iminner.size());

            iminner.copyTo(overlayinner, mask);

            // show image
            imshow("bee view", overlay);
        } else {
            imshow("bee view", view);
        }

        // quit when user presses esc
        if ((waitKey(1) & 0xff) == 27)
            break;
    }
}
