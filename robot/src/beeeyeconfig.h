#pragma once

#include "beeeye.h"

namespace Eye {
const int CROSS_SIZE = 20; // size of calibration cross

// keyboard key codes
const int KB_LEFT = 81;
const int KB_UP = 82;
const int KB_RIGHT = 83;
const int KB_DOWN = 84;
const int KB_ESC = 27;

// number of pixels to move/resize things by in calibration mode
const int BIG_PX_JUMP = 5;

/* draws a line for the calibration cross */
inline void DrawCalibrationLine(cv::Mat &imorig, cv::Point p1, cv::Point p2)
{
    cv::line(imorig, p1, p2, cv::Scalar(0x00, 0xff, 0x00), 2);
}

/* run the bee eye config display */
void runEyeConfig(vid_t* vid, bool calib_enabled)
{
    BeeEye eye(vid);

    bool do_calib = true; // whether calibration screen is visible or not
    int px_jump = BIG_PX_JUMP; // number of pixels to move by for calibration (either 1 or 5)

    cv::Mat imorig, unwrap, view;

    // display remapped camera output on loop until user presses escape
    for (bool do_run = true; do_run;) {
        if (!eye.getImage(imorig)) {
            std::cerr << "Error: Could not read from webcam" << std::endl;
            exit(1);
        }

        eye.getUnwrappedImage(unwrap, imorig);
        eye.getEyeView(view, unwrap);

        if (do_calib) { // then show calibration screen
            // show unwrapped image
            imshow("unwrapped", unwrap);

            // draw calibration cross at what we've chose as the center
            DrawCalibrationLine(imorig, cv::Point(eye.params.cent.x - CROSS_SIZE, eye.params.cent.y), cv::Point(eye.params.cent.x + CROSS_SIZE, eye.params.cent.y));
            DrawCalibrationLine(imorig, cv::Point(eye.params.cent.x, eye.params.cent.y - CROSS_SIZE), cv::Point(eye.params.cent.x, eye.params.cent.y + CROSS_SIZE));

            // draw inner and outer circles, showing the area which we will unwrap
            circle(imorig, eye.params.cent, eye.params.r_inner, cv::Scalar(0x00, 0x00, 0xff), 2);
            circle(imorig, eye.params.cent, eye.params.r_outer, cv::Scalar(0xff, 0x00, 0x00), 2);

            // show the image
            imshow("calibration", imorig);
        } else {
            // show bee's eye view
            imshow("bee view", view);
        }

        // read keypress in
        int key = cv::waitKey(1) & 0xff;
        /*if (key != 0xff)
            cout << "key: " << key << endl;*/
        switch (key) {
        case 'c': // toggle display of calibration screen
            if (calib_enabled) {
                if (do_calib) { // we have to close window explicitly
                    cv::destroyWindow("calibration");
                    cv::destroyWindow("unwrapped");

                    // set opencv window to display full screen
                    cvNamedWindow("bee view", CV_WINDOW_NORMAL);
                    setWindowProperty("bee view", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
                } else {
                    cv::destroyWindow("bee view");
                }

                do_calib = !do_calib;
            }
            break;
        case KB_ESC: // quit program
            do_run = false;
            break;
        default:
            if (do_calib) {
                switch (key) {
                case ' ': // toggle 1px/5px jumps when moving/resizing
                    if (px_jump == BIG_PX_JUMP)
                        px_jump = 1;
                    else
                        px_jump = BIG_PX_JUMP;
                    break;
                case 'w': // make inner circle bigger
                    eye.params.r_inner += px_jump;
                    eye.params.generate_map();
                    break;
                case 's': // make inner circle smaller
                    if (eye.params.r_inner > 0) {
                        eye.params.r_inner -= px_jump;
                        eye.params.r_inner = std::max(0, eye.params.r_inner);
                        eye.params.generate_map();
                    }
                    break;
                case 'q': // make outer circle bigger
                    eye.params.r_outer += px_jump;
                    eye.params.generate_map();
                    break;
                case 'a': // make outer circle smaller
                    if (eye.params.r_outer > 0) {
                        eye.params.r_outer -= px_jump;
                        eye.params.r_outer = std::max(0, eye.params.r_outer);
                        eye.params.generate_map();
                    }
                    break;
                case KB_UP: // move centre up
                    eye.params.cent.y -= px_jump;
                    eye.params.generate_map();
                    break;
                case KB_DOWN: // move centre down
                    eye.params.cent.y += px_jump;
                    eye.params.generate_map();
                    break;
                case KB_LEFT: // move centre left
                    eye.params.cent.x -= px_jump;
                    eye.params.generate_map();
                    break;
                case KB_RIGHT: // move centre right
                    eye.params.cent.x += px_jump;
                    eye.params.generate_map();
                    break;
                }
            }
        }
    }

    if (calib_enabled) {
        // write params to file
        // in particular we want to remember our calibration settings so we don't
        // have to recalibrate the next time we start the program
        eye.params.write();
    }
}
}
