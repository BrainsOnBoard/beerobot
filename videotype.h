#pragma once

using namespace std;

struct vid_t {
    int dev_int, width, height;
    const char* dev_char;
    const char* ini_file;
};

// different video devices
vid_t* get_usb();
vid_t* get_pixpro_wifi();