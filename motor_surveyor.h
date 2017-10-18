#pragma once

// Standard C++ includes
#include <string>
#include <stdexcept>

// Standard C includes
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

// POSIX includes
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

//----------------------------------------------------------------------------
// MotorSurveyor
//----------------------------------------------------------------------------

using namespace std;

class MotorSurveyor : public Motor {
public:

    MotorSurveyor(const std::string &address, unsigned int port)
    {
        // Create socket
        m_Socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_Socket < 0) {
            throw std::runtime_error("Cannot open socket");
        }

        // Create socket address structure
        sockaddr_in destAddress = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr =
            { .s_addr = inet_addr(address.c_str())}
        };

        // Connect socket
        if (connect(m_Socket, reinterpret_cast<sockaddr*> (&destAddress), sizeof (destAddress)) < 0) {
            throw std::runtime_error("Cannot connect socket to " + address + ":" + std::to_string(port));
        }

        cout << "Connected to " << address << ":" << port << endl;
    }

    ~MotorSurveyor()
    {
        if (m_Socket > 0) {
            close(m_Socket);
        }
    }

    //----------------------------------------------------------------------------
    // Public API
    //----------------------------------------------------------------------------

    virtual void tank(float left, float right)
    {
        // Clamp left and right within normalised range
        left = std::min(1.0f, std::max(-1.0f, left));
        right = std::min(1.0f, std::max(-1.0f, right));

        // Scale and convert to int
        int leftInt = (int) std::round(left * 100.0f);
        int rightInt = (int) std::round(right * 100.0f);

        // Generate command string
        char command[17];
        snprintf(command, 17, "#tnk(%d,%d)\n", leftInt, rightInt);

#ifdef DRIVE_TRACE
        std::cout << "command: " << command;
#endif
        // Write command to socket
        if (write(m_Socket, command, strlen(command)) < 0) {
            throw std::runtime_error("Cannot write to socket");
        }
    }

private:
    //----------------------------------------------------------------------------
    // Private members
    //----------------------------------------------------------------------------
    int m_Socket = 0;
};
