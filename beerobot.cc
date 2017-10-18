#include <cstdlib>
#include <thread> // std::thread

// for motor control of robot
#include "motor.h"
#include "motor_surveyor.h"
#include "motor_i2c.h"

// to exchange messages between robot and viewer
#include "mainserver.h"
#include "mainclient.h"

// for displaying robot's bee eye view remotely
#include "beeeyeviewer.h"

// for processing single image files
#include "imagefile.h"

// for config mode (i.e. editing .ini files)
#include "beeeyeconfig.h"

// for using the Xbox controller to drive the robot
#include "xboxrobot.h"

using namespace std;

/* show help information */
void showusage()
{
    cout << "Usage: beerobot [--config|--controller|--no-controller] [--motor dummy|surveyor|arduino] [usb|wifi|viewer [ip]]" << endl;
    exit(1);
}

/*
 * Start listening for joystick input on a separate thread and issuing
 * appropriate Motor.tank(,) command. Note that MainClient is a kind of Motor.
 */
void startcontroller(Motor* mtr)
{
    pthread_t cthread;
    pthread_create(&cthread, NULL, &run_controller, mtr);
}

/* main entry point */
int main(int argc, char** argv)
{
    bool controllerflag = false; // controller CL arg present
    bool controller = false; // controller enabled
    bool motorflag = false; // motor CL arg present
    MotorType mtype = Arduino; // type of Motor to use (server only)
    char* server_ip = NULL; // IP of robot

    if (argc > 1) { // if we have command line args
        vid_t* vid = NULL; // video device to read from
        bool config = false; // whether or not to enter config mode (edit .ini files)
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
            } else if (strcmp(argv[i], "viewer") == 0) { // start the viewer client
                if (argc < i + 2)
                    showusage();
                server_ip = argv[++i];
            } else if (strcmp(argv[i], "--controller") == 0) { // enable controller
                if (controllerflag)
                    showusage();
                controllerflag = true;
                controller = true;
            } else if (strcmp(argv[i], "--no-controller") == 0) { // disable controller
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
            } else if (config || !process_file(argv[i]))
                showusage();
        }

        if (server_ip) { // then start the viewer
            // code run by client (connecting to robot)
            MainClient client(server_ip);
            if (controller)
                startcontroller(&client);

            run_eye_viewer();
            return 0;
        } else if (vid) {
            // code run if just showing video locally
            run_eye_config(vid, config);
            return 0;
        }
    }

    // begin code run by robot

    // start appropriate motor device
    Motor *mtr;
    switch (mtype) {
    case Surveyor:
        cout << "Using Surveyor as motor" << endl;
        mtr = new MotorSurveyor("192.168.1.1", 2000);
        break;
    case Arduino:
        cout << "Using Arduino as motor" << endl;
        mtr = new MotorI2C();
        break;
    default:
        cout << "Motor disabled" << endl;
        mtr = new Motor();
    }

    // if using Xbox controller, start it
    if (controller)
        startcontroller(mtr);
    else
        cout << "Use of controller is disabled" << endl;

    // run main server
    MainServer::run_server(mtr);

    do_run_controller = false;
    delete mtr;

    return 0;
}
