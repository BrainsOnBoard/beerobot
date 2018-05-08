#include "CXBOXController.h"
#include <iostream>
#include <string>

using namespace std;

Controller* Player1;
int main(int argc, char* argv[])
{

	Player1 = new Controller(1);

	while(true)
	{
		if(Player1->open())
		{
			if(Player1->Read().Gamepad.wButtons & A){
				std::cout << "You pressed A" << std::endl;
				int state = Player1->Read().dwPacketNumber;
				std::cout << "It is " << state << std::endl;
			}

			if(Player1->Read().Gamepad.wButtons & B){
				std::cout << "You pressed B" << std::endl;
			}

			if(Player1->Read().Gamepad.wButtons & X){
				std::cout << "You pressed X" << std::endl;
			}

			if(Player1->Read().Gamepad.wButtons & Y){
				std::cout << "You pressed Y" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & LB){
				std::cout << "You pressed LB" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & RB){
				std::cout << "You pressed RB" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & Back){
				std::cout << "You pressed Back" << std::endl;
				break;
			}

			if (Player1->Read().Gamepad.wButtons & Start){
				std::cout << "You pressed Start" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & LeftStickButton)	{
				std::cout << "You pressed the left Yoystick down" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & RightStickButton){
				std::cout << "You pressed the right Yoystick down" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & Left){
				std::cout << "You pressed the LEFT arrow" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & Right){
				std::cout << "You pressed the RIGHT arrow" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & Up){
				std::cout << "You pressed the UP arrow" << std::endl;
			}

			if (Player1->Read().Gamepad.wButtons & Down){
				std::cout << "You pressed the DOWN arrow" << std::endl;
			}

			
			if (Player1->Read().Gamepad.bLeftTrigger){
				float leftTrigger = Player1->Read().Gamepad.bLeftTrigger;
				std::cout << "You pressed teh LEft Trigger by " << leftTrigger << std::endl;
			}

			if (Player1->Read().Gamepad.bRightTrigger){
				float rightTrigger = Player1->Read().Gamepad.bRightTrigger;
				std::cout << "You pressed teh Right Trigger by " << rightTrigger << std::endl;
			}

			//  axis number between -32768 and 32767.
			float thumbLX = Player1->Read().Gamepad.sThumbLX;
			float thumbLY = Player1->Read().Gamepad.sThumbLY;
			float restLY = 128;
			float restLX = 128;
			if (thumbLX != restLX || thumbLY != restLY) {
				std::cout << "You are moving " << thumbLX << " " << "horizontaly and " << thumbLY << " " << "verticaly" << std::endl;
			}

			float thumbRX = Player1->Read().Gamepad.sThumbRX;
			float thumbRY = Player1->Read().Gamepad.sThumbRY;
			float restRY = 128;
			float restRX = 128;
			if (thumbRX != restRX || thumbRY != restRY) {
				std::cout << "You are moving " << thumbRX << " " << "horizontaly and " << thumbRY << " " << "verticaly" << std::endl;
			}
		}
		else
		{
			std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
			std::cout << "Press Any Key To Exit.";
			std::cin.get();
			break;
		}
	}

	delete(Player1);

	return( 0 );
}