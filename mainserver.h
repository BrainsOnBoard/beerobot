#pragma once

#include "motor.h"

class MainServer {
public:
    MainServer(int port, Motor* mtr);
    virtual ~MainServer();
    static void* run_server(void* mtr);
private:
    int listenfd;
    Motor *mtr;
    void run();
};
