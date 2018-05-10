#pragma once

#include "eye/beeeye.h"
#include "image/common.h"
#include "os/net.h"

#include <errno.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif

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

    socket_t m_Fd = -1;
    const sockaddr_in *m_DestAddr;
    Eye::BeeEye m_Eye;
};
}
