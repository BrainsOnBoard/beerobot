/*
 * File:   beeeye.h
 * Author: ad374
 *
 * Created on 16 May 2017, 10:59
 */

#ifndef BEEEYE_H
#define BEEEYE_H

// opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

// bee-eye pixel map generated by gigerommatidiamodel.py
#include "gigerdatacam.h"

#include "ini.h"

#ifdef USE_ROBOT
#define VIDEO_DEV "http://192.168.1.1:8080/?action=stream"
#else
#define VIDEO_DEV 0
#endif

using namespace std;
using namespace cv;

const int CROSS_SIZE = 20;

const int KEY_LEFT = 81;
const int KEY_UP = 82;
const int KEY_RIGHT = 83;
const int KEY_DOWN = 84;

const int PX_JUMP = 5;

inline void calib_line(Mat &src, Point p1, Point p2) {
    line(src, p1, p2, Scalar(0x00, 0xff, 0x00), 2);
}

/*
 * Displays camera output on screen after passing through bee-eye transform.
 * User can press Q to quit.
 */
void run_camera() {

    // read params from file
    CamParams p;
    p.read();

    // to capture webcam output
    VideoCapture cap(VIDEO_DEV);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open webcam" << endl;
        exit(1);
    }

    // set camera resolution to max (only works on opencv v3+)
    cap.set(CAP_PROP_FRAME_WIDTH, p.ssrc.width);
    cap.set(CAP_PROP_FRAME_HEIGHT, p.ssrc.height);

    Mat src;
    Mat dst(p.sdst, CV_8UC3);

    // create pixel maps for unwrapping panoramic images
    p.generate_map();

    /*
    // create x and y pixel maps
    Mat map_x, map_y;
    map_x.create(sz_out, CV_32FC1);
    map_y.create(sz_out, CV_32FC1);
    for (int i = 0; i < gdataLength; i++) {
        map_x.at<float>(gdata[i][3], gdata[i][2]) = floor(gdata[i][0]);
        map_y.at<float>(gdata[i][3], gdata[i][2]) = floor(gdata[i][1]);
    }

    Mat src2;
    Mat disp;
     */

    bool do_calib = false;

    // display remapped webcam output on loop until user presses Q
    bool do_run = true;
    while (do_run) {
        cap >> src;
        if (!src.size().width) {
            cerr << "Error: Could not read from webcam" << endl;
            exit(1);
        }

        remap(src, dst, p.map_x, p.map_y, INTER_NEAREST);

        /*
        resize(src, src2, sz);
        remap(src2, dst, map_x, map_y, INTER_LINEAR);
        resize(dst, disp, sz, 0, 0, INTER_NEAREST);
         */

        imshow("unwrapped image", dst);

        if (do_calib) {
            calib_line(src, Point(p.cent.x - CROSS_SIZE, p.cent.y), Point(p.cent.x + CROSS_SIZE, p.cent.y));
            calib_line(src, Point(p.cent.x, p.cent.y - CROSS_SIZE), Point(p.cent.x, p.cent.y + CROSS_SIZE));

            circle(src, p.cent, p.r_inner, Scalar(0x00, 0x00, 0xff), 2);
            circle(src, p.cent, p.r_outer, Scalar(0xff, 0x00, 0x00), 2);

            imshow("calibration", src);
        }

        int key = waitKey(1) & 0xff;
        /*if (key != 0xff)
            cout << "key: " << key << endl;*/
        switch (key) {
            case 'c':
                if (do_calib)
                    destroyWindow("calibration");

                do_calib = !do_calib;
                break;
            case 27: // ESC
                do_run = false;
                break;
            default:
                if (do_calib) {
                    switch (key) {
                        case 'q':
                            if (p.r_inner > 0) {
                                p.r_inner -= PX_JUMP;
                                p.generate_map();
                            }
                            break;
                        case 'a':
                            p.r_inner += PX_JUMP;
                            p.generate_map();
                            break;
                        case 'w':
                            if (p.r_outer > 0) {
                                p.r_outer -= PX_JUMP;
                                p.generate_map();
                            }
                            break;
                        case 's':
                            p.r_inner += PX_JUMP;
                            p.generate_map();
                            break;
                        case KEY_UP:
                            p.cent.y -= PX_JUMP;
                            p.generate_map();
                            break;
                        case KEY_DOWN:
                            p.cent.y += PX_JUMP;
                            p.generate_map();
                            break;
                        case KEY_LEFT:
                            p.cent.x -= PX_JUMP;
                            p.generate_map();
                            break;
                        case KEY_RIGHT:
                            p.cent.x += PX_JUMP;
                            p.generate_map();
                            break;
                    }
                }
        }
    }

    // write params to file
    p.write();
}

#endif /* BEEEYE_H */