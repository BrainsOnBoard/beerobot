#pragma once

#include "motor.h"
#include "imagesender.h"

class MainServer {
public:
    static void run_server(Motor *mtr);

    MainServer(Motor* mtr);
    virtual ~MainServer();
private:
    int listenfd;
    Motor *mtr;
    void run();
};
