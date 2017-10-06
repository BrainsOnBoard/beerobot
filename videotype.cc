
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
int get_camera_by_name(const char* name)
{
    char cname[4096];

    // iterate through devices video0, video1 etc. reading the device name from sysfs
    // until the correct device is found
    for (int i = 0;; i++) {
        string vfn = "/sys/class/video4linux/video" + to_string(i) + "/name";
        ifstream file(vfn, ios::in);
        if (!file.is_open()) {
            if (i == 0) {
                cerr << "Error: Could not find video device " << name << " and there are no other cameras attached" << endl;
                exit(1);
            }

            cerr << "Warning: Could not find video device " << name << ". Using default instead." << endl;
            return 0;
        }

        file.read(cname, sizeof (cname));
        cname[file.gcount() - 1] = 0; // delete the last char, which is always newline
        file.close();

        if (strcmp(name, cname) == 0) { // we've found the correct device
            return i;
        }
    }
}

vid_t* get_pixpro_usb()
{
    vid_t* vid = new vid_t;
    vid->dev_int = get_camera_by_name("PIXPRO SP360 4K");
    vid->dev_char = NULL;
    vid->width = 1440;
    vid->height = 1440;
    vid->ini_file = "beerobot_usb.ini";
    return vid;
}

vid_t* get_pixpro_wifi()
{
    vid_t* vid = new vid_t;
    vid->dev_char = "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg";
    vid->width = 1024;
    vid->height = 1024;
    vid->ini_file = "beerobot_wifi.ini";
}