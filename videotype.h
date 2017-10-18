#pragma once

using namespace std;

struct vid_t {
    int dev_int, width, height;
    const char* dev_char;
    const char* ini_file;
};

/* get the number for a camera with a given name (-1 if not found) */
int get_camera_by_name(const string name);

// different video devices
vid_t* get_usb();
vid_t* get_pixpro_wifi();
