#pragma once

#include <iostream>

/* different types of motor output */
enum MotorType {
    Dummy,
    Surveyor,
    Arduino
};

/* this is the superclass for other Motor devices */
class Motor {
public:

    virtual void tank(float left, float right)
    {
        std::cout << "Dummy motor: left: " << left << "; right: " << right << std::endl;
    }
};