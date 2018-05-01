#pragma once

#include "imagecommon.h"
#include "readable.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    uchar buff[MAX_UDP_PACKET_SIZE];
    std::vector<uchar> lastbuff;
    int lastid;
};
