#pragma once

#include "image/common.h"
#include "os/net.h"
#include "videoin/videoinput.h"

#include <errno.h>
#include <opencv2/opencv.hpp>

#ifndef _MSC_VER
#include <unistd.h>
#endif

namespace Net {
class ImageReceiver : public VideoIn::VideoInput
{
public:
    ImageReceiver();
    virtual ~ImageReceiver();
    virtual bool readFrame(cv::Mat &view);

private:
    socket_t m_Socket = INVALID_SOCKET;
    mybuff_t m_Buffer[Image::MAX_UDP_PACKET_SIZE];
    std::vector<mybuff_t> m_LastBuffer;
    int m_LastId;
};
}
