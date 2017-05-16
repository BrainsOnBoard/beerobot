/*
 * File:   beerobot.cpp
 * Author: alex
 *
 * Created on 12 May 2017, 13:38
 */

#include <cstdlib>
#include <thread> // std::thread

#include "beeeye.h"
#include "xboxrobot.h"

int main(int, char**) {
    
    thread tcamera(run_camera); // thread for displaying camera output on screen
    thread tcontroller(run_controller); // thread for handling controller button presses
    
    // wait for the camera thread to finish
    tcamera.join();
    
    // camera thread ends when user quits, so we must now stop controller thread
    do_run_controller = false;
    tcontroller.join(); // wait for thread to finish

    return 0;
}

