#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "videotype.h"

using namespace std;

namespace Image {

/* get the number for a camera with a given name (-1 if not found) */
template<size_t N>
int
getCameraByName(const char *const (&names)[N], int &selected)
{
    char cname[4096];

    // iterate through devices /dev/video0, /dev/video1 etc. reading the device
    // name from sysfs until the correct device is found
    int defcam = -1;
    int cam1 = -1;
    for (int i = 0; i < 10; i++) {
        string vfn = "/sys/class/video4linux/video" + to_string(i) + "/name";
        ifstream file(vfn, ios::in);
        if (!file.is_open()) {
            continue;
        }

        file.read(cname, sizeof(cname));
        int len = file.gcount() - 1;
        cname[len] = '\0'; // delete the last char, which is always newline
        file.close();

        // compare until colon or end of string (newer kernels add extra crap to
        // the name)
        int ncmp;
        for (ncmp = 0; cname[ncmp] != ':' && cname[ncmp]; ncmp++)
            ;

        // Loop through camera names we're looking for
        // **NOTE* these are in priority order
        for (int c = 0; c < N; c++) {
            // If name matches select it and return its device ID
            if (strncmp(names[c], cname, ncmp) == 0) {
                selected = c;
                return i;
            }
        }

        // At least this is a camera - use it by default
        defcam = i;
    }

    if (defcam == -1) {
        cerr << "Error: Could not find listed video devices and there are no "
                "other cameras attached"
             << endl;
        exit(1);
    }

    cerr << "Warning: Could not find listed video devices. Using default "
            "instead."
         << endl;
    return defcam;
}

/* get a PixPro or webcam video device over USB */
vid_t *
getUSB()
{
    vid_t *vid = new vid_t;
    int sel = 2;

#ifdef _WIN32
    vid->dev_int = 0;
#else
    vid->dev_int = getCameraByName(
            { "See3CAM_CU40", "PIXPRO SP360 4K", "USB 2.0 Camera" }, sel);
#endif
    vid->dev_char = NULL;
    if (sel == 0) {
        vid->width = 640;
        vid->height = 360;
        vid->yaml_file = "see3cam.yaml";
        vid->is_see3cam = true;
    } else if (sel == 1) {
        vid->width = 1440;
        vid->height = 1440;
        vid->yaml_file = "pixpro_usb.yaml";
        vid->is_see3cam = false;
    } else {
        vid->width = 1280;
        vid->height = 720;
        vid->yaml_file = "webcam.yaml";
        vid->is_see3cam = false;
    }
    return vid;
}

/* get PixPro over wifi */
vid_t *
getPixProWifi()
{
    vid_t *vid = new vid_t;
    vid->dev_int = -1;
    vid->dev_char = "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg";
    vid->width = 1024;
    vid->height = 1024;
    vid->yaml_file = "pixpro_wifi.yaml";
    return vid;
}
}
