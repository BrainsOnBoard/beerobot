#pragma once

#include "common/motor.h"
#include "imagesender.h"
#include <memory>

namespace Net {
class MainServer
{
public:
    static void runServer(std::shared_ptr<Motor> &motor);

    MainServer(std::shared_ptr<Motor> motor);
    virtual ~MainServer();

private:
    socket_t m_Socket = INVALID_SOCKET;
    std::shared_ptr<Motor> m_Motor;
    void run();
};
}
