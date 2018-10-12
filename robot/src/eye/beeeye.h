#pragma once

// C includes
#include <cmath>

// C++ includes
#include <memory>

// OpenCV
#include <opencv2/opencv.hpp>

// BoB robotics includes
#include "imgproc/opencv_unwrap_360.h"
#if !defined(NO_SEE3CAM) && !defined(_WIN32)
#include "video/see3cam_cu40.h"
#endif
#include "video/input.h"
#include "video/opencvinput.h"
#include "video/panoramic.h"

// local includes
#include "beeeye.h"
#include "gigerdatacam.h"

using namespace BoBRobotics;

namespace Eye {
class BeeEye : public Video::Input
{
private:
    std::unique_ptr<Input> m_Camera;
    cv::Mat m_MapX, m_MapY;
    cv::Mat m_ImOrig, m_ImUnwrap, m_ImEye;

public:
    ImgProc::OpenCVUnwrap360 m_Unwrapper;

    BeeEye();

    bool getEyeView(cv::Mat &view);
    void getEyeView(cv::Mat &view, cv::Mat &imunwrap);
    bool getImage(cv::Mat &imorig);
    cv::Size getOutputSize() const;
    void getUnwrappedImage(cv::Mat &unwrap, cv::Mat &imorig);
    bool readFrame(cv::Mat &view);
}; // BeeEye

BeeEye::BeeEye()
  : m_Camera(Video::getPanoramicCamera())
  , m_Unwrapper(m_Camera->createUnwrapper({1280, 400}))
{
    // create x and y pixel maps for bee-eye transform
    cv::Size outSize(eye_size[0], eye_size[1]);
    m_MapX.create(outSize, CV_32FC1);
    m_MapY.create(outSize, CV_32FC1);
    for (int i = 0; i < gdataLength; i++) {
        // left eye
        m_MapX.at<float>((int) gdata[i][3], 15 + (int) gdata[i][2]) =
                floor(gdata[i][0]);
        m_MapY.at<float>((int) gdata[i][3], 15 + (int) gdata[i][2]) =
                floor(gdata[i][1]);

        // right eye
        m_MapX.at<float>((int) gdata[i][3],
                         720 - 316 - eye_size[0] - (int) gdata[i][2]) =
                gim_size[0] - floor(gdata[i][0]);
        m_MapY.at<float>((int) gdata[i][3],
                         720 - 316 - eye_size[0] - (int) gdata[i][2]) =
                floor(gdata[i][1]);
    }

    m_ImUnwrap.create(1280, 400, CV_8UC3);
    m_ImEye.create(outSize, CV_8UC3);
}

bool
BeeEye::getEyeView(cv::Mat &view)
{
    if (!getImage(m_ImOrig)) {
        return false;
    }

    m_Unwrapper.unwrap(m_ImOrig, m_ImUnwrap);
    getEyeView(view, m_ImUnwrap);
    return true;
}

void
BeeEye::getEyeView(cv::Mat &view, cv::Mat &imunwrap)
{
    /* perform two transformations:
     * - unwrap panoramic image
     * - bee eye
     *
     * (this could be done in a single step with the correct pixel map, but
     * this way is easier for now and works...) */
    remap(imunwrap, m_ImEye, m_MapX, m_MapY, cv::INTER_NEAREST);

    // resize the image we get out so it's large enough to see properly
    cv::Size sz(970, 1046);
    resize(m_ImEye, view, sz, 0, 0, cv::INTER_LINEAR);
}

bool
BeeEye::getImage(cv::Mat &imorig)
{
    // read frame from camera
    return m_Camera && m_Camera->readFrame(imorig);
}

cv::Size
BeeEye::getOutputSize() const
{
    return cv::Size(970, 1046);
}

bool
BeeEye::readFrame(cv::Mat &view)
{
    return getEyeView(view);
}
} // Eye
