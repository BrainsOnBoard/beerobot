#include "CXBOXController.h"
#include "iostream"

Controller::Controller(int playerNumber)
{
	// Set the Controller Number
	_controllerNum = playerNumber - 1;
}

XINPUT_STATE Controller::Read()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	XInputGetState(_controllerNum, &_controllerState);

	return _controllerState;
}

bool Controller::open()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	DWORD Result = XInputGetState(_controllerNum, &_controllerState);

	if(Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Controller::Change()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	//Check for any changes in the controller
	int state1 = Read().dwPacketNumber;
	while (true) {
		int state2 = Read().dwPacketNumber;
		if (state1 != state2) {
			state1 = state2;
			return(true);
		}
	}
}

void Controller::close()
{
	std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
	std::cout << "Press Any Key To Exit.";
	std::cin.get();
	return;
}


