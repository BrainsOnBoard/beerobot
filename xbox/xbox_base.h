#pragma once

#include <cstdint>

namespace Xbox {
/*
 * Controller axes. Note that the triggers are axes as is the dpad. The dpad is
 * slightly strange in that it's also treated as buttons (i.e. pressing up gives
 * both a button event and an axis event).
 */
enum Axis
{
    LeftStickHorizontal = 0,
    LeftStickVertical = 1,
    RightStickHorizontal = 3,
    RightStickVertical = 4,
    LeftTrigger = 2,
    RightTrigger = 5,
    DpadHorizontal = 6,
    DpadVertical = 7
};

struct JoystickEvent
{
    int16_t value;
    unsigned int number;
    bool isAxis;
    bool isInitial;
};

// For callbacks when a controller event occurs (button is pressed, axis moves)
using ControllerCallback = void (*)(JoystickEvent *js, void *userData);

class ControllerBase
{
private:
    bool m_Closing = false;
    JoystickEvent m_JsEvent;

public:
    virtual bool open() = 0;
    virtual bool read(Xbox::JoystickEvent &js) = 0;

    bool read()
    {
        return read(m_JsEvent);
    }

    std::string getButtonName(unsigned int number)
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

    std::string getAxisName(unsigned int number)
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

    void startThread(ControllerCallback callback, void *data)
    {
        if (!m_Thread) {
            m_Thread = new std::thread(runThread, this, callback, data);
        }
    }

    void close()
    {
        if (m_Closing) {
            return;
        }
        m_Closing = true;

        if (m_Thread) {
            m_Thread->join();
            delete m_Thread;
        }
    }

private:
    std::thread *m_Thread = nullptr;

    static void runThread(ControllerBase *c,
                          ControllerCallback callback,
                          void *userData)
    {
        while (c->read()) {
            callback(&c->m_JsEvent, userData);
        }
        if (!c->m_Closing) {
            callback(nullptr, userData);
        }
    }
};
}
