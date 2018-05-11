#include "xbox_windows.h"
#include <iostream>
#include <string>

using namespace std;
using namespace Xbox;

Xbox::Controller* Player1;




int main(int argc, char* argv[])
{
    Player1 = new Xbox::Controller(1);
    int state1 = Player1->Read().dwPacketNumber;
    Xbox::JoystickEvent event;
    while (true)
    {
        if (Player1->open())
        {
            Player1->read(event);
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