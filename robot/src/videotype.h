#pragma once

struct vid_t {
    int dev_int, width, height;
    const char* dev_char;
    const char* ini_file;
    bool is_see3cam;
};

// different video devices
vid_t* get_usb();
vid_t* get_pixpro_wifi();
