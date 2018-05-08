#pragma once

#include "common/motor.h"
#include "socketcommon.h"

#include <string>
#include <limits>

class MainClient : public Motor {
public:
    MainClient(const std::string host);
    virtual ~MainClient();
    virtual void tank(float left, float right);
private:
    int connfd = -1;
    float oldleft = std::numeric_limits<float>::quiet_NaN();
    float oldright = std::numeric_limits<float>::quiet_NaN();
    char buff[MAIN_BUFFSIZE];
};
