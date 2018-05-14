#include "beeeye.h"
#include "gigerdatacam.h"

#ifdef _WIN32
#define NO_SEE3CAM
#endif

// GeNN robotics includes
#include "videoin/opencvinput.h"
#ifndef NO_SEE3CAM
#include "common/see3cam_cu40.h"
#endif

namespace Eye {
BeeEye::BeeEye(const CameraInfo *vid, int vidDeviceNum)
{
    auto unwrapper = OpenCVUnwrap360::loadFromFile(
            vid->configFile, cv::Size(vid->width, vid->height));
    m_Unwrapper = std::unique_ptr<OpenCVUnwrap360>(unwrapper);

    // create camera object
    if (vidDeviceNum != -1 || vid->deviceURL) {
#ifndef NO_SEE3CAM
        if (vid->isSee3Cam) {
            std::cout << "Opening /dev/video" + std::to_string(vidDeviceNum)
                      << std::endl;
            auto see3cam = new See3CAM_CU40(
                    "/dev/video" + std::to_string(vidDeviceNum),
                    See3CAM_CU40::Resolution::_1280x720);
            see3cam->setBrightness(20);

            m_Camera = std::unique_ptr<VideoIn::VideoInput>(see3cam);
        } else {
#endif
            VideoIn::OpenCVInput *cam;
            if (vid->deviceURL) {
                cam = new VideoIn::OpenCVInput(vid->deviceURL);
            } else {
                cam = new VideoIn::OpenCVInput(vidDeviceNum);
            }

            m_Camera = std::unique_ptr<VideoIn::VideoInput>(cam);
#ifndef NO_SEE3CAM
        }
#endif
    }

    m_Camera->setOutputSize(unwrapper->m_CameraResolution);

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

    m_ImUnwrap.create(unwrapper->m_UnwrappedResolution, CV_8UC3);
    m_ImEye.create(outSize, CV_8UC3);
}

bool
BeeEye::getImage(cv::Mat &imorig)
{
    // read frame from camera
    return m_Camera && m_Camera->readFrame(imorig);
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
BeeEye::getEyeView(cv::Mat &view)
{
    if (!getImage(m_ImOrig)) {
        return false;
    }

    m_Unwrapper->unwrap(m_ImUnwrap, m_ImOrig);
    getEyeView(view, m_ImUnwrap);
    return true;
}

bool
BeeEye::readFrame(cv::Mat &view)
{
    return getEyeView(view);
}
}
