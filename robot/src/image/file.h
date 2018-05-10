#pragma once

#include "eye/beeeye.h"
#include <iostream>
#include <opencv2/core/core.hpp>

namespace Image {

/* try to process file, return false on error (e.g. file doesn't exist) */
bool
processFile(const char *filepath)
{
    cv::Mat im = cv::imread(filepath, CV_LOAD_IMAGE_COLOR);
    if (!im.data) {
        return false;
    }

    std::cout << "Processing " << filepath << std::endl;

    vid_t dummy;
    dummy.dev_char = nullptr;
    dummy.dev_int = -1;
    dummy.width = im.cols;
    dummy.height = im.rows;
    dummy.yaml_file = "webcam.yaml";
    Eye::BeeEye eye(&dummy);

    cv::Mat unwrap;
    eye.getUnwrappedImage(unwrap, im);

    std::vector<int> imparams;
    imparams.push_back(CV_IMWRITE_JPEG_QUALITY);
    imparams.push_back(100);

    std::string uwpath = std::string(filepath) + ".unwrap.jpg";
    std::cout << "Saving unwrapped image to " << uwpath << std::endl;
    cv::imwrite(uwpath, unwrap, imparams);

    cv::Mat view;
    eye.getEyeView(view, unwrap);
    std::string eyepath = std::string(filepath) + ".beeeye.jpg";
    std::cout << "Saving bee eye view to " << eyepath << std::endl;
    cv::imwrite(eyepath, view, imparams);

    return true;
}
}
