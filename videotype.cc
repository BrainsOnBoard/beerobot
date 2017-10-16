
/*
 * File:   videotype.cc
 * Author: alex
 *
 * Created on 06 September 2017, 12:25
 */

#include <cstdlib>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

#include "videotype.h"

using namespace std;

/* get the number for a camera with a given name (-1 if not found) */
int get_camera_by_name(const string name0, const string name1, int &selected)
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
        cname[file.gcount() - 1] = 0; // delete the last char, which is always newline
        file.close();

        if (string(cname).compare(0, name0.length(), name0) == 0) { // we've found the correct device
            selected = 0;
            return i;
        }
        if (string(cname).compare(name1) == 0) {
            cam1 = i;
        }

        defcam = i;
    }

    if (cam1 >= 0) {
        selected = 1;
        return cam1;
    }

    if (defcam == -1) {
        cerr << "Error: Could not find video device " << name0 << " or " << name1 << " and there are no other cameras attached" << endl;
        exit(1);
    }

    cerr << "Warning: Could not find video device " << name0 << " or " << name1 << ". Using default instead." << endl;
    return defcam;
}

vid_t* get_usb()
{
    vid_t* vid = new vid_t;
    int sel;
    vid->dev_int = get_camera_by_name("PIXPRO SP360 4K", "USB 2.0 Camera", sel);
    vid->dev_char = NULL;
    if (sel == 0) {
        vid->width = 1440;
        vid->height = 1440;
        vid->ini_file = "beerobot_pixpro_usb.ini";
    } else {
        vid->width = 1280;
        vid->height = 720;
        vid->ini_file = "beerobot_webcam_usb.ini";
    }
    return vid;
}

vid_t* get_pixpro_wifi()
{
    vid_t* vid = new vid_t;
    vid->dev_char = "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg";
    vid->width = 1024;
    vid->height = 1024;
    vid->ini_file = "beerobot_pixpro_wifi.ini";
}
