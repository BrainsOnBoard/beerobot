#pragma once

#include <opencv2/opencv.hpp>
#include "camparams.h"

// Forward declarations
class See3CAM_CU40;

class BeeEye {
public:
    CamParams params;

    BeeEye(vid_t* vid);
    ~BeeEye();

    bool get_image(Mat &imorig);
    void get_unwrapped_image(Mat &unwrap, Mat &imorig);
    void get_eye_view(Mat &view, Mat &imunwrap);
    bool get_eye_view(Mat &view);
private:
    VideoCapture* cap;
    See3CAM_CU40* see3cam;
    Mat map_x, map_y;
    Mat imorig, imunwrap, imeye;
};
