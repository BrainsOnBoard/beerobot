#include "xbox_linux.h"

using namespace std;
using namespace Xbox;

void
handleButton(uint8_t number, int16_t value)
{
    const char *name = Controller::getButtonName(number);
    if (value) {
        cout << "Button pushed: " << name << " (" << (int) number << ")"
             << endl;
    } else {
        cout << "Button released: " << name << " (" << (int) number << ")"
             << endl;
    }
}

void
handleAxis(uint8_t number, int16_t value)
{
    const char *name = Controller::getAxisName(number);
    cout << "Axis " << name << " (" << (int) number << "): " << value << endl;
}

void
callback(JoystickEvent *js, void *)
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

int
main()
{
    cout << "Xbox controller test program" << endl;
    cout << "Press return to quit" << endl << endl;

    Controller cont;
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
