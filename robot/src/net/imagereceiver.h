#pragma once

#include "image/common.h"
#include "os/net.h"
#include "readable.h"

#include <errno.h>
#include <opencv2/opencv.hpp>

#ifndef _MSC_VER
#include <unistd.h>
#endif

namespace Net {
class ImageReceiver : public Readable
{
public:
    ImageReceiver();
    virtual ~ImageReceiver();
    virtual bool read(cv::Mat &view) override;

private:
    socket_t m_Fd = -1;
    mybuff_t m_Buffer[Image::MAX_UDP_PACKET_SIZE];
    std::vector<mybuff_t> m_LastBuffer;
    int m_LastId;
};
}
