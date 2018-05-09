#ifdef _WIN32
#define _WIN32_WINNT 0x600
#include <ws2tcpip.h>
#endif

#include <cstring>
#include <iostream>

#include "mainclient.h"

namespace Net {

/* Create client, connect to host on MAIN_PORT over TCP */
MainClient::MainClient(const std::string host)
{
    // Create socket
    m_Fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_Fd < 0) {
        throw std::runtime_error("Cannot open socket");
    }

        // Create socket address structure
#ifdef _WIN32
    in_addr addr;
    if (inet_pton(AF_INET, host.c_str(), &addr)) {
        throw std::runtime_error("Cannot parse address: " + host);
    }
#else
    in_addr addr = { .s_addr = inet_addr(host.c_str()) };
#endif
    sockaddr_in destAddress = { .sin_family = AF_INET,
                                .sin_port = htons(MAIN_PORT),
                                .sin_addr = addr };

    // Connect socket
    if (connect(m_Fd,
                reinterpret_cast<sockaddr *>(&destAddress),
                sizeof(destAddress)) < 0) {
        throw std::runtime_error("Cannot connect socket to " + host + ":" +
                                 std::to_string(MAIN_PORT));
    }

    std::cout << "Opened socket" << std::endl;

    // NB: this will give info about "image server" in future
    if (readline(m_Fd, m_Buffer) == -1)
        throw std::runtime_error(std::string("Error: ") + strerror(errno));
}

/* Destructor: send BYE message and close connection */
MainClient::~MainClient()
{
    if (m_Fd >= 0) {
        send(m_Fd, "BYE\n", 4);
        close(m_Fd);
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
    if (!send(m_Fd, m_Buffer, len)) {
        throw std::runtime_error(
                std::string("Could not send steering command: ") +
                strerror(errno));
    }

    // store current left/right values to compare next time
    m_OldLeft = left;
    m_OldRight = right;
}
}