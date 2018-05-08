#pragma once

#include "common/motor.h"
#include "xbox_linux.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

namespace Controller {
Xbox::Controller xbox;
float x = 0;
float y = 0;

void
run(bool isAxis, uint8_t number, int16_t value, void *userData, bool isError)
{
    if (isError) {
        std::cerr << "An error occurred reading from Xbox controller"
                  << std::endl;
        return;
    }

    // only interested in the joystick
    if (!isAxis) {
        return;
    }

    // only interested in left joystick
    switch (number) {
    case Xbox::LeftStickVertical:
        y = value / (float) std::numeric_limits<int16_t>::max();
        break;
    case Xbox::LeftStickHorizontal:
        x = value / (float) std::numeric_limits<int16_t>::max();
        break;
    default:
        return;
    }

    // Code below is adapted from Jamie's joystick.h - AD
    // If length of joystick vector places it in deadzone, stop motors
    const float r = sqrt((x * x) + (y * y));
    const float theta = atan2(x, -y);
    const float twoTheta = 2.0f * theta;

    // Drive motor
    Motor *motor = reinterpret_cast<Motor *>(userData);
    if (theta >= 0.0f && theta < M_PI_2) {
        motor->tank(r, r * cos(twoTheta));
    } else if (theta >= M_PI_2 && theta < M_PI) {
        motor->tank(-r * cos(twoTheta), -r);
    } else if (theta < 0.0f && theta >= -M_PI_2) {
        motor->tank(r * cos(twoTheta), r);
    } else if (theta < -M_PI_2 && theta >= -M_PI) {
        motor->tank(-r, -r * cos(twoTheta));
    }
}

void
start(Motor *motor)
{
    xbox.open();
    xbox.startThread(run, motor);
    std::cout << "Running controller service" << std::endl;
}

void
stop()
{
    xbox.close();
}
}
