#pragma once

#include "beeeye.h"
#include <opencv2/core/core.hpp>
#include <iostream>

/* try to process file, return false on error (e.g. file doesn't exist) */
bool process_file(const char* filepath)
{
    cv::Mat im = cv::imread(filepath, CV_LOAD_IMAGE_COLOR);
    if (!im.data) {
        return false;
    }

    std::cout << "Processing " << filepath << std::endl;

    vid_t dummy;
    dummy.dev_char = NULL;
    dummy.dev_int = -1;
    dummy.width = im.cols;
    dummy.height = im.rows;
    dummy.ini_file = "beerobot_usb.ini";
    BeeEye eye(&dummy);

    cv::Mat unwrap;
    eye.get_unwrapped_image(unwrap, im);

    std::vector<int> imparams;
    imparams.push_back(CV_IMWRITE_JPEG_QUALITY);
    imparams.push_back(100);

    std::string uwpath = std::string(filepath) + ".unwrap.jpg";
    std::cout << "Saving unwrapped image to " << uwpath << std::endl;
    cv::imwrite(uwpath, unwrap, imparams);

    cv::Mat view;
    eye.get_eye_view(view, unwrap);
    std::string eyepath = std::string(filepath) + ".beeeye.jpg";
    std::cout << "Saving bee eye view to " << eyepath << std::endl;
    cv::imwrite(eyepath, view, imparams);

    return true;
}
