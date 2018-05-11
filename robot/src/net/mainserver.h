#pragma once

#include "common/motor.h"
#include "imagesender.h"

namespace Net {
class MainServer
{
public:
    static void runServer(Motor *mtr);

    MainServer(Motor *mtr);
    virtual ~MainServer();

private:
    socket_t m_Socket = INVALID_SOCKET;
    Motor *m_Motor;
    void run();
};
}
