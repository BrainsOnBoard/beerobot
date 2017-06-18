/*
 * File:   xboxrobot.h
 * Author: alex
 *
 * Created on 12 May 2017, 16:27
 */

#ifndef XBOXROBOT_H
#define XBOXROBOT_H

#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>

// if we're not using the robot then don't actually connect
#ifndef USE_ROBOT
#define DUMMY_DRIVE
#endif

#define DRIVE_TRACE // additionally output drive commands to console

#include "motor.h" // for connecting to and sending driving commands to robot

#define JS_DEV "/dev/input/js0" // which joystick device to use
//#define JS_TRACE // displays trace of joystick input in console

// joystick button IDs
#define JS_BTN_A  0
#define JS_BTN_B  1
#define JS_PAD_LR 6

#define SPEED     0.7 // forward/backward driving speed of robot
#define TURNSPEED 1.0 // turning speed of robot

using namespace std;

bool do_run_controller = true; // flag to exit controller loop

/*
 * Listens to controller input and sends appropriate drive command to robot.
 */
void run_controller() {
    
    // open joystick device
    int fd = open(JS_DEV, O_RDONLY);
    if (fd < 0) {
        cout << "Could not find joystick (" << fd << ")" << endl;
        return;
    }

    // connect to robot
    Motor mtr("192.168.1.1", 2000);

    js_event e; // struct for storing joystick events
    
    // flag is set to false when user tries to quit program
    while (do_run_controller) {

        // read from joystick device (blocking)
        if (read(fd, &e, sizeof (e)) != sizeof (e)) {
            cerr << "Error: Could not read from joystick" << endl;
            exit(1);
        }

#ifdef JS_TRACE
        cout << "number: " << (int) e.number << endl;
        cout << "type: " << (int) e.type << endl;
        cout << "value: " << e.value << endl;
        cout << "time: " << e.time << endl << endl;
#endif

        // we only care about A, B and left/right button pushes
        switch (e.type) {
            case JS_EVENT_BUTTON:
                switch (e.number) {
                    case JS_BTN_A: // pressed A
                        if (e.value)
                            mtr.tank(SPEED, SPEED);
                        else
                            mtr.tank(0, 0);
                        break;
                    case JS_BTN_B: // pressed B
                        if (e.value)
                            mtr.tank(-SPEED, -SPEED);
                        else
                            mtr.tank(0, 0);
                        break;
                }
                break;
            case JS_EVENT_AXIS:
                if (e.number == JS_PAD_LR) {
                    if (e.value < 0) // pressed left
                        mtr.tank(-TURNSPEED, TURNSPEED);
                    else if (e.value > 0) // pressed right
                        mtr.tank(TURNSPEED, -TURNSPEED);
                    else
                        mtr.tank(0, 0);
                }
                break;
        }
    }

    // close joystick file descriptor
    close(fd);
}

#endif /* XBOXROBOT_H */