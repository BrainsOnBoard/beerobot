#pragma once

#include "beeeye.h"
#include "imagecommon.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
