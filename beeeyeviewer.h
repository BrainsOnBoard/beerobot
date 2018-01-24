#pragma once
#include "readable.h"
#include <opencv2/opencv.hpp>

using namespace cv;

Mat overlay, overlayinner;

/* run the remote bee eye viewer */
void run_eye_viewer(Readable &recv)
{
    Size oversz(1920, 1200);

    Mat overlayfull = imread("honeycomb_overlay.png", 3);
    resize(overlayfull, overlay, oversz, 0, 0, INTER_CUBIC);

    int w = round((float) oversz.height * 970.0 / 1048.0);
    int xoff = (oversz.width - w) / 2;
    overlayinner = overlay(Range::all(), Range(xoff, xoff + w));

    // to store frame
    Mat view;
    Mat imfull = overlay.clone();
    Mat iminner = imfull(Range::all(), Range(xoff, xoff + w));

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

        resize(view, iminner, overlayinner.size());

        for (int i = 0; i < overlayinner.rows; i++) {
            for (int j = 0; j < overlayinner.cols; j++) {
                Vec3b px = overlayinner.at<Vec3b>(i, j);
                if (px[0] && px[1] && px[2])
                    iminner.at<Vec3b>(i, j) = overlayinner.at<Vec3b>(i, j);
            }
        }

        // show image
        imshow("bee view", imfull);

        // quit when user presses esc
        if ((waitKey(1) & 0xff) == 27)
            break;
    }
}
