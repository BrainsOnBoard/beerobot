#ifndef BEEEYESERVER_H
#define BEEEYESERVER_H

#include <iostream>
#include <fstream>

#include "httpserver.h"
#include "beeeye.h"

class BeeEyeServer : public HttpServer {
public:
    static BeeEyeServer Instance;
    static void run_server();
    static void stop_server();

    BeeEyeServer();
    BeeEyeServer(const BeeEyeServer& orig);
    //virtual ~BeeEyeServer();
    void run();
    bool handle_request(int connfd, char* path);
private:
    BeeEye eye;
    
    static bool run_request;
    static bool handle_request_server(int connfd, char* path);
    static void kill_request_server();
};
#endif