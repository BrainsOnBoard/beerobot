#ifndef BEEEYESERVER_H
#define BEEEYESERVER_H

#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#define VID_WIDTH 1440
#define VID_HEIGHT 1440
#include "ini.h"

#include "httpserver.h"

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
    CamParams params;
    VideoCapture cap;
    Mat map_x, map_y;
    
    static bool run_request;
    static bool handle_request_server(int connfd, char* path);
    static void kill_request_server();
};

// get the number for a camera with a given name (-1 if not found)
int get_camera_by_name(const char* name);
#endif