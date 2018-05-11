#pragma once

#include "common/motor.h"
#include "socketcommon.h"

#include <limits>
#include <string>

namespace Net {
class MainClient : public Motor
{
public:
    MainClient(const std::string host);
    virtual ~MainClient();
    virtual void tank(float left, float right);

private:
    socket_t m_Socket = INVALID_SOCKET;
    float m_OldLeft = std::numeric_limits<float>::quiet_NaN();
    float m_OldRight = std::numeric_limits<float>::quiet_NaN();
    char m_Buffer[MAIN_BUFFSIZE];
};
}
