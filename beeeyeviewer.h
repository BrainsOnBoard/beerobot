#pragma once
#include "readable.h"
#include <opencv2/opencv.hpp>

using namespace cv;

/* run the remote bee eye viewer */
void run_eye_viewer(Readable &recv)
{
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

        // show image
        imshow("bee view", view);

        // quit when user presses esc
        if ((waitKey(1) & 0xff) == 27)
            break;
    }
}
