#pragma once

#include "beeeye.h"
#include <opencv2/core/core.hpp>
#include <iostream>

/* try to process file, return false on error (e.g. file doesn't exist) */
bool process_file(const char* filepath)
{
    Mat im = imread(filepath, CV_LOAD_IMAGE_COLOR);
    if (!im.data) {
        return false;
    }

    cout << "Processing " << filepath << endl;

    vid_t dummy;
    dummy.dev_char = NULL;
    dummy.dev_int = -1;
    dummy.width = im.cols;
    dummy.height = im.rows;
    dummy.ini_file = "beerobot_usb.ini";
    BeeEye eye(&dummy);

    Mat unwrap;
    eye.get_unwrapped_image(unwrap, im);

    vector<int> imparams;
    imparams.push_back(CV_IMWRITE_JPEG_QUALITY);
    imparams.push_back(100);

    string uwpath = string(filepath) + ".unwrap.jpg";
    cout << "Saving unwrapped image to " << uwpath << endl;
    imwrite(uwpath, unwrap, imparams);

    Mat view;
    eye.get_eye_view(view, unwrap);
    string eyepath = string(filepath) + ".beeeye.jpg";
    cout << "Saving bee eye view to " << eyepath << endl;
    imwrite(eyepath, view, imparams);

    return true;
}
