#include <cstdlib>
#include <thread> // std::thread

// for motor control of robot
#include "motor.h"
#include "motor_surveyor.h"
#include "motor_i2c.h"

#include "mainserver.h"
#include "mainclient.h"

#include "imagereceiver.h"
#include "beeeyeviewer.h"

#include "imagefile.h"
#include "beeeyeconfig.h"
#include "xboxrobot.h" // for using the Xbox controller to drive the robot

using namespace std;

void showusage()
{
    cout << "Usage: beerobot [--config|--controller|--no-controller] [--motor dummy|surveyor|arduino] [usb|wifi|viewer [ip]]" << endl;
    exit(1);
}

void startcontroller(Motor* mtr)
{
    pthread_t cthread;
    pthread_create(&cthread, NULL, &run_controller, mtr);
}

int main(int argc, char** argv)
{
    bool controllerflag = false;
    bool motorflag = false;
    MotorType mtype = Arduino;
    bool controller = false;
    char* viewer_ip = NULL;
    if (argc > 1) {
        vid_t* vid = NULL;
        bool config = false;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--config") == 0) {
                if (config)
                    showusage();
                config = true;
            } else if (vid) {
                showusage();
            } else if (strcmp(argv[i], "usb") == 0) {
                vid = get_usb();
            } else if (strcmp(argv[i], "wifi") == 0) {
                vid = get_pixpro_wifi();
            } else if (strcmp(argv[i], "viewer") == 0) {
                if (argc < i + 2)
                    showusage();
                viewer_ip = argv[++i];
            } else if (strcmp(argv[i], "--controller") == 0) {
                if (controllerflag)
                    showusage();
                controllerflag = true;
                controller = true;
            } else if (strcmp(argv[i], "--no-controller") == 0) {
                if (controllerflag)
                    showusage();
                controllerflag = true;
                controller = false;
            } else if (strcmp(argv[i], "--motor") == 0) {
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

        if (viewer_ip) {
            // code run by client (connecting to robot)
            MainClient client(viewer_ip);
            if (controller)
                startcontroller(&client);

            ImageReceiver recv;
            run_eye_viewer(recv);
            return 0;
        } else if (vid) {
            // code run if just showing video locally
            run_eye_config(vid, config);
            return 0;
        }
    }

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

    if (controller)
        startcontroller(mtr);
    else
        cout << "Use of controller is disabled" << endl;

    MainServer::run_server(mtr);

    do_run_controller = false;
    delete mtr;

    return 0;
}
