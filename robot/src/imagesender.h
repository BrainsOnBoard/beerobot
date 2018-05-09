#pragma once

#include "eye/beeeye.h"
#include "image/common.h"

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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
typedef uchar buff_t;
#endif

#include <errno.h>
#include <unistd.h>

namespace Net {
class ImageSender
{
public:
    static bool m_Running;
    static void startSending(const sockaddr_in *destAddress);

    ImageSender(const sockaddr_in *destAddress);
    virtual ~ImageSender();

private:
    static ImageSender *Instance;

    void run();

    int m_Fd = -1;
    const sockaddr_in *m_DestAddr;
    Eye::BeeEye m_Eye;
};
}
