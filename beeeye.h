/* 
 * File:   beeeye.h
 * Author: ad374
 *
 * Created on 04 September 2017, 13:33
 */

#ifndef BEEEYE_H
#define BEEEYE_H

#include <opencv2/opencv.hpp>

#define VIDEO_DEV get_camera_by_name("PIXPRO SP360 4K")

#define VID_WIDTH 1440
#define VID_HEIGHT 1440
#include "camparams.h"

class BeeEye {
public:
    BeeEye();
    //virtual ~BeeEye();
    
    bool get_image(Mat &imorig);
    void get_unwrapped_image(Mat &imunwrap, Mat &imorig);
    void get_eye_view(Mat &view, Mat &imunwrap);
    bool get_eye_view(Mat &view);
private:
    CamParams params;
    VideoCapture cap;
    Mat map_x, map_y;
    Mat imorig, imunwrap, imeye;
};

// get the number for a camera with a given name (-1 if not found)
int get_camera_by_name(const char* name);

#endif    /* BEEEYE_H */
