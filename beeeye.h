/*
 * File:   beeeye.h
 * Author: ad374
 *
 * Created on 16 May 2017, 10:59
 */

#ifndef BEEEYE_H
#define BEEEYE_H

// opencv includes
// (note that the program won't work with older versions of opencv v2;
// all versions of opencv v3 should be fine)
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

// bee-eye pixel map generated by gigerommatidiamodel.py
#include "gigerdatacam.h"

#include <iostream>
#include <fstream>

#if defined(USE_SURVEYOR)
    // Surveyor's webcam stream
    //#define VIDEO_DEV "http://192.168.1.1:8080/?action=stream?dummy_parameter=bee.mjpg"
    //#define VID_WIDTH 640
    //#define VID_HEIGHT 480

    // Kodak PixPro
    #define VIDEO_DEV "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg"
    #define VID_WIDTH 1024
    #define VID_HEIGHT 1024
#elif defined(USE_ARDUINO)
    // USB connnected Kodak PixPro
    #define VIDEO_DEV 0 //get_camera_by_name("USB 2.0 Camera")
    #define VID_WIDTH 640
    #define VID_HEIGHT 480
#else
    // USB webcam
    #define VIDEO_DEV 0 //get_camera_by_name("USB 2.0 Camera")
    #define VID_WIDTH 640
    #define VID_HEIGHT 480
#endif

#include "ini.h"

using namespace std;
using namespace cv;

const int CROSS_SIZE = 20; // size of calibration cross

// keyboard key codes
const int KB_LEFT = 81;
const int KB_UP = 82;
const int KB_RIGHT = 83;
const int KB_DOWN = 84;
const int KB_ESC = 27;

// number of pixels to move/resize things by in calibration mode
const int BIG_PX_JUMP = 5;

// draws a line for the calibration cross
inline void calib_line(Mat &src, Point p1, Point p2) {
    line(src, p1, p2, Scalar(0x00, 0xff, 0x00), 2);
}

// get the number for a camera with a given name (-1 if not found)
int get_camera_by_name(const char* name) {
    char cname[4096];
    
    // iterate through devices video0, video1 etc. reading the device name from sysfs
    // until the correct device is found
    for (int i = 0;; i++) {
        string vfn = "/sys/class/video4linux/video" + to_string(i) + "/name";
        ifstream file(vfn, ios::in);
        if (!file.is_open())
            return -1;

        file.read(cname, sizeof (cname));
        cname[file.gcount() - 1] = 0; // delete the last char, which is always newline
        file.close();

        if (strcmp(name, cname) == 0) // we've found the correct device
            return i;
    }
}

/*
 * Displays camera output on screen after passing through bee-eye transform.
 * User can press escape to quit.
 */
