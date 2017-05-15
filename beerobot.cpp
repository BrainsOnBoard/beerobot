/*
 * File:   beerobot.cpp
 * Author: alex
 *
 * Created on 12 May 2017, 13:38
 */

#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "gigerdatacam.h"

using namespace std;
using namespace cv;

int main(int, char**) {

    // the size of the frame after resizing
    Size sz(720, 480);
    Size sz_out(140,100);
    
    printf("here...\n");

    // to capture webcam output
    VideoCapture cap(0);

    Mat src, dst;
    dst.create(sz_out, CV_8UC3);
    
    printf("here1.5...\n");

    // create x and y pixel maps
    Mat map_x, map_y;
    map_x.create(sz_out, CV_32FC1);
    map_y.create(sz_out, CV_32FC1);
    for( int i = 0; i < gdataLength; i++ )
    {
        map_x.at<float>(gdata[i][3],gdata[i][2]) = floor(gdata[i][0]) ;
        map_y.at<float>(gdata[i][3],gdata[i][2]) = floor(gdata[i][1]) ;
    }

    
    printf("here2...\n");

    // display remapped webcam output on loop until user presses Q
    Mat src2;
    Mat disp;
    
    if (!cap.isOpened()) printf("OMFG\n");
    
    while (1) {
        cap >> src;
        cv::resize(src, src2, sz);

        remap(src2, dst, map_x, map_y, INTER_LINEAR);
        
        cv::resize(dst, disp, sz, 0,0, INTER_NEAREST);

        imshow("camera", disp);
        if ((char) (waitKey(1) & 0xff) == 'q')
            break;
    }

    return 0;
}

