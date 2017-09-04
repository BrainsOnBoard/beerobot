/*
 * File:   beeeye.cc
 * Author: ad374
 *
 * Created on 04 September 2017, 13:33
 */

#include "beeeye.h"
#include "gigerdatacam.h"

BeeEye::BeeEye() : cap(VIDEO_DEV)
{
    if (!cap.isOpened()) {
        cerr << "Error: Could not open webcam" << endl;
        exit(1);
    }

    // set resolution
    cap.set(CAP_PROP_FRAME_WIDTH, params.ssrc.width);
    cap.set(CAP_PROP_FRAME_HEIGHT, params.ssrc.height);

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

inline bool BeeEye::get_image(Mat &imorig)
{
    // read frame from camera
    cap >> imorig;
    return imorig.size().width != 0;
}

void BeeEye::get_eye_view(Mat &view, Mat &imorig)
{
    /* perform two transformations:
     * - unwrap panoramic image
     * - bee eye
     *
     * (this could be done in a single step with the correct pixel map, but
     * this way is easier for now and works...) */
    remap(imorig, imunwrap, params.map_x, params.map_y, INTER_NEAREST);
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

    get_eye_view(view, imorig);
    return true;
}

/* get the number for a camera with a given name (-1 if not found) */
int get_camera_by_name(const char* name)
{
    char cname[4096];

    // iterate through devices video0, video1 etc. reading the device name from sysfs
    // until the correct device is found
    for (int i = 0;; i++) {
        string vfn = "/sys/class/video4linux/video" + to_string(i) + "/name";
        ifstream file(vfn, ios::in);
        if (!file.is_open()) {
            cout << "Warning: Could not find video device " << name << ". Using default instead." << endl;
            return 0;
        }

        file.read(cname, sizeof (cname));
        cname[file.gcount() - 1] = 0; // delete the last char, which is always newline
        file.close();

        if (strcmp(name, cname) == 0) { // we've found the correct device
            return i;
        }
    }
}