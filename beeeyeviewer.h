#pragma once

#include "imagereceiver.h"
#include <opencv2/opencv.hpp>

using namespace cv;

void run_eye_viewer(ImageReceiver &recv)
{
    Mat view;

    // set opencv window to display full screen
    cvNamedWindow("bee view", CV_WINDOW_NORMAL);
    setWindowProperty("bee view", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    // display remote camera input on loop until user presses escape
    while (true) {
        recv.read(view);
        if (view.rows == 0 && view.cols == 0)
            continue;

        // show image
        imshow("bee view", view);

        // read keypress in
        if ((waitKey(1) & 0xff) == 27) {
            break;
        }
    }
}