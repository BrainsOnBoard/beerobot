#include "CXBOXController.h"
#include <iostream>
#include <string>

using namespace std;

Controller* Player1;
int main(int argc, char* argv[])
{
    Player1 = new Controller(1);
    int state1 = Player1->Read().dwPacketNumber;
    while (true)
    {
        if (Player1->open())
        {
            while (Player1->Change()) {
                if (Player1->Read().Gamepad.wButtons & A) {
                    std::cout << "You pressed A" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & B) {
                    std::cout << "You pressed B" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & X) {
                    std::cout << "You pressed X" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & Y) {
                    std::cout << "You pressed Y" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & LB) {
                    std::cout << "You pressed LB" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & RB) {
                    std::cout << "You pressed RB" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & Back) {
                    std::cout << "You pressed Back" << std::endl;
                    break;
                }

                if (Player1->Read().Gamepad.wButtons & Start) {
                    std::cout << "You pressed Start" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & LeftStickButton) {
                    std::cout << "You pressed the left Yoystick down" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & RightStickButton) {
                    std::cout << "You pressed the right Yoystick down" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & Left) {
                    std::cout << "You pressed the LEFT arrow" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & Right) {
                    std::cout << "You pressed the RIGHT arrow" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & Up) {
                    std::cout << "You pressed the UP arrow" << std::endl;
                }

                if (Player1->Read().Gamepad.wButtons & Down) {
                    std::cout << "You pressed the DOWN arrow" << std::endl;
                }

                if (Player1->Read().Gamepad.bLeftTrigger) {
                    float leftTrigger = Player1->Read().Gamepad.bLeftTrigger;
                    std::cout << "You pressed the LEft Trigger by " << leftTrigger << std::endl;
                }

                if (Player1->Read().Gamepad.bRightTrigger) {
                    float rightTrigger = Player1->Read().Gamepad.bRightTrigger;
                    std::cout << "You pressed the Right Trigger by " << rightTrigger << std::endl;
                }


                if (Player1->Read().Gamepad.sThumbLX || Player1->Read().Gamepad.sThumbLY) {
                    float thumbLX = Player1->Read().Gamepad.sThumbLX; 				//  axis is a number between -32768 and 32767.
                    float thumbLY = Player1->Read().Gamepad.sThumbLY;
                    std::cout << "You are moving with the left joystick " << thumbLX << " " << "horizontaly and " << thumbLY << " " << "verticaly" << std::endl;
                }

                if (Player1->Read().Gamepad.sThumbRX || Player1->Read().Gamepad.sThumbRY) {
                    float thumbRX = Player1->Read().Gamepad.sThumbRX; 				//  axis is a number between -32768 and 32767.
                    float thumbRY = Player1->Read().Gamepad.sThumbRY;
                    std::cout << "You are moving with the right joystick " << thumbRX << " " << "horizontaly and " << thumbRY << " " << "verticaly" << std::endl;
                }
            }
        }
        else
        {
            Player1->close();
            break;
        }

    }

    delete(Player1);

    return(0);
}