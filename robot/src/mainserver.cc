#include "mainserver.h"
#include "socketcommon.h"

#include <iostream>
#include <string.h>
#include <cstring>
#include <thread>

#ifdef _WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <ws2tcpip.h>
#endif

using namespace std;

/* Create a server listening on MAIN_PORT (TCP), sending motor commands to *mtr */
MainServer::MainServer(Motor *mtr) : mtr(mtr)
{
    struct sockaddr_in addr;
    int on = 1;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        goto error;
    }
#ifndef _WIN32
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) < 0) {
        goto error;
    }
#endif

    memset(&addr, '0', sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(MAIN_PORT);

    if (bind(listenfd, (sockaddr*) & addr, sizeof (addr))) {
        goto error;
    }
    if (listen(listenfd, 10)) {
        goto error;
    }
    cout << "Listening on port " << MAIN_PORT << endl;

    return;

error:
    cerr << "Error (" << errno << "): Could not bind to port " << MAIN_PORT << endl;
    exit(1);
}

/* Stop listening */
MainServer::~MainServer()
{
    close(listenfd);
}

/*
 * Keep accepting connections and parsing input for ever. Can only handle
 * one connection at a time.
 */
void MainServer::run()
{
    // for incoming connection
    sockaddr_in addr;
    socklen_t addrlen = sizeof (addr);

    // for outgoing (ImageSender) connection (UDP)
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(IMAGE_PORT);

    // thread to run ImageSender on
    pthread_t tisend;

    // loop for ever
    for (;;) {
        // wait for incoming TCP connection
        cout << "Waiting for incoming connection..." << endl;
        int connfd = accept(listenfd, (sockaddr*) & addr, &addrlen);
        if (!send(connfd, "HEY\n", 4))
            throw new runtime_error("Could not write to socket");

        // convert IP to string
        char saddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, (void*) &addr.sin_addr, saddr, addrlen);
        cout << "Incoming connection from " << saddr << endl;

        // our destination IP is the same IP as the current connection
        dest.sin_addr = addr.sin_addr;

        // start ImageSending thread
        pthread_create(&tisend, NULL, ImageSender::start_sending, (void*) &dest);

        // for reading in data
        char buff[MAIN_BUFFSIZE];
        string sbuff;
        int len;

        // motor command
        float left, right;
        while ((len = readline(connfd, buff)) > 0) {
            sbuff = string(buff);
            if (sbuff.compare(0, 4, "TNK ") == 0) { // driving command (e.g. TNK 0.5 0.5)
                // second space separates left and right parameters
                size_t space = sbuff.rfind(' ');
                if (space == string::npos)
                    throw new runtime_error("Error: Bad command");

                // parse strings to floats
                left = stof(sbuff.substr(4, space - 4));
                right = stof(sbuff.substr(space + 1));

                // send motor command
                mtr->tank(left, right);
            } else if (sbuff.compare(0, 3, "BYE") == 0) // client closing connection
                break;
            else // no other commands supported
                throw new runtime_error("Error: Unknown command received");
        }
        if (len == -1) // loop ended because an error occurred
            throw new runtime_error(string("Error: ") + strerror(errno));

        // close current connection
        close(connfd);
        cout << "Connection closed" << endl;

        // stop ImageSender thread
        ImageSender::running = false;
        pthread_join(tisend, NULL);
    }
}

void MainServer::run_server(Motor *mtr)
{
    MainServer srv(mtr);
    srv.run();
}