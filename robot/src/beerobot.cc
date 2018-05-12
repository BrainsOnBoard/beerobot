#include <cstdlib>

// for motor control of robot
#include "common/motor_dummy.h"
#ifndef _WIN32
#ifndef NO_I2C_ROBOT
#include "common/motor_i2c.h"
#endif
#include "common/motor_surveyor.h"
#endif

// to exchange messages between robot and viewer
#include "net/mainclient.h"
#include "net/mainserver.h"

// for displaying robot's bee eye view remotely
#include "eye/beeeyeviewer.h"

// for config mode (i.e. editing .ini files)
#include "eye/beeeyeconfig.h"

#include "net/imagereceiver.h"

// for processing single image files
#include "image/file.h"

// for using the Xbox controller to drive the robot
#include "xboxrobot.h"

using namespace std;

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
    cout << "Usage: beerobot "
            "[--config|--controller|--no-controller|--local|--no-overlay] "
            "[--motor dummy|surveyor|arduino] [usb|wifi|viewer [ip]]"
         << endl;
    exit(1);
}

/* main entry point */
int
main(int argc, char **argv)
{
    bool controllerflag = false; // controller CL arg present
    bool controller = false;     // controller enabled
    bool motorflag = false;      // motor CL arg present
    bool localflag = false;
    bool overlayflag = true;
    MotorType mtype = Arduino;       // type of Motor to use (server only)
    char *server_ip = nullptr;       // IP of robot
    const CameraInfo *vid = nullptr; // video device to read from
    int vidDeviceNum = -1;

    if (argc > 1) { // if we have command line args
        bool config =
                false; // whether or not to enter config mode (edit .ini files)
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--config") == 0) { // enable config mode
                if (config) {
                    showusage();
                }
                config = true;
            } else if (vid) {
                showusage();
            } else if (strcmp(argv[i], "usb") == 0) { // use a USB camera
                vid = Image::getUSB(&vidDeviceNum);
            } else if (strcmp(argv[i], "wifi") == 0) { // use PixPro over wifi
                vid = &Image::PixProWifiDevice;
            } else if (strcmp(argv[i], "viewer") == 0) {
                // start the viewer client
                if (argc < i + 2) {
                    showusage();
                }
                server_ip = argv[++i];
            } else if (strcmp(argv[i], "--no-overlay") == 0) {
                // no honeycomb overlay
                overlayflag = false;
            } else if (strcmp(argv[i], "--controller") == 0) {
                // enable controller
                if (controllerflag)
                    showusage();
                controllerflag = true;
                controller = true;
            } else if (strcmp(argv[i], "--no-controller") == 0) {
                // disable controller
                if (controllerflag) {
                    showusage();
                }
                controllerflag = true;
                controller = false;
            } else if (strcmp(argv[i], "--motor") == 0) { // set type of Motor
                if (motorflag || i == argc - 1) {
                    showusage();
                }

                i++;
                if (strcmp(argv[i], "dummy") == 0) {
                    mtype = Dummy;
                } else if (strcmp(argv[i], "surveyor") == 0) {
                    mtype = Surveyor;
                } else if (strcmp(argv[i], "arduino") == 0) {
                    mtype = Arduino;
                } else {
                    showusage();
                }
                motorflag = true;
            } else if (strcmp(argv[i], "--local") == 0) { // run locally
                localflag = true;
            } else if (config || !Image::processFile(argv[i]))
                showusage();
        }

        if (!localflag) {
            if (server_ip) { // then start the viewer
                // code run by client (connecting to robot)
                Net::MainClient client(server_ip);
                if (controller) {
                    Controller::start(&client);
                }

                Net::ImageReceiver recv;
                Eye::runEyeViewer(recv, overlayflag);

                // TODO: this is a case where smart pointers would be better
                if (controller) {
                    Controller::stop();
                }
                return 0;
            } else if (config || vid) {
                if (!vid) { // default to usb for config
                    vid = Image::getUSB(&vidDeviceNum);
                }

                // code run if just showing video locally
                Eye::runEyeConfig(vid, vidDeviceNum, config);
                return 0;
            }
        }
    }

    // start appropriate motor device
    Motor *mtr;
#ifdef _WIN32
    cout << "Motor disabled on Windows" << endl;
    mtr = new MotorDummy();
#else
    switch (mtype) {
    case Surveyor:
        cout << "Using Surveyor as motor" << endl;
        try {
            mtr = new MotorSurveyor("192.168.1.1", 2000);
        } catch (exception &e) {
            cout << "An error occurred: Disabling motor output" << endl;
            mtr = new MotorDummy();
        }
        break;
#ifndef NO_I2C_ROBOT
    case Arduino:
        cout << "Using Arduino as motor" << endl;
        try {
            mtr = new MotorI2C();
        } catch (exception &e) {
            cout << "An error occurred: Disabling motor output" << endl;
            mtr = new MotorDummy();
        }
        break;
#endif
    default:
        cout << "Motor disabled" << endl;
        mtr = new MotorDummy();
    }
#endif

    // if using Xbox controller, start it
    if (controller) {
        Controller::start(mtr);
    } else {
        cout << "Use of controller is disabled" << endl;
    }

    if (localflag) {
        if (!vid) {
            vid = Image::getUSB(&vidDeviceNum);
        }

        Eye::BeeEye eye(vid, vidDeviceNum);
        Eye::runEyeViewer(eye, overlayflag);
    } else {
        // run main server
        Net::MainServer::runServer(mtr);
    }

    // TODO: this is a case where smart pointers would be better
    if (controller) {
        Controller::stop();
    } else {
        delete mtr;
    }

    return 0;
}
