#pragma once

#include "imagecommon.h"
#include "readable.h"

#ifdef _WIN32
#include <winsock2.h>
typedef char mybuff_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
typedef uchar mybuff_t;
#endif

#include <errno.h>
#include <opencv2/opencv.hpp>
#include <unistd.h>

namespace Net {
class ImageReceiver : public Readable
{
public:
    ImageReceiver();
    virtual ~ImageReceiver();
    virtual bool read(cv::Mat &view) override;

private:
    int m_Fd = -1;
    mybuff_t m_Buffer[MAX_UDP_PACKET_SIZE];
    std::vector<mybuff_t> m_LastBuffer;
    int m_LastId;
};
}
