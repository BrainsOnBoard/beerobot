/*
 * File:   beerobot.cpp
 * Author: alex
 *
 * Created on 12 May 2017, 13:38
 */

#include <cstdlib>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

const int KEY_Q = 113;

using namespace cv;

int main(int, char**) {

    VideoCapture cap(0);

    Mat fr;

    while (1) {
        cap >> fr;

        imshow("camera", fr);
        if ((waitKey(1) & 0xff) == KEY_Q)
            break;
    }

    return 0;
}

