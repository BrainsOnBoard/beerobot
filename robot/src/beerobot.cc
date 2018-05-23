#include "os/windows_include.h"
#include <cstdlib>

// GeNN robotics includes
#include "robots/motor_dummy.h"
#ifndef _WIN32
#ifndef NO_I2C_ROBOT
#include "robots/motor_i2c.h"
#endif
#include "robots/motor_surveyor.h"
#endif
#include "video/panoramic.h"

// for rendering bee's eye view on screen
#include "eye/beeeye.h"
#include "image/overlaydisplay.h"

// to exchange messages between robot and viewer
#include "net/client.h"
#include "net/server.h"

// for using the Xbox controller to drive the robot
#include "joystickthread.h"

using namespace GeNNRobotics;

/* different types of motor output */
enum MotorType
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
    MotorType motorType = Arduino; // type of Motor to use (server only)
    char *serverIP = nullptr;      // IP of robot
    std::unique_ptr<JoystickThread> joystickThread(nullptr);

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
                    motorType = Dummy;
                } else if (strcmp(argv[i], "surveyor") == 0) {
                    motorType = Surveyor;
                } else if (strcmp(argv[i], "arduino") == 0) {
                    motorType = Arduino;
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
            auto client = std::shared_ptr<Net::Client>(
                    new Net::Client(serverIP));
            if (controller) {
                joystickThread = std::unique_ptr<JoystickThread>(
                        new JoystickThread(client));
            }

            client->startStreaming();
            Image::OverlayDisplay display(overlayFlag);
            display.run(*client.get());
            return 0;
        }
        if (localFlag) {
            // code run if just showing video locally
            auto cam = Video::getPanoramicCamera();
            Eye::BeeEye eye(cam.get());
            Image::OverlayDisplay display(overlayFlag);
            display.run(eye);
            return 0;
        }
    }

    // start appropriate motor device
    Robots::Motor *motor;
#ifdef _WIN32
    std::cout << "Motor disabled on Windows" << std::endl;
    motor = new Robots::MotorDummy();
#else
    switch (motorType) {
    case Surveyor:
        std::cout << "Using Surveyor as motor" << std::endl;
        try {
            motor = new Robots::MotorSurveyor("192.168.1.1", 2000);
        } catch (std::exception &) {
            std::cout << "An error occurred: Disabling motor output"
                      << std::endl;
            motor = new Robots::MotorDummy();
        }
        break;
#ifndef NO_I2C_ROBOT
    case Arduino:
        std::cout << "Using Arduino as motor" << std::endl;
        try {
            motor = new Robots::MotorI2C();
        } catch (std::exception &) {
            std::cout << "An error occurred: Disabling motor output"
                      << std::endl;
            motor = new Robots::MotorDummy();
        }
        break;
#endif
    default:
        std::cout << "Motor disabled" << std::endl;
        motor = new Robots::MotorDummy();
    }
#endif

    // so motor is freed when program exits
    auto pMotor = std::shared_ptr<Robots::Motor>(motor);

    // if using Xbox controller, start it
    if (controller) {
        joystickThread =
                std::unique_ptr<JoystickThread>(new JoystickThread(pMotor));
    } else {
        std::cout << "Use of controller is disabled" << std::endl;
    }

    if (localFlag) {
        auto cam = Video::getPanoramicCamera();
        Eye::BeeEye eye(cam.get());
        Image::OverlayDisplay display(overlayFlag);
        display.run(eye);
    } else {
        // run main server
        Net::Server::runServer(pMotor);
    }

    return 0;
}
