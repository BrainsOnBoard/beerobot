#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <opencv2/opencv.hpp>

#include "image/videotype.h"

// GeNN_Robotics includes
#include "common/opencv_unwrap_360.h"
#include "videoin/videoinput.h"

namespace Eye {
class BeeEye : public VideoIn::VideoInput
{
public:
    std::unique_ptr<OpenCVUnwrap360> m_Unwrapper;

    BeeEye(const CameraInfo *vid, int vidDeviceNum = -1);

    bool getImage(cv::Mat &imorig);
    void getUnwrappedImage(cv::Mat &unwrap, cv::Mat &imorig);
    void getEyeView(cv::Mat &view, cv::Mat &imunwrap);
    bool getEyeView(cv::Mat &view);
    bool readFrame(cv::Mat &view);
    void runDisplay();

private:
    std::unique_ptr<VideoInput> m_Camera;
    cv::Mat m_MapX, m_MapY;
    cv::Mat m_ImOrig, m_ImUnwrap, m_ImEye;
};
}
