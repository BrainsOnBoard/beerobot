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

// For callbacks when a controller event occurs (button is pressed, axis moves)
using ControllerCallback = void (*)(bool isAxis,
                                    uint8_t number,
                                    int16_t value,
                                    void *userData,
                                    bool isError);
}
