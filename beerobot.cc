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

#include "beeeyeserver.h"
#include "beeeyeconfig.h"
#include "beeeyeviewer.h"
#include "xboxrobot.h" // for using the Xbox controller to drive the robot

#define ENABLE_CONTROLLER

int main(int argc, char** argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "--config") == 0) {
            cout << "Running config program" << endl;
            run_eye_config(true);
            return 0;
        } else if (strcmp(argv[1], "--viewer") == 0) {
            cout << "Running viewer program" << endl;
            run_eye_viewer(argv[2], 1234);
        } else if (strcmp(argv[1], "usb") == 0) {
            run_eye_config(false);
            return 0;
        }
    }

    thread tserver(BeeEyeServer::run_server); // thread for displaying camera output on screen

#ifdef ENABLE_CONTROLLER
    thread tcontroller(run_controller); // thread for handling controller button presses
#endif

    // wait for the server thread to finish
    tserver.join();

#ifdef ENABLE_CONTROLLER
    // camera thread ends when user quits, so we must now stop controller thread
    do_run_controller = false;
    tcontroller.join(); // wait for thread to finish
#endif

    return 0;
}