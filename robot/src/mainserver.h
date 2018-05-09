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
    int listenfd;
    Motor *mtr;
    void run();
};
}