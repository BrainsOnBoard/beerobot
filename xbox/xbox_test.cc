#include "xbox.h"
#include <string>                   //included <string> 

using namespace std;
using namespace Xbox;

Xbox::Controller cont;              //moved this up here. Functions handleButten and handleAxis use cont Controller now.

void handleButton(unsigned int number, int16_t value)           //changed all uint8_t for unsignet int
{
    string name = cont.getButtonName(number);                   //changed all char to strings
    if (value) {
        cout << "Button pushed: " << name << " (" << (int) number << ")" << endl;
    } else {
        cout << "Button released: " << name << " (" << (int) number << ")" << endl;
    }
}

void handleAxis(unsigned int number, int16_t value)             //changed all uint8_t for unsignet int
{
    string name = cont.getAxisName(number);                      //changed all char to strings
    cout << "Axis " << name << " (" << (int) number << "): " << value << endl;
}

void callback(JoystickEvent *js, void *)
{
    if (!js) {
        cerr << "Error reading from joystick" << endl;
        exit(1);
    }

    if (js->isAxis) {
        handleAxis(js->number, js->value);
    } else {
        handleButton(js->number, js->value);
    }
}

int main()                                                          //moved Controller cont; to the top
{
    cout << "Xbox controller test program" << endl;
    cout << "Press return to quit" << endl << endl;

    if (!cont.open()) {
        cerr << "Error: Could not open joystick" << endl;
        return 1;
    }

    cout << "Opened joystick" << endl;
    cont.startThread(callback, nullptr);

    cin.ignore();

    cont.close();
    cout << "Controller closed" << endl;

    return 0;
}
