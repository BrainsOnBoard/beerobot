#pragma once

// C++ includes
#include <memory>
#include <string>

// GeNN robotics includes
#include "robots/motor.h"

// local includes
#include "imagesender.h"
#include "socket.h"

using namespace GeNNRobotics::Robots;

namespace Net {
class MainServer
{
public:
    static void runServer(std::shared_ptr<Motor> &motor);

    MainServer(std::shared_ptr<Motor> motor,
               int port = Socket::DefaultListenPort);
    virtual ~MainServer();

private:
    socket_t m_ListenSocket = INVALID_SOCKET;
    std::shared_ptr<Motor> m_Motor;
    void run();
};

/*
 * Create a server to send motor commands
 */
MainServer::MainServer(std::shared_ptr<Motor> motor, int port)
  : m_Motor(motor)
{
    struct sockaddr_in addr;
    int on = 1;

    // needed for Windows
    WSAStartup();

    m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_ListenSocket == INVALID_SOCKET) {
        goto error;
    }

#ifndef _WIN32
    if (setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) <
        0) {
        goto error;
    }
#endif

    memset(&addr, '0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(m_ListenSocket, (const sockaddr *) &addr, (int) sizeof(addr))) {
        goto error;
    }
    if (listen(m_ListenSocket, 10)) {
        goto error;
    }
    std::cout << "Listening on port " << port << std::endl;

    return;

error:
    std::cerr << "Error (" << errno << "): Could not bind to port " << port
              << std::endl;
    exit(1);
}

/* Stop listening */
MainServer::~MainServer()
{
    if (m_ListenSocket != INVALID_SOCKET) {
        close(m_ListenSocket);
    }

    // needed for Windows
    WSACleanup();
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
    while (true) {
        // wait for incoming TCP connection
        std::cout << "Waiting for incoming connection..." <<std::endl;
        Socket sock(accept(m_ListenSocket, (sockaddr *) &addr, &addrlen));
        sock.send("HEY\n");

        // convert IP to string
        char saddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, (void *) &addr.sin_addr, saddr, addrlen);
        std::cout << "Incoming connection from " << saddr << std::endl;

        // our destination IP is the same IP as the current connection
        dest.sin_addr = addr.sin_addr;

        // start ImageSending thread
        std::thread tsend(ImageSender::startSending, &dest);

        float left, right;
        std::string msg;
        // TODO: implement some kind of way to quit this loop
        while (true) {
            msg = sock.readLine();

            // driving command (e.g. TNK 0.5 0.5)
            if (msg.compare(0, 4, "TNK ") == 0) {
                // second space separates left and right parameters
                size_t space = msg.rfind(' ');
                if (space == std::string::npos)
                    throw std::runtime_error("Error: Bad command");

                // parse strings to floats
                left = stof(msg.substr(4, space - 4));
                right = stof(msg.substr(space + 1));

                // send motor command
                m_Motor->tank(left, right);
            } else if (msg.compare(0, 3, "BYE") == 0) {
                // client closing connection
                break;
            } else { // no other commands supported
                throw std::runtime_error("Error: Unknown command received");
            }
        }

        // stop ImageSender thread
        ImageSender::m_Running = false;
        tsend.join();
    }
}

void
MainServer::runServer(std::shared_ptr<Motor> &motor)
{
    MainServer server(motor);
    server.run();
}
}
