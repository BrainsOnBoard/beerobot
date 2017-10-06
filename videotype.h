
/*
 * File:   videotype.h
 * Author: alex
 *
 * Created on 06 September 2017, 12:08
 */

#ifndef VIDEOTYPE_H
#define VIDEOTYPE_H

using namespace std;

struct vid_t {
    int dev_int, width, height;
    const char* dev_char;
    const char* ini_file;
};

/* get the number for a camera with a given name (-1 if not found) */
int get_camera_by_name(const string name);

// different video devices
vid_t* get_pixpro_usb();
vid_t* get_pixpro_wifi();

#endif /* VIDEOTYPE_H */

