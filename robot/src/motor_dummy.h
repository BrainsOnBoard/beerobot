#pragma once

#include <iostream>

// GeNN robotics includes
#include "common/motor.h"

/* different types of motor output */
enum MotorType {
    Dummy,
    Surveyor,
    Arduino
};

/* this is the superclass for other Motor devices */
class MotorDummy : public Motor {
public:

    virtual void tank(float left, float right)
    {
        std::cout << "Dummy motor: left: " << left << "; right: " << right << std::endl;
    }
};