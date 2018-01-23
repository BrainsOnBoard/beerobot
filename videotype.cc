#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "videotype.h"

using namespace std;

namespace
{
/* get the number for a camera with a given name (-1 if not found) */
template<size_t N>
int get_camera_by_name(const char * const (&names)[N], int &selected)
{
    char cname[4096];

    // iterate through devices /dev/video0, /dev/video1 etc. reading the device name from sysfs
    // until the correct device is found
    int defcam = -1;
    int cam1 = -1;
    for (int i = 0; i < 10; i++) {
        string vfn = "/sys/class/video4linux/video" + to_string(i) + "/name";
        ifstream file(vfn, ios::in);
        if (!file.is_open()) {
            continue;
        }

        file.read(cname, sizeof (cname));
        cname[file.gcount() - 1] = '\0'; // delete the last char, which is always newline
        file.close();

        // Loop through camera names we're looking for
        // **NOTE* these are in priority order
        for(int c = 0; c < N; c++) {
            // If name matches select it and return it's device ID
            if(strcmp(names[c], cname) == 0) {
                selected = c;
                return i;
            }
        }
        
        // At least this is a camera - use it by default
        defcam = i;
    }


    if (defcam == -1) {
        cerr << "Error: Could not find listed video devices and there are no other cameras attached" << endl;
        exit(1);
    }

    cerr << "Warning: Could not find listed video devices. Using default instead." << endl;
    return defcam;
}
}

/* get a PixPro or webcam video device over USB */
vid_t* get_usb()
{
    vid_t* vid = new vid_t;
    int sel = 2;
    vid->dev_int = get_camera_by_name({"See3CAM_CU40", "PIXPRO SP360 4K", "USB 2.0 Camera"}, sel);
    vid->dev_char = NULL;
    if (sel == 0) {
        vid->width = 640;
        vid->height = 360;
        vid->ini_file = "beerobot_see3cam_usb.ini";
        vid->is_see3cam = true;
    }
    else if (sel == 1) {
        vid->width = 1440;
        vid->height = 1440;
        vid->ini_file = "beerobot_pixpro_usb.ini";
        vid->is_see3cam = false;
    } else {
        vid->width = 1280;
        vid->height = 720;
        vid->ini_file = "beerobot_webcam_usb.ini";
        vid->is_see3cam = false;
    }
    return vid;
}

/* get PixPro over wifi */
vid_t* get_pixpro_wifi()
{
    vid_t* vid = new vid_t;
    vid->dev_int = -1;
    vid->dev_char = "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg";
    vid->width = 1024;
    vid->height = 1024;
    vid->ini_file = "beerobot_pixpro_wifi.ini";
}
