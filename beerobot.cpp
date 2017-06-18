/*
 * File:   beerobot.cpp
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
#define USE_ROBOT

#include "beeeye.h" // for image reading/manipulation
#include "xboxrobot.h" // for using the Xbox controller to drive the robot

#define ENABLE_CONTROLLER

int main(int, char**) {

    thread tcamera(run_camera); // thread for displaying camera output on screen

#ifdef ENABLE_CONTROLLER
    thread tcontroller(run_controller); // thread for handling controller button presses
#endif

    // wait for the camera thread to finish
    tcamera.join();

#ifdef ENABLE_CONTROLLER
    // camera thread ends when user quits, so we must now stop controller thread
    do_run_controller = false;
    tcontroller.join(); // wait for thread to finish
#endif

    return 0;
}