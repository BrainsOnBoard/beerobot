#pragma once

#include <string>
#include "motor.h"
#include "socketcommon.h"

class MainClient : public Motor {
public:
    MainClient(const std::string host, const int port);
    virtual ~MainClient();
    virtual void tank(float left, float right);
private:
    int connfd = -1;
    char buff[buffsize];
};