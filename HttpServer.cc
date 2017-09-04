/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpServer.cc
 * Author: ad374
 * 
 * Created on 01 September 2017, 11:33
 */

#include "HttpServer.h"
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

HttpServer::HttpServer(int port) {
    struct sockaddr_in serv_addr;
    int on = 1;

    if ((this->listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        goto error;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(this->listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        goto error;
    }
    if (listen(this->listenfd, 10)) {
        goto error;
    }
    if (setsockopt(this->listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        goto error;
    }
    cout << "Listening on port " << port << endl;
    
    return;
    
    error:
    cerr << "Error (" << errno << "): Could not bind to port " << port << endl;
    exit(1);
}

HttpServer::~HttpServer() {
}

size_t indexof(const char* str, char c) {
    size_t i = 0;
    for(; str[i] && str[i] != c; i++);
    return str[i] ? i : -1;
}

void HttpServer::serve(void (*handle_request)(int, char*)) {
    cout << "Serving forever..." << endl;
    
    char buff[1025];
    while(1) {
        int connfd = accept(this->listenfd, (struct sockaddr*)NULL, NULL);
        cout << "Accepting connection " << connfd << endl;

        int len;
        for(;;) {
            while((len = read(connfd, buff, sizeof(buff)-1)) == 0) {
                //cout << "trying to read" << endl;
                usleep(250000);
            }
            if (len == -1) {
                cerr << "Error while reading" << endl;
                break;
            }
            
            buff[len] = 0;
            cout << "<<< " << buff << endl;
            
            size_t sp = indexof(buff,' ');
            if(sp == -1 || strncmp(buff, "GET", sp)) {
                cerr << "Error: Bad message" << endl;
                continue;
            }
            
            char* src = &buff[sp+1];
            size_t sp2 = indexof(src,' ');
            if(sp2 == -1) {
                cerr << "Error: Bad message" << endl;
                continue;
            }
            char path[sp2+1];
            strncpy(path, src, sp2);
            path[sp2] = 0;
            
            if(handle_request) {
                handle_request(connfd, path);
            }
        }
    }
}