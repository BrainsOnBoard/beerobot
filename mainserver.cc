#include "mainserver.h"
#include <iostream>

// for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


using namespace std;

MainServer::MainServer(int port)
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

    if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr))) {
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
    int connfd = accept(listenfd, NULL, NULL);
    send(connfd, "HI\n", 3, MSG_NOSIGNAL);
    close(connfd);
}