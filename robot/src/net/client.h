#pragma once

// C++ includes
#include <limits>
#include <string>

// GeNN robotics includes
#include "robots/motor.h"
#include "socket.h"

namespace Net {
class MainClient : public GeNNRobotics::Robots::Motor, Socket
{
public:
    MainClient(const std::string host);
    virtual ~MainClient();
    virtual void tank(float left, float right);

private:
    float m_OldLeft = std::numeric_limits<float>::quiet_NaN();
    float m_OldRight = std::numeric_limits<float>::quiet_NaN();
    char m_Buffer[DefaultBufferSize];
};


/* Create client, connect to host on MAIN_PORT over TCP */
MainClient::MainClient(const std::string host)
{
    WSAStartup();

    // Create socket
    setSocket(socket(AF_INET, SOCK_STREAM, 0));

    // Create socket address structure
    in_addr addr;
    addr.s_addr = inet_addr(host.c_str());
    sockaddr_in destAddress;
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = htons(DefaultListenPort);
    destAddress.sin_addr = addr;

    // Connect socket
    if (connect(getSocket(),
                reinterpret_cast<sockaddr *>(&destAddress),
                sizeof(destAddress)) < 0) {
        throw std::runtime_error("Cannot connect socket to " + host + ":" +
                                 std::to_string(DefaultListenPort));
    }

    std::cout << "Opened socket" << std::endl;

    // NB: this will give info about "image server" in future
    readLine();
}

/* Destructor: send BYE message and close connection */
MainClient::~MainClient()
{
    WSACleanup();
}

/* Motor command: send TNK command over TCP */
void
MainClient::tank(float left, float right)
{
    // don't send a command if it's the same as the last one
    if (left == m_OldLeft && right == m_OldRight) {
        return;
    }

    // send steering command
    send("TNK " + std::to_string(left) + " " + std::to_string(right) + "\n");

    // store current left/right values to compare next time
    m_OldLeft = left;
    m_OldRight = right;
}
}
