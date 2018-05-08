#include "beeeye.h"
#include "gigerdatacam.h"

#ifndef _WIN32
// GeNN robotics includes
#include "common/see3cam_cu40.h"
#endif

using namespace cv;
using namespace std;

BeeEye::BeeEye(vid_t* vid) : params(vid)
{
    if (vid->dev_int != -1 || vid->dev_char != nullptr) {
#ifndef _WIN32
        if (vid->is_see3cam) {
            std::cout << "Opening /dev/video" + std::to_string(vid->dev_int) << std::endl;
            see3cam = new See3CAM_CU40("/dev/video" + std::to_string(vid->dev_int), See3CAM_CU40::Resolution::_1280x720);
            see3cam->setBrightness(20);
        } else {
#endif
            if (vid->dev_char) {
                cap = new cv::VideoCapture(vid->dev_char);
            } else {
                cap = new cv::VideoCapture(vid->dev_int);
            }
            if (!cap->isOpened()) {
                std::cerr << "Error: Could not open webcam" << std::endl;
                exit(1);
            }

            // set resolution
            cap->set(cv::CAP_PROP_FRAME_WIDTH, params.ssrc.width);
            cap->set(cv::CAP_PROP_FRAME_HEIGHT, params.ssrc.height);
#ifndef _WIN32
        }
#endif
    }

    // create x and y pixel maps
    cv::Size sz_out(eye_size[0], eye_size[1]);
    map_x.create(sz_out, CV_32FC1);
    map_y.create(sz_out, CV_32FC1);
    for (int i = 0; i < gdataLength; i++) {
        // left eye
        map_x.at<float>(gdata[i][3], 15 + gdata[i][2]) = floor(gdata[i][0]);
        map_y.at<float>(gdata[i][3], 15 + gdata[i][2]) = floor(gdata[i][1]);

        // right eye
        map_x.at<float>(gdata[i][3], 720 - 316 - eye_size[0] - gdata[i][2]) = gim_size[0] - floor(gdata[i][0]);
        map_y.at<float>(gdata[i][3], 720 - 316 - eye_size[0] - gdata[i][2]) = floor(gdata[i][1]);
    }

    // create pixel maps for unwrapping panoramic images
    params.generate_map();

    imunwrap.create(params.sdst, CV_8UC3);
    imeye.create(sz_out, CV_8UC3);
}

BeeEye::~BeeEye()
{
    // stop reading from camera and free memory for object
    if (cap) {
        cap->release();
        delete cap;
#ifndef _WIN32
    } else if (see3cam) {
        delete see3cam;
#endif
    }
}

bool BeeEye::get_image(cv::Mat &imorig)
{
    // read frame from camera
    if (cap) {
        (*cap) >> imorig;

        return imorig.size().width != 0;
#ifndef _WIN32
    } else if (see3cam) {
        if (imorig.size().width == 0) {
            imorig.create(params.ssrc, CV_8UC3);
        }
        return see3cam->captureSuperPixelWBU30(imorig);
#endif
    }

    return false;
}

void BeeEye::get_unwrapped_image(cv::Mat &imunwrap, cv::Mat &imorig)
{
    remap(imorig, imunwrap, params.map_x, params.map_y, cv::INTER_NEAREST);
}

void BeeEye::get_eye_view(cv::Mat &view, cv::Mat &imunwrap)
{
    /* perform two transformations:
     * - unwrap panoramic image
     * - bee eye
     *
     * (this could be done in a single step with the correct pixel map, but
     * this way is easier for now and works...) */
    remap(imunwrap, imeye, map_x, map_y, cv::INTER_NEAREST);

    // resize the image we get out so it's large enough to see properly
    cv::Size sz(970, 1046);
    resize(imeye, view, sz, 0, 0, cv::INTER_LINEAR);
}

bool BeeEye::get_eye_view(cv::Mat& view)
{
    if (!get_image(imorig)) {
        return false;
    }

    get_unwrapped_image(imunwrap, imorig);
    get_eye_view(view, imunwrap);
    return true;
}

bool BeeEye::read(cv::Mat *view)
{
    return this->get_eye_view(*view);
}