void run_camera() {

    // read params from beerobot.ini
    CamParams p;
    p.read();

    // to capture webcam output
    VideoCapture cap(VIDEO_DEV);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open webcam (" << VIDEO_DEV << ")" << endl;
        exit(1);
    }

    // for the bee-eye transformation
    Size sz_out(eye_size[0], eye_size[1]);
    Mat dst_eye;
    dst_eye.create(sz_out, CV_8UC3);
    
    // ultimate output size, after the unwrapping and bee-eye transformations
    Size sz(970, 1046);

    // create x and y pixel maps
    Mat map_x, map_y;
    map_x.create(sz_out, CV_32FC1);
    map_y.create(sz_out, CV_32FC1);
    for (int i = 0; i < gdataLength; i++) {
        // left eye
        map_x.at<float>(gdata[i][3], 15 + gdata[i][2]) = floor(gdata[i][0]);
        map_y.at<float>(gdata[i][3], 15 + gdata[i][2]) = floor(gdata[i][1]);

        // right eye
        map_x.at<float>(gdata[i][3], 720 - 316 - eye_size[0] - gdata[i][2]) = gim_size[0] - floor(gdata[i][0]);
        map_y.at<float>(gdata[i][3], 720 - 316 - eye_size[0] - gdata[i][2]) = floor(gdata[i][1]);
    }

    // input and final output image matrices
    Mat src, disp;
    Mat dst(p.sdst, CV_8UC3);

    // create pixel maps for unwrapping panoramic images
    p.generate_map();

    bool do_calib = false; // whether calibration screen is visible or not
    int px_jump = BIG_PX_JUMP; // number of pixels to move by for calibration (either 1 or 5)

    // display remapped camera output on loop until user presses escape
    for (bool do_run = true; do_run;) {
        cap >> src; // read frame from camera
        
        // if empty image returned, quit program
        if (src.size().width == 0) {
            cerr << "Error: Could not read from webcam" << endl;
            exit(1);
        }

        /* perform two transformations:
         * - unwrap panoramic image
         * - bee eye
         * 
         * (this could be done in a single step with the correct pixel map, but 
         * this way is easier for now and works...) */
        remap(src, dst, p.map_x, p.map_y, INTER_NEAREST);
        remap(dst, dst_eye, map_x, map_y, INTER_NEAREST);
        
        // resize the image we get out so it's large enough to see properly
        resize(dst_eye, disp, sz, 0, 0, INTER_LINEAR);

        // show image
        imshow("unwrapped image", disp);

        if (do_calib) { // then show calibration screen
            // draw calibration cross at what we've chose as the center
            calib_line(src, Point(p.cent.x - CROSS_SIZE, p.cent.y), Point(p.cent.x + CROSS_SIZE, p.cent.y));
            calib_line(src, Point(p.cent.x, p.cent.y - CROSS_SIZE), Point(p.cent.x, p.cent.y + CROSS_SIZE));

            // draw inner and outer circles, showing the area which we will unwrap
            circle(src, p.cent, p.r_inner, Scalar(0x00, 0x00, 0xff), 2);
            circle(src, p.cent, p.r_outer, Scalar(0xff, 0x00, 0x00), 2);

            // show the image
            imshow("calibration", src);
        }

        // read keypress in
        int key = waitKey(1) & 0xff;
        /*if (key != 0xff)
            cout << "key: " << key << endl;*/
        switch (key) {
            case 'c': // toggle display of calibration screen
                if (do_calib) // we have to close window explicitly
                    destroyWindow("calibration");

                do_calib = !do_calib;
                break;
            case KB_ESC: // quit program
                do_run = false;
                break;
            default:
                if (do_calib) {
                    switch (key) {
                        case ' ': // toggle 1px/5px jumps when moving/resizing
                            if (px_jump == BIG_PX_JUMP)
                                px_jump = 1;
                            else
                                px_jump = BIG_PX_JUMP;
                            break;
                        case 'w': // make inner circle bigger
                            p.r_inner += px_jump;
                            p.generate_map();
                            break;
                        case 's': // make inner circle smaller
                            if (p.r_inner > 0) {
                                p.r_inner -= px_jump;
                                p.r_inner = max(0, p.r_inner);
                                p.generate_map();
                            }
                            break;
                        case 'q': // make outer circle bigger
                            p.r_outer += px_jump;
                            p.generate_map();
                            break;
                        case 'a': // make outer circle smaller
                            if (p.r_outer > 0) {
                                p.r_outer -= px_jump;
                                p.r_outer = max(0, p.r_outer);
                                p.generate_map();
                            }
                            break;
                        case KB_UP: // move centre up
                            p.cent.y -= px_jump;
                            p.generate_map();
                            break;
                        case KB_DOWN: // move centre down
                            p.cent.y += px_jump;
                            p.generate_map();
                            break;
                        case KB_LEFT: // move centre left
                            p.cent.x -= px_jump;
                            p.generate_map();
                            break;
                        case KB_RIGHT: // move centre right
                            p.cent.x += px_jump;
                            p.generate_map();
                            break;
                    }
                }
        }
    }

    // write params to file
    // in particular we want to remember our calibration settings so we don't 
    // have to recalibrate the next time we start the program
    p.write();
}

#endif /* BEEEYE_H */