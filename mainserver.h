
/*
 * File:   mainserver.h
 * Author: ad374
 *
 * Created on 13 October 2017, 17:26
 */

#pragma once

class MainServer {
public:
    MainServer(int port);
    virtual ~MainServer();
    void run();
private:
    int listenfd;
};
