/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpServer.h
 * Author: ad374
 *
 * Created on 01 September 2017, 11:33
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

// for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

class HttpServer {
public:
    HttpServer(int port);
    void serve(void (*)(int, char*));
    virtual ~HttpServer();
private:
    int listenfd;
};

#endif /* HTTPSERVER_H */