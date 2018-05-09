#include "mainserver.h"
#include "socketcommon.h"

#include <cstring>
#include <iostream>
#include <string.h>
#include <thread>

#ifdef _WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <ws2tcpip.h>
#endif

namespace Net {

/*
 * Create a server listening on MAIN_PORT (TCP), sending motor commands to *mtr
 */
MainServer::MainServer(Motor *mtr)
  : m_Motor(mtr)
{
    struct sockaddr_in addr;
    int on = 1;

    if ((m_Fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        goto error;
    }
#ifndef _WIN32
    if (setsockopt(m_Fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        goto error;
    }
#endif

    memset(&addr, '0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(MAIN_PORT);

    if (bind(m_Fd, (sockaddr *) &addr, sizeof(addr))) {
        goto error;
    }
    if (listen(m_Fd, 10)) {
        goto error;
    }
    std::cout << "Listening on port " << MAIN_PORT << std::endl;

    return;

error:
    std::cerr << "Error (" << errno << "): Could not bind to port " << MAIN_PORT
              << std::endl;
    exit(1);
}

/* Stop listening */
MainServer::~MainServer()
{
    close(m_Fd);
}

/*
 * Keep accepting connections and parsing input for ever. Can only handle
 * one connection at a time.
 */
void
MainServer::run()
{
    // for incoming connection
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    // for outgoing (ImageSender) connection (UDP)
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(Image::IMAGE_PORT);

    // loop for ever
    for (;;) {
        // wait for incoming TCP connection
        std::cout << "Waiting for incoming connection..." << std::endl;
        int connfd = accept(m_Fd, (sockaddr *) &addr, &addrlen);
        if (!send(connfd, "HEY\n", 4)) {
            throw std::runtime_error("Could not write to socket");
        }

        // convert IP to string
        char saddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, (void *) &addr.sin_addr, saddr, addrlen);
        std::cout << "Incoming connection from " << saddr << std::endl;

        // our destination IP is the same IP as the current connection
        dest.sin_addr = addr.sin_addr;

        // start ImageSending thread
        std::thread tsend(ImageSender::startSending, &dest);

        // for reading in data
        char buff[MAIN_BUFFSIZE];
        std::string sbuff;
        int len;

        // motor command
        float left, right;
        while ((len = readline(connfd, buff)) > 0) {
            sbuff = std::string(buff);
            if (sbuff.compare(0, 4, "TNK ") ==
                0) { // driving command (e.g. TNK 0.5 0.5)
                // second space separates left and right parameters
                size_t space = sbuff.rfind(' ');
                if (space == std::string::npos)
                    throw std::runtime_error("Error: Bad command");

                // parse strings to floats
                left = stof(sbuff.substr(4, space - 4));
                right = stof(sbuff.substr(space + 1));

                // send motor command
                m_Motor->tank(left, right);
            } else if (sbuff.compare(0, 3, "BYE") ==
                       0) // client closing connection
                break;
            else // no other commands supported
                throw std::runtime_error("Error: Unknown command received");
        }
        if (len == -1) // loop ended because an error occurred
            throw std::runtime_error(std::string("Error: ") + strerror(errno));

        // close current connection
        close(connfd);
        std::cout << "Connection closed" << std::endl;

        // stop ImageSender thread
        ImageSender::m_Running = false;
        tsend.join();
    }
}

void
MainServer::runServer(Motor *mtr)
{
    MainServer srv(mtr);
    srv.run();
}
}
