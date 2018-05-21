#pragma once

// C includes
#define _USE_MATH_DEFINES
#include <cmath>

// C++ includes
#include <iostream>
#include <limits>
#include <memory>

// GeNN robotics includes
#include "joystick/joystick.h"
#include "robots/motor.h"

using namespace GeNNRobotics;

class JoystickThread
{
public:
    JoystickThread(std::shared_ptr<Robots::Motor> motor)
      : m_Motor(motor)
    {
        m_Joystick.open();
        m_Joystick.startThread(run, this);
        std::cout << "Running controller service" << std::endl;
    }

private:
    std::shared_ptr<Robots::Motor> m_Motor;
    Joystick::Joystick m_Joystick;
    float m_X = 0;
    float m_Y = 0;

    static void run(Joystick::Event *js, void *userData)
    {
        if (!js) {
            std::cerr << "An error occurred reading from Xbox controller"
                      << std::endl;
            return;
        }

        // only interested in the joystick
        if (!js->isAxis) {
            return;
        }

        // our JoystickThread object was passed to thread as user data
        auto thread = reinterpret_cast<JoystickThread *>(userData);

        // only interested in left joystick
        float x = thread->m_X;
        float y = thread->m_Y;
        switch (js->number) {
        case Joystick::LeftStickVertical:
            y = js->value / (float) std::numeric_limits<int16_t>::max();
            break;
        case Joystick::LeftStickHorizontal:
            x = js->value / (float) std::numeric_limits<int16_t>::max();
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
        const float pi = 3.141592653589793238462643383279502884f;
        if (theta >= 0.0f && theta < pi / 2) {
            thread->m_Motor->tank(r, r * cos(twoTheta));
        } else if (theta >= pi / 2 && theta < pi) {
            thread->m_Motor->tank(-r * cos(twoTheta), -r);
        } else if (theta < 0.0f && theta >= -pi / 2) {
            thread->m_Motor->tank(r * cos(twoTheta), r);
        } else if (theta < -pi / 2 && theta >= -pi) {
            thread->m_Motor->tank(-r, -r * cos(twoTheta));
        }
    }
};
