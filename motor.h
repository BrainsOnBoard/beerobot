#pragma once
#include <iostream>

enum MotorType {
    Dummy,
    Surveyor,
    Arduino
};

class Motor {
public:

    virtual void tank(float left, float right)
    {
        std::cout << "Dummy motor: left: " << left << "; right: " << right << std::endl;
    }
};