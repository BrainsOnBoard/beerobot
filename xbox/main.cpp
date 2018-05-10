#include "CXBOXController.h"
#include <iostream>
#include <string>

using namespace std;

Controller* Player1;
int main(int argc, char* argv[])
{
    Player1 = new Controller(1);
    int state1 = Player1->Read().dwPacketNumber;
    Xbox::JoystickEvent event;
    while (true)
    {
        if (Player1->open())
        {
            while (Player1->Change())
            {
                Player1->read(event);
                std::cout << "Button " << (int)event.number << "was " << event.value << std::endl;
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