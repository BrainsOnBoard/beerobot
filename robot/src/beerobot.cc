#include <cstdlib>

// for motor control of robot
#include "motor_dummy.h"
#ifndef _WIN32
#include "common/motor_i2c.h"
#include "common/motor_surveyor.h"
#endif

// to exchange messages between robot and viewer
#include "mainclient.h"
#include "mainserver.h"

// for displaying robot's bee eye view remotely
#include "beeeyeviewer.h"

#include "imagereceiver.h"

// for processing single image files
#include "imagefile.h"

// for config mode (i.e. editing .ini files)
#include "beeeyeconfig.h"

#ifndef _WIN32
// for using the Xbox controller to drive the robot
#include "xboxrobot.h"
#endif

using namespace std;

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
    MotorType mtype = Arduino; // type of Motor to use (server only)
    char *server_ip = NULL;    // IP of robot
    vid_t *vid = NULL;         // video device to read from

    if (argc > 1) { // if we have command line args
        bool config =
                false; // whether or not to enter config mode (edit .ini files)
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--config") == 0) { // enable config mode
                if (config)
                    showusage();
                config = true;
            } else if (vid) {
                showusage();
            } else if (strcmp(argv[i], "usb") == 0) { // use a USB camera
                vid = get_usb();
            } else if (strcmp(argv[i], "wifi") == 0) { // use PixPro over wifi
                vid = get_pixpro_wifi();
            } else if (strcmp(argv[i], "viewer") ==
                       0) { // start the viewer client
                if (argc < i + 2)
                    showusage();
                server_ip = argv[++i];
            } else if (strcmp(argv[i], "--no-overlay") ==
                       0) { // no honeycomb overlay
                overlayflag = false;
            } else if (strcmp(argv[i], "--controller") ==
                       0) { // enable controller
                if (controllerflag)
                    showusage();
                controllerflag = true;
                controller = true;
            } else if (strcmp(argv[i], "--no-controller") ==
                       0) { // disable controller
                if (controllerflag)
                    showusage();
                controllerflag = true;
                controller = false;
            } else if (strcmp(argv[i], "--motor") == 0) { // set type of Motor
                if (motorflag || i == argc - 1)
                    showusage();
                i++;
                if (strcmp(argv[i], "dummy") == 0)
                    mtype = Dummy;
                else if (strcmp(argv[i], "surveyor") == 0)
                    mtype = Surveyor;
                else if (strcmp(argv[i], "arduino") == 0)
                    mtype = Arduino;
                else
                    showusage();
                motorflag = true;
            } else if (strcmp(argv[i], "--local") == 0) { // run locally
                localflag = true;
            } else if (config || !process_file(argv[i]))
                showusage();
        }

        if (!localflag) {
            if (server_ip) { // then start the viewer
                // code run by client (connecting to robot)
                MainClient *client = new MainClient(server_ip);
#ifndef _WIN32
                if (controller) {
                    Controller::start(client);
                }
#endif

                ImageReceiver recv;
                run_eye_viewer(recv, overlayflag);

#ifndef _WIN32

                // TODO: this is a case where smart pointers would be better
                if (controller) {
                    Controller::stop();
                } else {
                    delete client;
                }
#endif

                return 0;
            } else if (config || vid) {
                if (!vid) // default to usb for config
                    vid = get_usb();

                // code run if just showing video locally
                run_eye_config(vid, config);
                return 0;
            }
        }
    }

    // start appropriate motor device
    Motor *mtr;
    switch (mtype) {
#ifndef _WIN32
    case Surveyor:
        cout << "Using Surveyor as motor" << endl;
        try {
            mtr = new MotorSurveyor("192.168.1.1", 2000);
        } catch (exception &e) {
            cout << "An error occurred: Disabling motor output" << endl;
            mtr = new MotorDummy();
        }
        break;
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

    // if using Xbox controller, start it
    if (controller) {
#ifdef _WIN32
        cout << "Controller is disabled in Windows" << endl;
#else
        Controller::start(mtr);
#endif
    } else {
        cout << "Use of controller is disabled" << endl;
    }

    if (localflag) {
        if (!vid)
            vid = get_usb();

        BeeEye eye(vid);
        run_eye_viewer(eye, overlayflag);
    } else {
        // run main server
        MainServer::run_server(mtr);
    }

#ifndef _WIN32
    // TODO: this is a case where smart pointers would be better
    if (controller) {
        Controller::stop();
    } else {
        delete mtr;
    }
#else
    delete mtr;
#endif
    return 0;
}
