struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-
#include "os/windows_include.h"

// C includes
#include <cstdlib>

// C++ includes
#include <thread>

// BoB robotics robot includes
#include "robots/tank.h"
#ifndef _WIN32
#ifndef NO_I2C_ROBOT
#include "robots/norbot.h"
#endif
#include "robots/surveyor.h"
#endif
#include "robots/tank_netsink.h"

// BoB robotics video includes
#include "video/netsink.h"
#include "video/netsource.h"

// for rendering bee's eye view on screen
#include "eye/beeeye.h"
#include "overlaydisplay.h"

// to exchange messages between robot and viewer
#include "net/client.h"
#include "net/server.h"

using namespace BoBRobotics;

/* different types of motor output */
enum RobotType
{
    Dummy,
    Surveyor,
    Arduino
};

/* show help information */
void
showusage()
{
    std::cout << "Usage: beerobot "
                 "[--controller|--no-controller|--local|--no-overlay] "
                 "[--motor dummy|surveyor|arduino] [viewer [ip]]"
              << std::endl;
    exit(1);
}

/* main entry point */
int
main(int argc, char **argv)
{
    bool controllerFlag = false; // controller CL arg present
    bool motorFlag = false;      // motor CL arg present
    bool localFlag = false;
    bool overlayFlag = true;

    bool controller = false;       // controller enabled
    RobotType robotType = Arduino; // type of Motor to use (server only)
    char *serverIP = nullptr;      // IP of robot
    std::unique_ptr<HID::Joystick> joystick;

    if (argc > 1) { // if we have command line args
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "viewer") == 0) {
                // start the viewer client
                if (argc < i + 2) {
                    showusage();
                }
                serverIP = argv[++i];
            } else if (strcmp(argv[i], "--no-overlay") == 0) {
                // no honeycomb overlay
                overlayFlag = false;
            } else if (strcmp(argv[i], "--controller") == 0) {
                // enable controller
                if (controllerFlag) {
                    showusage();
                }
                controllerFlag = true;
                controller = true;
            } else if (strcmp(argv[i], "--no-controller") == 0) {
                // disable controller
                if (controllerFlag) {
                    showusage();
                }
                controllerFlag = true;
                controller = false;
            } else if (strcmp(argv[i], "--motor") == 0) { // set type of Motor
                if (motorFlag || i == argc - 1) {
                    showusage();
                }

                i++;
                if (strcmp(argv[i], "dummy") == 0) {
                    robotType = Dummy;
                } else if (strcmp(argv[i], "surveyor") == 0) {
                    robotType = Surveyor;
                } else if (strcmp(argv[i], "arduino") == 0) {
                    robotType = Arduino;
                } else {
                    showusage();
                }
                motorFlag = true;
            } else if (strcmp(argv[i], "--local") == 0) { // run locally
                localFlag = true;
            } else
                showusage();
        }

        if (serverIP) {

            // code run by client (connecting to robot)
            Net::Client client(serverIP);
            client.runInBackground();

            // read video stream from network
            Video::NetSource videoIn(client);            

            // send motor commands over network
            Robots::TankNetSink motorOut(client);

            // start joystick
            if (controller) {
                joystick.reset(new HID::Joystick());
                motorOut.addJoystick(*joystick.get());
                joystick->runInBackground();
            }

            // show video on screen
            OverlayDisplay display(videoIn, overlayFlag);
            display.run();

            return 0;
        }
        if (localFlag) {
            // code run if just showing video locally
            Eye::BeeEye eye;
            OverlayDisplay display(eye, overlayFlag);
            display.run();
            return 0;
        }
    }

    // start appropriate motor device
    std::unique_ptr<Robots::Tank> motor;
#ifdef _WIN32
    std::cout << "Motor disabled on Windows" << std::endl;
    motor.reset(new Robots::Tank());
#else
    switch (robotType) {
    case Surveyor:
        std::cout << "Using Surveyor as motor" << std::endl;
        try {
            motor.reset(new Robots::Surveyor("192.168.1.1", 2000));
        } catch (std::exception &) {
            std::cout << "An error occurred: Disabling motor output"
                      << std::endl;
            motor.reset(new Robots::Tank());
        }
        break;
#ifndef NO_I2C_ROBOT
    case Arduino:
        std::cout << "Using Arduino as motor" << std::endl;
        try {
            motor.reset(new Robots::Norbot());
        } catch (std::exception &) {
            std::cout << "An error occurred: Disabling motor output"
                      << std::endl;
            motor.reset(new Robots::Tank());
        }
        break;
#endif
    default:
        std::cout << "Motor disabled" << std::endl;
        motor.reset(new Robots::Tank());
    }
#endif

    // if using Xbox controller, start it
    if (controller) {
        joystick.reset(new HID::Joystick());
        motor->addJoystick(*joystick.get());
        joystick->runInBackground();
    } else {
        std::cout << "Use of controller is disabled" << std::endl;
    }

    Eye::BeeEye eye;
    if (localFlag) {
        OverlayDisplay display(eye, overlayFlag);
        display.run();
    } else {
        // run main server
        Net::Server server;
        motor->readFromNetwork(server);

        // stream video to network
        Video::NetSink sink(server, eye);

        // run server on main thread
        server.run();
    }

    return 0;
}
