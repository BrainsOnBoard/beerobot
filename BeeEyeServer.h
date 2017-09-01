#ifndef BEEEYESERVER_H
#define BEEEYESERVER_H

#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#define VID_WIDTH 1440
#define VID_HEIGHT 1440
#include "ini.h"

#include "HttpServer.h"

class BeeEyeServer : public HttpServer
{
public:
    static BeeEyeServer Instance;
    
    BeeEyeServer();
    BeeEyeServer(const BeeEyeServer& orig);
    //virtual ~BeeEyeServer();
    void run();
    void handle_request(int connfd, char* path);
    static void run_server();
    static void handle_request_server(int connfd, char* path);
private:
    CamParams params;
    VideoCapture cap;
    Mat map_x, map_y;
};

using namespace std;

// get the number for a camera with a given name (-1 if not found)
int get_camera_by_name(const char* name);
#endif