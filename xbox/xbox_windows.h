#pragma once

#define WIN32_LEAN_AND_MEAN
#include "xbox_defines.h"
#include <windows.h>

#include <XInput.h>
#pragma comment(lib, "XInput.lib")

#include "iostream"
#include <cstdint>
#include <fcntl.h>
#include <sys/stat.h>
#include <thread>

namespace Xbox {
class Controller
{
private:
    XINPUT_STATE _controllerState;
    int _controllerNum = 0;
    unsigned int pressed = 0;
    int lThumbXState1 = 0;
    int lThumbYState1 = 0;
    int rThumbXState1 = 0;
    int rThumbYState1 = 0;

public:
    XINPUT_STATE Read();
    bool open();
    void close();
    bool Change();
    bool read(Xbox::JoystickEvent &js);
    std::string getButtonName(unsigned int number);
    std::string getAxisName(unsigned int number);
    void startThread(ControllerCallback callback, void *data);
    bool read();

    std::thread *m_Thread = nullptr;
    bool m_Closing = false;
    JoystickEvent m_JsEvent;
};

enum Button
{
    A = XINPUT_GAMEPAD_A,
    B = XINPUT_GAMEPAD_B,
    X = XINPUT_GAMEPAD_X,
    Y = XINPUT_GAMEPAD_Y,
    LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
    RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
    Back = XINPUT_GAMEPAD_BACK,
    Start = XINPUT_GAMEPAD_START,
    LeftStickButton = XINPUT_GAMEPAD_LEFT_THUMB,
    RightStickButton = XINPUT_GAMEPAD_RIGHT_THUMB,
    Left = XINPUT_GAMEPAD_DPAD_LEFT,
    Right = XINPUT_GAMEPAD_DPAD_RIGHT,
    Up = XINPUT_GAMEPAD_DPAD_UP,
    Down = XINPUT_GAMEPAD_DPAD_DOWN
};

XINPUT_STATE
Controller::Read()
{
    // Zeroise the state
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

    // Get the state
    XInputGetState(_controllerNum, &_controllerState);

    return _controllerState;
}

bool
Controller::open()
{
    // Zeroise the state
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

    // Get the state
    DWORD Result = XInputGetState(_controllerNum, &_controllerState);

    if (Result == ERROR_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

bool
Controller::Change()
{
    // Zeroise the state
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

    // Check for any changes in the controller
    int state1 = Read().dwPacketNumber;
    while (true) {
        int state2 = Read().dwPacketNumber;
        if (state1 != state2) {
            state1 = state2;
            return (true);
        }
    }
}

// read the buttons on the controller and report which button(s) are
// pressed/unpressed
bool
Controller::read(Xbox::JoystickEvent &js)
{
    while (Change()) {
        unsigned int buttState = Read().Gamepad.wButtons;
        if (~pressed & buttState) {
            js.number = ~pressed & buttState;
            js.value = true;
            js.isAxis = false;
            pressed = buttState;
            break;
        } else {
            if (pressed & ~buttState) {
                js.number = pressed & ~buttState;
                js.value = false;
                js.isAxis = false;
                pressed = buttState;
                break;
            } else {
                int lTrigState = Read().Gamepad.bLeftTrigger;
                int rTrigState = Read().Gamepad.bRightTrigger;
                int lThumbYState2 = Read().Gamepad.sThumbLX;
                int lThumbXState2 = Read().Gamepad.sThumbLY;
                int rThumbXState2 = Read().Gamepad.sThumbRX;
                int rThumbYState2 = Read().Gamepad.sThumbRY;
                if (lThumbXState2 != lThumbXState1) {
                    js.number = LeftStickHorizontal;
                    js.value = lThumbXState2;
                    js.isAxis = true;
                    lThumbXState1 = lThumbXState2;
                    break;
                }
                if (lThumbYState2 != lThumbYState1) {
                    js.number = LeftStickVertical;
                    js.value = lThumbYState2;
                    js.isAxis = true;
                    lThumbYState1 = lThumbYState2;
                    break;
                }
                if (rThumbXState2 != rThumbXState1) {
                    js.number = RightStickHorizontal;
                    js.value = rThumbXState2;
                    js.isAxis = true;
                    rThumbXState1 = rThumbXState2;
                    break;
                }
                if (rThumbYState2 != rThumbYState1) {
                    js.number = RightStickVertical;
                    js.value = rThumbYState2;
                    js.isAxis = true;
                    rThumbYState1 = rThumbYState2;
                    break;
                }
                if (lTrigState >= 1) {
                    js.number = LeftTrigger;
                    js.value = lTrigState;
                    js.isAxis = true;
                    break;
                }
                if (rTrigState >= 1) {
                    js.number = RightTrigger;
                    js.value = rTrigState;
                    js.isAxis = true;
                    break;
                } else {
                    if (js.number == LeftTrigger && lTrigState < 1) {
                        js.value = 0;
                        break;
                    }
                    if (js.number == RightTrigger && rTrigState < 1) {
                        js.value = 0;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

bool
Controller::read()
{
    return read(m_JsEvent);
}

void
Controller::close()
{
    if (m_Closing) {
        return;
    }
    m_Closing = true;

    if (m_Thread) {
        m_Thread->join();
        delete m_Thread;
    }
    return;
}

std::string
Controller::getButtonName(unsigned int number) // Get the name of the button
                                               // corresponding to number.
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

std::string
Controller::getAxisName(unsigned int number)
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

static void
runThread(Controller *c, ControllerCallback callback, void *userData)
{
    while (c->read()) {
        callback(&c->m_JsEvent, userData);
    }
    if (!c->m_Closing) {
        callback(nullptr, userData);
    }
}

void
Controller::startThread(ControllerCallback callback, void *data)
{
    if (!m_Thread) {
        m_Thread = new std::thread(runThread, this, callback, data);
    }
}
}