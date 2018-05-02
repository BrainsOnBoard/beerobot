#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <opencv2/opencv.hpp>
#include "camparams.h"
#include "readable.h"

// Forward declarations
class See3CAM_CU40;

class BeeEye : public Readable {
public:
    CamParams params;

    BeeEye(vid_t* vid);
    ~BeeEye();

    bool get_image(cv::Mat &imorig);
    void get_unwrapped_image(cv::Mat &unwrap, cv::Mat &imorig);
    void get_eye_view(cv::Mat &view, cv::Mat &imunwrap);
    bool get_eye_view(cv::Mat &view);
    virtual bool read(cv::Mat *view) override;
private:
    cv::VideoCapture* cap = nullptr;
#ifndef _WIN32
    See3CAM_CU40* see3cam = nullptr;
#endif
    cv::Mat map_x, map_y;
    cv::Mat imorig, imunwrap, imeye;
};
