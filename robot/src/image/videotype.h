#pragma once

struct vid_t
{
    int width, height;
    const char *dev_char;
    const char *yaml_file;
    bool is_see3cam;
};

namespace Image {
const vid_t *
getUSB(int *deviceNum);

static const vid_t See3CamDevice{ .width = 640,
                                  .height = 360,
                                  .yaml_file = "see3cam.yaml",
                                  .is_see3cam = true };

static const vid_t PixProUSBDevice{
    .width = 1440,
    .height = 1440,
    .yaml_file = "pixpro_usb.yaml",
};

static const vid_t PixProWifiDevice{
    .width = 1024,
    .height = 1024,
    .dev_char = "http://172.16.0.254:9176/;dummy_parameter=bee.mjpg",
    .yaml_file = "pixpro_wifi.yaml"
};

static const vid_t WebcamDevice{ .width = 1280,
                                 .height = 720,
                                 .yaml_file = "webcam.yaml" };
}
