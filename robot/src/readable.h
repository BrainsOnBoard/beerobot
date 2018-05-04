#pragma once

#include <opencv2/opencv.hpp>

class Readable {
public:
    virtual bool read(cv::Mat *image) = 0;
};
