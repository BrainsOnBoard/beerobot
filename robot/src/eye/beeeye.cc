#include "beeeye.h"
#include "gigerdatacam.h"

#ifndef _WIN32
// GeNN robotics includes
#include "common/see3cam_cu40.h"
#endif

namespace Eye {
BeeEye::BeeEye(vid_t *vid)
  : params(vid)
{
    if (vid->dev_int != -1 || vid->dev_char != nullptr) {
#ifndef _WIN32
        if (vid->is_see3cam) {
            std::cout << "Opening /dev/video" + std::to_string(vid->dev_int)
                      << std::endl;
            m_See3Cam = new See3CAM_CU40("/dev/video" +
                                                 std::to_string(vid->dev_int),
                                         See3CAM_CU40::Resolution::_1280x720);
            m_See3Cam->setBrightness(20);
        } else {
#endif
            if (vid->dev_char) {
                m_Cap = new cv::VideoCapture(vid->dev_char);
            } else {
                m_Cap = new cv::VideoCapture(vid->dev_int);
            }
            if (!m_Cap->isOpened()) {
                std::cerr << "Error: Could not open webcam" << std::endl;
                exit(1);
            }

            // set resolution
            m_Cap->set(cv::CAP_PROP_FRAME_WIDTH, params.ssrc.width);
            m_Cap->set(cv::CAP_PROP_FRAME_HEIGHT, params.ssrc.height);
#ifndef _WIN32
        }
#endif
    }

    // create x and y pixel maps
    cv::Size sz_out(eye_size[0], eye_size[1]);
    m_MapX.create(sz_out, CV_32FC1);
    m_MapY.create(sz_out, CV_32FC1);
    for (int i = 0; i < gdataLength; i++) {
        // left eye
        m_MapX.at<float>(gdata[i][3], 15 + gdata[i][2]) = floor(gdata[i][0]);
        m_MapY.at<float>(gdata[i][3], 15 + gdata[i][2]) = floor(gdata[i][1]);

        // right eye
        m_MapX.at<float>(gdata[i][3], 720 - 316 - eye_size[0] - gdata[i][2]) =
                gim_size[0] - floor(gdata[i][0]);
        m_MapY.at<float>(gdata[i][3], 720 - 316 - eye_size[0] - gdata[i][2]) =
                floor(gdata[i][1]);
    }

    // create pixel maps for unwrapping panoramic images
    params.generate_map();

    m_ImUnwrap.create(params.sdst, CV_8UC3);
    m_ImEye.create(sz_out, CV_8UC3);
}

BeeEye::~BeeEye()
{
    // stop reading from camera and free memory for object
    if (m_Cap) {
        m_Cap->release();
        delete m_Cap;
#ifndef _WIN32
    } else if (m_See3Cam) {
        delete m_See3Cam;
#endif
    }
}

bool
BeeEye::getImage(cv::Mat &imorig)
{
    // read frame from camera
    if (m_Cap) {
        (*m_Cap) >> imorig;

        return imorig.size().width != 0;
#ifndef _WIN32
    } else if (m_See3Cam) {
        if (imorig.size().width == 0) {
            imorig.create(params.ssrc, CV_8UC3);
        }
        return m_See3Cam->captureSuperPixelWBU30(imorig);
#endif
    }

    return false;
}

void
BeeEye::getUnwrappedImage(cv::Mat &imunwrap, cv::Mat &imorig)
{
    remap(imorig, imunwrap, params.map_x, params.map_y, cv::INTER_NEAREST);
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

    getUnwrappedImage(m_ImUnwrap, m_ImUnwrap);
    getEyeView(view, m_ImUnwrap);
    return true;
}

bool
BeeEye::read(cv::Mat &view)
{
    return getEyeView(view);
}
}
