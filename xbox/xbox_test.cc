#include "xbox.h"
#include <string>

using namespace std;
using namespace Xbox;

Xbox::Controller cont;

void handleButton(unsigned int number, int16_t value)
{
    string name = cont.getButtonName(number);
    if (value) {
        cout << "Button pushed: " << name << " (" << (int) number << ")" << endl;
    } else {
        cout << "Button released: " << name << " (" << (int) number << ")" << endl;
    }
}

void handleAxis(unsigned int number, int16_t value)
{
    string name = cont.getAxisName(number);
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

int main()
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
