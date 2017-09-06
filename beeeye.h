/*
 * File:   beeeye.h
 * Author: ad374
 *
 * Created on 04 September 2017, 13:33
 */

#ifndef BEEEYE_H
#define BEEEYE_H

#include <opencv2/opencv.hpp>
#include "camparams.h"

// Kodak PixPro over wifi
//#define VIDEO_PIXPRO_WIFI "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg"
//#define VID_WIDTH 1024
//#define VID_HEIGHT 1024

class BeeEye {
public:
    CamParams params;

    BeeEye(vid_t* vid);

    bool get_image(Mat &imorig);
    void get_unwrapped_image(Mat &unwrap, Mat &imorig);
    void get_eye_view(Mat &view, Mat &imunwrap);
    bool get_eye_view(Mat &view);
private:
    VideoCapture* cap;
    Mat map_x, map_y;
    Mat imorig, imunwrap, imeye;
};

#endif    /* BEEEYE_H */
