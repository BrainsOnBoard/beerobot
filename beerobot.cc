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

#include "motor.h"
#include "motor_surveyor.h"
#include "motor_i2c.h"

#include "imagefile.h"
#include "beeeyeserver.h"
#include "beeeyeconfig.h"
#include "beeeyeviewer.h"
#include "xboxrobot.h" // for using the Xbox controller to drive the robot

#define ENABLE_CONTROLLER

using namespace std;

void showusage()
{
    cout << "Usage: beerobot [--config|--controller|--no-controller] [usb|wifi|viewer [ip]]" << endl;
    exit(1);
}

pthread_t* startcontroller(Motor* mtr)
{
    pthread_t* cthread = new pthread_t;
    pthread_create(cthread, NULL, &run_controller, mtr);
    return cthread;
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
                vid = get_pixpro_usb();
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
            controller = controllerflag && controller;
            HttpClient client(viewer_ip, 1234, controller);
            if (controllerflag && controller) {
                startcontroller(&client);
            }
            run_eye_viewer(client);
            return 0;
        } else if (vid) {
            run_eye_config(vid, config);
            return 0;
        }
    }

#ifdef ENABLE_CONTROLLER
    controller = controllerflag && controller;
#endif

    Motor* mtr = NULL;
#if defined(USE_SURVEYOR)
    mtr = new MotorSurveyor("192.168.1.1", 2000);
    if (controller)
        startcontroller(mtr);
#elif defined(USE_ARDUINO)
    mtr = new MotorI2C();
    if (controller)
        startcontroller(mtr);
#endif

    if (!controller)
        cout << "Use of controller is disabled" << endl;

    // start thread for displaying camera output on screen
    pthread_t tserver;
    pthread_create(&tserver, NULL, BeeEyeServer::run_server, mtr);

    // wait for the server thread to finish
    pthread_join(tserver, NULL);

    do_run_controller = false;

    return 0;
}
