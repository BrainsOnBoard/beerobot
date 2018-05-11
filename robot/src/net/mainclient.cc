#include <cstring>
#include <iostream>

#include "mainclient.h"

namespace Net {

/* Create client, connect to host on MAIN_PORT over TCP */
MainClient::MainClient(const std::string host)
{
    // Create socket
    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_Socket == INVALID_SOCKET) {
        throw std::runtime_error("Cannot open socket");
    }

    // Create socket address structure
    in_addr addr;
    addr.s_addr = inet_addr(host.c_str());
    sockaddr_in destAddress;
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = htons(MAIN_PORT);
    destAddress.sin_addr = addr;

    // Connect socket
    if (connect(m_Socket,
                reinterpret_cast<sockaddr *>(&destAddress),
                sizeof(destAddress)) < 0) {
        throw std::runtime_error("Cannot connect socket to " + host + ":" +
                                 std::to_string(MAIN_PORT));
    }

    std::cout << "Opened socket" << std::endl;

    // NB: this will give info about "image server" in future
    if (readLine(m_Socket, m_Buffer) == -1) {
        throw std::runtime_error(std::string("Error: ") + strerror(errno));
    }
}

/* Destructor: send BYE message and close connection */
MainClient::~MainClient()
{
    if (m_Socket != INVALID_SOCKET) {
        send(m_Socket, "BYE\n", 4);
        close(m_Socket);
    }
}

/* Motor command: send TNK command over TCP */
void
MainClient::tank(float left, float right)
{
    // don't send a command if it's the same as the last one
    if (left == m_OldLeft && right == m_OldRight)
        return;

    // send steering command
    int len = sprintf(m_Buffer, "TNK %g %g\n", left, right);
    if (!send(m_Socket, m_Buffer, len)) {
        throw std::runtime_error(
                std::string("Could not send steering command: ") +
                strerror(errno));
    }

    // store current left/right values to compare next time
    m_OldLeft = left;
    m_OldRight = right;
}
}
