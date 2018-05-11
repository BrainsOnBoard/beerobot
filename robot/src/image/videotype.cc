#include "videotype.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

namespace Image {

/* get the number for a camera with a given name (-1 if not found) */
template<size_t N>
int
getCameraByName(const char *const (&names)[N], int &selected)
{
    char cname[4096];

    // iterate through devices /dev/video0, /dev/video1 etc. reading the device
    // name from sysfs until the correct device is found
    int bestcamnum = -1;
    int bestcamscore = -1;
    for (int i = 0; i < 10; i++) {
        std::string vfn =
                "/sys/class/video4linux/video" + std::to_string(i) + "/name";
        std::ifstream file(vfn, std::ios::in);
        if (!file.is_open()) {
            continue;
        }

        file.read(cname, sizeof(cname));
        file.close();

        // compare until colon or end (newer kernels add extra crap to the name)
        int len;
        for (len = 0; cname[len] != ':' && cname[len] != '\n'; len++)
            ;
        cname[len] = '\0';
        std::cout << "Found camera #" << i << ": " << cname << std::endl;

        // Loop through camera names we're looking for
        // **NOTE* these are in priority order
        for (int j = 0; j < N; j++) {
            // If name matches select it and return its device ID
            if (strcmp(names[j], cname) == 0) {
                int cscore = N - j;
                if (cscore > bestcamscore) {
                    bestcamnum = i;
                    selected = j;
                    bestcamscore = cscore;
                }
                break;
            }
        }

        // At least this is a camera - use it by default
        if (bestcamscore == -1) {
            bestcamnum = i;
        }
    }

    if (bestcamscore == -1) {
        if (bestcamnum == -1) {
            std::cerr << "Error: Could not find listed video devices and there "
                         "are no other cameras attached"
                      << std::endl;
            exit(1);
        } else {
            std::cerr << "Warning: Could not find listed video devices. Using "
                         "default instead."
                      << std::endl;
        }
    }

    std::cout << "Selecting camera #" << bestcamnum << std::endl;
    return bestcamnum;
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
