#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "camparams.h"
#include "readable.h"
#include <opencv2/opencv.hpp>

// Forward declarations
class See3CAM_CU40;

namespace Eye {
class BeeEye : public Readable
{
public:
    CamParams params;

    BeeEye(vid_t *vid);
    ~BeeEye();

    bool getImage(cv::Mat &imorig);
    void getUnwrappedImage(cv::Mat &unwrap, cv::Mat &imorig);
    void getEyeView(cv::Mat &view, cv::Mat &imunwrap);
    bool getEyeView(cv::Mat &view);
    virtual bool read(cv::Mat &view) override;

private:
    cv::VideoCapture *m_Cap = nullptr;
#ifndef _WIN32
    See3CAM_CU40 *m_See3Cam = nullptr;
#endif
    cv::Mat m_MapX, m_MapY;
    cv::Mat m_ImOrig, m_ImUnwrap, m_ImEye;
};
}
