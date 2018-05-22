#pragma once

#include <chrono>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "os/net.h"

namespace Net {
class Socket
{
public:
    static const size_t DefaultBufferSize = 512;
    static const int DefaultListenPort = 2000;
    static const bool PrintDebug = true;

    Socket(bool print = PrintDebug)
      : m_Print(print)
    {}

    Socket(socket_t sock, bool print = PrintDebug)
      : m_Print(print)
    {
        setSocket(sock);
    }

    virtual ~Socket()
    {
        if (m_Socket != INVALID_SOCKET) {
            close(m_Socket);
        }
    }

    socket_t getSocket() const
    {
        return m_Socket;
    }

    std::vector<std::string> readCommand()
    {
        std::istringstream iss(readLine());
        std::vector<std::string> results(
                std::istream_iterator<std::string>{ iss },
                std::istream_iterator<std::string>());
        return results;
    }

    /*
     * Read a single line in. This function's probably not terribly robust.
     */
    std::string readLine()
    {
        checkSocket();

        int len =
                ::OS::Net::readBlocking(m_Socket, m_Buffer, DefaultBufferSize);
        if (len == -1) {
            throw std::runtime_error("Could not read line from socket");
        }
        if ((size_t) len > DefaultBufferSize) {
            throw std::runtime_error("Line read from socket was too long");
        }

        char *last = m_Buffer;
        for (; *last && *last != '\n'; last++)
            ;
        *last = 0;

        if (m_Print) {
            std::cout << "<<< " << m_Buffer << std::endl;
        }

        return std::string(m_Buffer);
    }

    /* Send a message, return false on error */
    void send(const std::string &msg)
    {
        checkSocket();

        int ret = ::send(
                m_Socket, msg.c_str(), (socklen_t) msg.length(), MSG_NOSIGNAL);
        if (ret == -1) {
            throw std::runtime_error("Could not send message from socket");
        }

        if (m_Print) {
            std::cout << ">>> " << msg;
        }
    }

    void setSocket(socket_t sock)
    {
        m_Socket = sock;
        checkSocket();
    }

private:
    char m_Buffer[DefaultBufferSize + 1];
    bool m_Print;
    socket_t m_Socket = INVALID_SOCKET;

    void inline checkSocket()
    {
        if (m_Socket == INVALID_SOCKET) {
            throw std::runtime_error("Bad socket");
        }
    }
};
}
