#pragma once

struct CameraInfo
{
    int width, height;
    const char *deviceURL;
    const char *configFile;
    bool isSee3Cam;
};

namespace Image {
const CameraInfo *
getUSB(int *deviceNum);

static const CameraInfo See3CamDevice{ .width = 640,
                                  .height = 360,
                                  .configFile = "see3cam.yaml",
                                  .isSee3Cam = true };

static const CameraInfo PixProUSBDevice{
    .width = 1440,
    .height = 1440,
    .configFile = "pixpro_usb.yaml",
};

static const CameraInfo PixProWifiDevice{
    .width = 1024,
    .height = 1024,
    .deviceURL = "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg",
    .configFile = "pixpro_wifi.yaml"
};

static const CameraInfo WebcamDevice{ .width = 1280,
                                 .height = 720,
                                 .configFile = "webcam.yaml" };
}
