#pragma once

#include "beeeye.h"
#include "imagecommon.h"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif
#include <winsock2.h>

#define MSG_NOSIGNAL 0
#define INET_ADDRSTRLEN 22
typedef const char buff_t;
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef uchar buff_t;
#endif

#include <unistd.h>
#include <errno.h>

class ImageSender {
public:
    static bool running;
    static void* start_sending(void *destAddress);

    ImageSender(const sockaddr_in *destAddress);
    virtual ~ImageSender();
private:
    static ImageSender *Instance;

    void run();

    int connfd = -1;
    const sockaddr_in *dest;
    BeeEye eye;
};
