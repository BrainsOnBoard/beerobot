/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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
#include "motor.h"

#define JS_DEV "/dev/input/js0"
//#define JS_TRACE
#define JS_BTN_A  0
#define JS_BTN_B  1
#define JS_PAD_LR 6

#define SPEED     0.7
#define TURNSPEED 1.0

using namespace std;

void listen_controller() {
    int fd = open(JS_DEV, O_RDONLY);
    if (fd < 0) {
        cerr << "Error: Could not find joystick (" << fd << ")" << endl;
        exit(1);
    }

    Motor mtr("192.168.1.1", 2000);

    js_event e;
    while (1) {

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

        switch (e.type) {
            case JS_EVENT_BUTTON:
                switch (e.number) {
                    case JS_BTN_A:
                        if (e.value)
                            mtr.tank(SPEED, SPEED);
                        else
                            mtr.tank(0, 0);
                        break;
                    case JS_BTN_B:
                        if (e.value)
                            mtr.tank(-SPEED, -SPEED);
                        else
                            mtr.tank(0, 0);
                        break;
                }
                break;
            case JS_EVENT_AXIS:
                if (e.number == JS_PAD_LR) {
                    if (e.value < 0)
                        mtr.tank(-TURNSPEED, TURNSPEED);
                    else if (e.value > 0)
                        mtr.tank(TURNSPEED, -TURNSPEED);
                    else
                        mtr.tank(0, 0);
                }
                break;
        }
    }

    //close(fd);
}

#endif /* XBOXROBOT_H */

