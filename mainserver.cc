#include "mainserver.h"
#include "socketcommon.h"

#include <iostream>
#include <string.h>
#include <cstring>
#include <thread>

using namespace std;

MainServer::MainServer(int port, Motor *mtr) : mtr(mtr)
{
    struct sockaddr_in serv_addr;
    int on = 1;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        goto error;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) < 0) {
        goto error;
    }

    memset(&serv_addr, '0', sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(listenfd, (sockaddr*) & serv_addr, sizeof (serv_addr))) {
        goto error;
    }
    if (listen(listenfd, 10)) {
        goto error;
    }
    cout << "Listening on port " << port << endl;

    return;

error:
    cerr << "Error (" << errno << "): Could not bind to port " << port << endl;
    exit(1);
}

MainServer::~MainServer()
{
    close(listenfd);
}

void MainServer::run()
{
    sockaddr_in addr;
    socklen_t addrlen = sizeof (addr);
    int connfd = accept(listenfd, (sockaddr*) & addr, &addrlen);
    if (!send(connfd, "HI\n", 3))
        throw new runtime_error("Could not write to socket");

    char saddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (const void*) &addr.sin_addr, saddr, addrlen);
    cout << "Incoming connection from " << saddr << endl;

    sockaddr_in *dest = new sockaddr_in;
    dest->sin_family = AF_INET;
    dest->sin_addr = addr.sin_addr;
    dest->sin_port = htons(IMAGE_PORT);
    pthread_t tisend;
    pthread_create(&tisend, NULL, ImageSender::start_sending, (void*) dest);

    char buff[buffsize];
    string sbuff;
    int len;
    float left, right;
    while ((len = readline(connfd, buff)) > 0) {
        sbuff = string(buff);
        if (sbuff.compare(0, 4, "TNK ") != 0)
            throw new runtime_error("Error: Unknown command received");

        size_t space = sbuff.rfind(' ');
        if (space == string::npos)
            throw new runtime_error("Error: Bad command");

        left = stof(sbuff.substr(4, space - 4));
        right = stof(sbuff.substr(space + 1));
        mtr->tank(left, right);
    }
    if (len == -1)
        throw new runtime_error(string("Error: ") + strerror(errno));

    close(connfd);
}

void MainServer::run_server(Motor *mtr)
{
    MainServer srv(2000, mtr);
    srv.run();
}