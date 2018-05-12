#pragma once

#include "common/motor.h"
#include "xbox.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <memory>

class ControllerThread
{
public:
    ControllerThread(std::shared_ptr<Motor> motor)
      : m_Motor(motor)
    {
        m_Controller.open();
        m_Controller.startThread(run, this);
        std::cout << "Running controller service" << std::endl;
    }

private:
    std::shared_ptr<Motor> m_Motor;
    Xbox::Controller m_Controller;
    float m_X = 0;
    float m_Y = 0;

    static void run(Xbox::JoystickEvent *js, void *userData)
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

        // our ControllerThread object was passed to thread as user data
        auto thread = reinterpret_cast<ControllerThread *>(userData);

        // only interested in left joystick
        float x = thread->m_X;
        float y = thread->m_Y;
        switch (js->number) {
        case Xbox::LeftStickVertical:
            y = js->value / (float) std::numeric_limits<int16_t>::max();
            break;
        case Xbox::LeftStickHorizontal:
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
        if (theta >= 0.0f && theta < M_PI_2) {
            thread->m_Motor->tank(r, r * cos(twoTheta));
        } else if (theta >= M_PI_2 && theta < M_PI) {
            thread->m_Motor->tank(-r * cos(twoTheta), -r);
        } else if (theta < 0.0f && theta >= -M_PI_2) {
            thread->m_Motor->tank(r * cos(twoTheta), r);
        } else if (theta < -M_PI_2 && theta >= -M_PI) {
            thread->m_Motor->tank(-r, -r * cos(twoTheta));
        }        
    }
};
