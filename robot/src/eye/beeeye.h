#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <opencv2/opencv.hpp>

#include "image/camparams.h"
#include "videoin/videoinput.h"

namespace Eye {
class BeeEye : public VideoIn::VideoInput
{
public:
    Image::CamParams m_Params;

    BeeEye(vid_t *vid);
    ~BeeEye();

    bool getImage(cv::Mat &imorig);
    void getUnwrappedImage(cv::Mat &unwrap, cv::Mat &imorig);
    void getEyeView(cv::Mat &view, cv::Mat &imunwrap);
    bool getEyeView(cv::Mat &view);
    virtual bool readFrame(cv::Mat &view);

private:
    VideoInput *m_Camera = nullptr;
    cv::Mat m_MapX, m_MapY;
    cv::Mat m_ImOrig, m_ImUnwrap, m_ImEye;
};
}
