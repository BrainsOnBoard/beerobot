#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>
#include "xbox_defines.h"

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput.lib")

class Controller
{
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
    unsigned int pressed = 0;
    int lThumbXState1 = 0;
    int lThumbYState1 = 0;
    int rThumbXState1 = 0;
    int rThumbYState1 = 0;
public:
	Controller(int playerNumber);
	XINPUT_STATE Read();
	bool open();
	void close();
	bool Change();
    void read(Xbox::JoystickEvent &js);
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
#endif

