#pragma once

#include "motor.h"
#include "socketcommon.h"

#include <string>
#include <limits>

using namespace std;

class MainClient : public Motor {
public:
    MainClient(const string host, const int port);
    virtual ~MainClient();
    virtual void tank(float left, float right);
private:
    int connfd = -1;
    float oldleft = numeric_limits<float>::quiet_NaN();
    float oldright = numeric_limits<float>::quiet_NaN();
    char buff[buffsize];
};