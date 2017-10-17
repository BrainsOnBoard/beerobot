/*
 * File:   beerobot.cc
 * Author: alex
 *
 * Created on 12 May 2017, 13:38
 */

#include <cstdlib>
#include <thread> // std::thread

/* If enabled, the program uses the PixPro as a camera and sends drive commands
 * to the robot when controller buttons are pushed. Otherwise, the computer's
 * webcam is used as a camera and the program does not attempt to connect to the
 * robot but shows the commands it would have sent.
 */
//#define USE_SURVEYOR
#define USE_ARDUINO

// for motor control of robot
#include "motor.h"
#include "motor_surveyor.h"
#include "motor_i2c.h"

#include "mainserver.h"
#include "mainclient.h"

#include "imagereceiver.h"
#include "beeeyeviewer.h"

#include "imagefile.h"
#include "beeeyeconfig.h"
#include "xboxrobot.h" // for using the Xbox controller to drive the robot

#define ENABLE_CONTROLLER

using namespace std;

void showusage()
{
    cout << "Usage: beerobot [--config|--controller|--no-controller] [usb|wifi|viewer [ip]]" << endl;
    exit(1);
}

void startcontroller(Motor* mtr)
{
    pthread_t cthread;
    pthread_create(&cthread, NULL, &run_controller, mtr);
}

int main(int argc, char** argv)
{
    bool controllerflag = false;
    bool controller = false;
    char* viewer_ip = NULL;
    if (argc > 1) {
        vid_t* vid = NULL;
        bool config = false;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--config") == 0) {
                if (config) {
                    showusage();
                    return 1;
                }

                config = true;
            } else if (vid) {
                showusage();
                return 1;
            } else if (strcmp(argv[i], "usb") == 0) {
                vid = get_usb();
            } else if (strcmp(argv[i], "wifi") == 0) {
                vid = get_pixpro_wifi();
            } else if (strcmp(argv[i], "viewer") == 0) {
                if (argc < i + 2) {
                    showusage();
                    return 1;
                } else {
                    viewer_ip = argv[++i];
                }
            } else if (strcmp(argv[i], "--controller") == 0) {
                if (controllerflag) {
                    showusage();
                    return 1;
                } else {
                    controllerflag = true;
                    controller = true;
                }
            } else if (strcmp(argv[i], "--no-controller") == 0) {
                if (controllerflag) {
                    showusage();
                    return 1;
                } else {
                    controllerflag = true;
                    controller = false;
                }
            } else if (config || !process_file(argv[i])) {
                showusage();
                return 1;
            }
        }

        if (viewer_ip) {
            // code run by client (connecting to robot)
            controller = controllerflag && controller;
            MainClient client(viewer_ip, 2000);
            if (controllerflag && controller)
                startcontroller(&client);

            ImageReceiver recv(5555);
            run_eye_viewer(recv);
            return 0;
        } else if (vid) {
            // code run if just showing video locally
            run_eye_config(vid, config);
            delete vid;
            return 0;
        }
    }
#ifdef ENABLE_CONTROLLER
    controller = controllerflag && controller;
#endif

#if defined(USE_SURVEYOR)
    Motor* mtr = new MotorSurveyor("192.168.1.1", 2000);
#elif defined(USE_ARDUINO)
    Motor* mtr = new MotorI2C();
#else
    Motor* mtr = new Motor();
#endif

    if (controller)
        startcontroller(mtr);
    else
        cout << "Use of controller is disabled" << endl;

    MainServer::run_server(mtr);

    do_run_controller = false;

    return 0;
}
