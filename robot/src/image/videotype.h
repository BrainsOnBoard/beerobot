#pragma once

struct vid_t {
    int dev_int, width, height;
    const char* dev_char;
    const char* yaml_file;
    bool is_see3cam;
};

namespace Image {
// different video devices
vid_t* getUSB();
vid_t* getPixProWifi();
}
