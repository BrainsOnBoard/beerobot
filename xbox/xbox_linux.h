#pragma once

#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <linux/joystick.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#include "xbox_defines.h"

namespace Xbox {

/*
 * Controller buttons. The left stick and right stick are also buttons (you can
 * click them.)
 */
enum Button
{
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    LB = 4,
    RB = 5,
    Back = 6,
    Start = 7,
    XboxButton = 8,
    LeftStickButton = 9,
    RightStickButton = 10,
    Left = 11,
    Right = 12,
    Up = 13,
    Down = 14
};

class Controller
{
public:
    ~Controller()
    {
        close();
    }

    /*
     * Open connection to controller. Return true if connected successfully,
     * false otherwise.
     */
    bool open()
    {
        m_Fd = ::open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
        return m_Fd >= 0;
    }

    /*
     * Close connection to controller.
     */
    void close()
    {
        if (m_Closing) {
            return;
        }
        m_Closing = true;

        if (m_Thread) {
            m_Thread->join();
            delete m_Thread;
            delete m_JsEvent;
        }

        ::close(m_Fd);
    }

    /*
     * Start the read thread in the background. Call callback when an event
     * occurs.
     */
    void startThread(ControllerCallback callback, void *data)
    {
        if (!m_Thread) {
            m_JsEvent = new js_event;
            m_Thread = new std::thread(runThread, this, callback, data);
        }
    }

    /*
     * Read controller event into js struct. Returns true if read successfully,
     * false if an error occurs.
     */
    bool read(js_event &js)
    {
        while (!m_Closing) {
            const ssize_t bytes = ::read(m_Fd, &js, sizeof(js));
            if (bytes > 0) {
                break;
            }
            if (errno != EAGAIN) {
                return false;
            }

            usleep(sleepmillis * 1000);
        }
        if (m_Closing) {
            return false;
        }

        // we treat initial events (indicating initial state of controller) like
        // any other
        js.type &= ~JS_EVENT_INIT;

        // if it's an axis event for the left or right stick, account for
        // deadzone
        if (js.type == JS_EVENT_AXIS && js.number >= LeftStickHorizontal &&
            js.number <= RightStickVertical && abs(js.value) < deadzone) {
            js.value = 0;
        }

        return true;
    }

    /*
     * Get the name of the button corresponding to number.
     */
    static const char *getButtonName(uint8_t number)
    {
        switch (number) {
        case A:
            return "A";
        case B:
            return "B";
        case X:
            return "X";
        case Y:
            return "Y";
        case LB:
            return "LB";
        case RB:
            return "RB";
        case Back:
            return "BACK";
        case Start:
            return "START";
        case XboxButton:
            return "XBOX";
        case LeftStickButton:
            return "LSTICK";
        case RightStickButton:
            return "RSTICK";
        case Left:
            return "LEFT";
        case Right:
            return "RIGHT";
        case Up:
            return "UP";
        case Down:
            return "DOWN";
        }
        return "(unknown)";
    }

    /*
     * Get the name of the axis corresponding to number.
     */
    static const char *getAxisName(uint8_t number)
    {
        switch (number) {
        case LeftStickHorizontal:
            return "LSTICKH";
        case LeftStickVertical:
            return "LSTICKV";
        case RightStickHorizontal:
            return "RSTICKH";
        case RightStickVertical:
            return "RSTICKV";
        case LeftTrigger:
            return "LTRIGGER";
        case RightTrigger:
            return "RTRIGGER";
        case DpadHorizontal:
            return "DPADH";
        case DpadVertical:
            return "DPADV";
        }
        return "(unknown)";
    }

private:
    int m_Fd = 0;                    // file descriptor for joystick device
    std::thread *m_Thread = nullptr; // read thread object
    bool m_Closing = false;          // is controller closing?
    js_event *m_JsEvent = nullptr;   // struct to contain joystick event
    static const int16_t deadzone = 10000; // size of deadzone for axes (i.e.
                                           // region within which not activated)
    static const long sleepmillis = 25; // number of milliseconds between polls

    /*
     * This function is invoked by the read thread. It repeatedly polls the
     * controller, calling the callback function as appropriate. If an error
     * occurs, the callback is called with a nullptr in place of the js_event
     * struct.
     */
    static void runThread(Controller *c,
                          ControllerCallback callback,
                          void *userData)
    {
        while (c->read(*c->m_JsEvent)) {
            js_event &js = *c->m_JsEvent;
            callback(js.type & JS_EVENT_AXIS,
                     js.number,
                     js.value,
                     userData,
                     false);
        }
        if (!c->m_Closing) {
            callback(false, 0, 0, userData, true);
        }
    }
};
}
