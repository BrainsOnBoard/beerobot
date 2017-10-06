#ifndef BEEEYESERVER_H
#define BEEEYESERVER_H

#include <iostream>
#include <fstream>

#include "httpserver.h"
#include "beeeye.h"
#include "motor.h"

class BeeEyeServer : public HttpServer {
public:
    static BeeEyeServer* Instance;
    static void* run_server(void* mtr);
    static void stop_server();

    BeeEyeServer(Motor* mtr);
    BeeEyeServer(const BeeEyeServer& orig);
    //virtual ~BeeEyeServer();
    void run();
    bool handle_request(int connfd, char* path);
private:
    BeeEye eye;
    Motor* mtr;

    static bool run_request;
    static bool handle_request_server(int connfd, char* path);
    static void kill_request_server();
};

bool getfloat(const string str, float &f);
#endif