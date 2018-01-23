#pragma once

#include <iostream>

#include "joystick.h"

#define DRIVE_TRACE // additionally output drive commands to console

#define JS_DEV "/dev/input/js0" // which joystick device to use
//#define JS_TRACE // displays trace of joystick input in console

#define DEADZONE 0.25

using namespace std;

bool do_run_controller = true; // flag to exit controller loop


/* Listens to controller input and sends appropriate drive command to robot */
void* run_controller(void *ptr)
{
    cout << "Running controller service" << endl;

    // motor device to send commands to
    Motor *mtr = (Motor*) ptr;

    Joystick joystick(JS_DEV);
    
    // flag is set to false when user tries to quit program
    while (do_run_controller) {
         // Read joystick
        joystick.read();
        
        // Drive motor using joystick
        joystick.drive(*mtr, DEADZONE);
    }
}
