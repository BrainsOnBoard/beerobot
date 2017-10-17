#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <opencv2/opencv.hpp>

class ImageReceiver {
public:
    ImageReceiver(int port);
    virtual ~ImageReceiver();
    void read(cv::Mat &view);
private:
    static const size_t buffsize = 65507;

    int listenfd = -1;
    sockaddr_in serv_addr;
    uchar buff[buffsize];
};