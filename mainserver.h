#pragma once

#include "motor.h"
#include "imagesender.h"

class MainServer {
public:
    MainServer(int port, Motor* mtr);
    virtual ~MainServer();
    static void run_server(Motor *mtr);
private:
    int listenfd;
    Motor *mtr;
    void run();
};
