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
#ifdef _WIN32
    return 0;
#else
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
        if (bestcamscore == -1 && bestcamnum == -1) {
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
#endif
}

/* get a PixPro or webcam video device over USB */
const CameraInfo *
getUSB(int *usbDeviceNum)
{
    int sel = 2;

#ifdef _WIN32
    *usbDeviceNum = 0;
#else
    *usbDeviceNum = getCameraByName(
            { "See3CAM_CU40", "PIXPRO SP360 4K", "USB 2.0 Camera" }, sel);
#endif
    switch (sel) {
    case 0:
        return &See3CamDevice;
    case 1:
        return &PixProUSBDevice;
    default:
        return &WebcamDevice;
    }
}
}
