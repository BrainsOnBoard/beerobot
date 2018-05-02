#pragma once

#include "imagecommon.h"
#include "readable.h"

#ifdef _WIN32
#include <winsock2.h>
typedef char mybuff_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef uchar mybuff_t;
#endif

#include <unistd.h>
#include <errno.h>
#include <opencv2/opencv.hpp>

class ImageReceiver : public Readable {
public:
    ImageReceiver();
    virtual ~ImageReceiver();
    virtual bool read(cv::Mat *view) override;
private:
    int listenfd = -1;
    mybuff_t buff[MAX_UDP_PACKET_SIZE];
    std::vector<mybuff_t> lastbuff;
    int lastid;
};
