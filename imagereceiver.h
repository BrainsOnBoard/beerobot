#pragma once

#include "imagecommon.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <opencv2/opencv.hpp>

class ImageReceiver {
public:
    ImageReceiver();
    virtual ~ImageReceiver();
    void read(cv::Mat &view);
private:
    int listenfd = -1;
    sockaddr_in serv_addr;
    uchar buff[MAX_UDP_PACKET_SIZE];
    vector<uchar> lastbuff;
    packinfo lastinfo;
};