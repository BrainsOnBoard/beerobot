/*
 * File:   beerobot.cpp
 * Author: alex
 *
 * Created on 12 May 2017, 13:38
 */

#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>

#define JS_DEV "/dev/input/js0"
//#define JS_TRACE
#define JS_BTN_A  0
#define JS_BTN_B  1
#define JS_PAD_LR 6

using namespace std;

void listen_joystick() {
    int fd = open(JS_DEV, O_RDONLY);
    if (fd < 0) {
        cerr << "Error: Could not find joystick (" << fd << ")" << endl;
        exit(1);
    }

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
                            cout << "A down" << endl;
                        else
                            cout << "A up" << endl;
                        break;
                    case JS_BTN_B:
                        if (e.value)
                            cout << "B down" << endl;
                        else
                            cout << "B up" << endl;
                        break;
                }
                break;
            case JS_EVENT_AXIS:
                if (e.number == JS_PAD_LR) {
                    if (e.value < 0)
                        cout << "pressed left" << endl;
                    else if (e.value > 0)
                        cout << "pressed right" << endl;
                    else
                        cout << "l/r pad up" << endl;
                }
                break;
        }
    }

    //close(fd);
}

int main(int argc, char** argv) {

    listen_joystick();

    return 0;
}

