#include "beeeye.h"
#include "gigerdatacam.h"

BeeEye::BeeEye(vid_t* vid) : params(vid)
{
    if (vid->dev_int != -1) {
        if (vid->dev_char) {
            cap = new VideoCapture(vid->dev_char);
        } else {
            cap = new VideoCapture(vid->dev_int);
        }
        if (!cap->isOpened()) {
            cerr << "Error: Could not open webcam" << endl;
            exit(1);
        }

        // set resolution
        cap->set(CAP_PROP_FRAME_WIDTH, params.ssrc.width);
        cap->set(CAP_PROP_FRAME_HEIGHT, params.ssrc.height);
    }

    // create x and y pixel maps
    Size sz_out(eye_size[0], eye_size[1]);
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
    if (cap) {
        cap->release();
        delete cap;
    }
}

bool BeeEye::get_image(Mat &imorig)
{
    // read frame from camera
    (*cap) >> imorig;
    return imorig.size().width != 0;
}

void BeeEye::get_unwrapped_image(Mat &imunwrap, Mat &imorig)
{
    remap(imorig, imunwrap, params.map_x, params.map_y, INTER_NEAREST);
}

void BeeEye::get_eye_view(Mat &view, Mat &imunwrap)
{
    /* perform two transformations:
     * - unwrap panoramic image
     * - bee eye
     *
     * (this could be done in a single step with the correct pixel map, but
     * this way is easier for now and works...) */
    remap(imunwrap, imeye, map_x, map_y, INTER_NEAREST);

    // resize the image we get out so it's large enough to see properly
    Size sz(970, 1046);
    resize(imeye, view, sz, 0, 0, INTER_LINEAR);
}

bool BeeEye::get_eye_view(Mat& view)
{
    if (!get_image(imorig)) {
        return false;
    }

    get_unwrapped_image(imunwrap, imorig);
    get_eye_view(view, imunwrap);
    return true;
}