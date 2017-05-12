/*
 * File:   beerobot.cpp
 * Author: alex
 *
 * Created on 12 May 2017, 13:38
 */

#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "gigerdatacam.h"

using namespace std;
using namespace cv;

int main(int, char**) {

    // the size of the frame after resizing
    Size sz(720, 480);

    // to capture webcam output
    VideoCapture cap(0);

    Mat src, dst;
    dst.create(sz, CV_32SC3);

    // create x and y pixel maps
    Mat map_x, map_y;
    map_x.create(sz, CV_32FC1);
    map_y.create(sz, CV_32FC1);
    for (int i = 0; i < gdataLength; i++) {
        for (int j = 0; j < map_x.rows; j++)
            map_x.at<float>(j, gdata[i][0]) = gdata[i][2];

        for (int j = 0; j < map_y.cols; j++)
            map_y.at<float>(gdata[i][1], j) = gdata[i][3];
    }

    // display remapped webcam output on loop until user presses Q
    Mat src2;
    while (1) {
        cap >> src;
        cv::resize(src, src2, sz);

        remap(src2, dst, map_x, map_y, INTER_LINEAR);

        imshow("camera", dst);
        if ((char) (waitKey(1) & 0xff) == 'q')
            break;
    }

    return 0;
}

