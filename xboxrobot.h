#pragma once

#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>

#define DRIVE_TRACE // additionally output drive commands to console

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

/* the current driving state of the robot */
enum DriveState {
    dstop = 0,
    dforward,
    dbackward,
    dleft,
    dright
};

// current and previous driving state
DriveState drivecur = dstop;
DriveState drivelast = dstop;

/* revert to previous driving state (on button released) */
void pop_drivestate(DriveState olddrive)
{
    if (drivecur == olddrive || (olddrive == dleft && drivecur == dright)) {
        drivecur = drivelast;
    }
    drivelast = dstop;
}

/* store current driving state and change to new one (on button pushed) */
void push_drivestate(DriveState newdrive)
{
    drivelast = drivecur;
    drivecur = newdrive;
}

/* Listens to controller input and sends appropriate drive command to robot */
void* run_controller(void *ptr)
{
    cout << "Running controller service" << endl;

    // motor device to send commands to
    Motor *mtr = (Motor*) ptr;

    // open joystick device
    int fd = open(JS_DEV, O_RDONLY);
    if (fd < 0) {
        cout << "Could not find joystick (" << fd << ")" << endl;
        return NULL;
    }

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
                    push_drivestate(dforward);
                else
                    pop_drivestate(dforward);
                break;
            case JS_BTN_B: // pressed B
                if (e.value)
                    push_drivestate(dbackward);
                else
                    pop_drivestate(dbackward);
                break;
            }
            break;
        case JS_EVENT_AXIS:
            if (e.number == JS_PAD_LR) {
                if (e.value < 0) // pressed left
                    push_drivestate(dleft);
                else if (e.value > 0) // pressed right
                    push_drivestate(dright);
                else
                    // actually handles left and right scenarios
                    pop_drivestate(dleft);
            }
            break;
        default:
            continue;
        }

        // send appropriate driving command
        switch (drivecur) {
        case dforward:
            mtr->tank(SPEED, SPEED);
            break;
        case dbackward:
            mtr->tank(-SPEED, -SPEED);
            break;
        case dleft:
            mtr->tank(-TURNSPEED, TURNSPEED);
            break;
        case dright:
            mtr->tank(TURNSPEED, -TURNSPEED);
            break;
        default:
            mtr->tank(0, 0);
        }
    }

    // close joystick file descriptor
    close(fd);
}
