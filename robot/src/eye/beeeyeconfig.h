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
inline void
DrawCalibrationLine(cv::Mat &imorig, cv::Point p1, cv::Point p2)
{
    cv::line(imorig, p1, p2, cv::Scalar(0x00, 0xff, 0x00), 2);
}

/* run the bee eye config display */
void
runEyeConfig(const CameraInfo *vid, int vidDeviceNum, bool calibrationEnabled)
{
    BeeEye eye(vid, vidDeviceNum);

    bool doCalibration = true;   // whether calibration screen is visible or not
    int pixelJump = BIG_PX_JUMP; // number of pixels to move by for calibration
                                 // (either 1 or 5)

    cv::Mat imorig, view;
    cv::Mat unwrap(eye.m_Unwrapper->m_UnwrappedResolution, CV_8UC3);
    bool dirtyFlag = false;

    // display remapped camera output on loop until user presses escape
    for (bool runLoop = true; runLoop;) {
        if (!eye.getImage(imorig)) {
            std::cerr << "Error: Could not read from webcam" << std::endl;
            exit(1);
        }

        eye.m_Unwrapper->unwrap(imorig, unwrap);
        eye.getEyeView(view, unwrap);

        if (doCalibration) { // then show calibration screen
            // show unwrapped image
            imshow("unwrapped", unwrap);

            // draw calibration cross at what we've chose as the center
            DrawCalibrationLine(
                    imorig,
                    cv::Point(eye.m_Unwrapper->m_CentrePixel.x - CROSS_SIZE,
                              eye.m_Unwrapper->m_CentrePixel.y),
                    cv::Point(eye.m_Unwrapper->m_CentrePixel.x + CROSS_SIZE,
                              eye.m_Unwrapper->m_CentrePixel.y));
            DrawCalibrationLine(
                    imorig,
                    cv::Point(eye.m_Unwrapper->m_CentrePixel.x,
                              eye.m_Unwrapper->m_CentrePixel.y - CROSS_SIZE),
                    cv::Point(eye.m_Unwrapper->m_CentrePixel.x,
                              eye.m_Unwrapper->m_CentrePixel.y + CROSS_SIZE));

            // draw inner and outer circles, showing the area which we will
            // unwrap
            circle(imorig,
                   eye.m_Unwrapper->m_CentrePixel,
                   eye.m_Unwrapper->m_InnerPixel,
                   cv::Scalar(0x00, 0x00, 0xff),
                   2);
            circle(imorig,
                   eye.m_Unwrapper->m_CentrePixel,
                   eye.m_Unwrapper->m_OuterPixel,
                   cv::Scalar(0xff, 0x00, 0x00),
                   2);

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
            if (calibrationEnabled) {
                if (doCalibration) { // we have to close window explicitly
                    cv::destroyWindow("calibration");
                    cv::destroyWindow("unwrapped");

                    // set opencv window to display full screen
                    cvNamedWindow("bee view", CV_WINDOW_NORMAL);
                    setWindowProperty("bee view",
                                      cv::WND_PROP_FULLSCREEN,
                                      cv::WINDOW_FULLSCREEN);
                } else {
                    cv::destroyWindow("bee view");
                }

                doCalibration = !doCalibration;
            }
            break;
        case KB_ESC: // quit program
            runLoop = false;
            break;
        default:
            if (doCalibration) {
                switch (key) {
                case ' ': // toggle 1px/5px jumps when moving/resizing
                    if (pixelJump == BIG_PX_JUMP)
                        pixelJump = 1;
                    else
                        pixelJump = BIG_PX_JUMP;
                    break;
                case 'w': // make inner circle bigger
                    eye.m_Unwrapper->m_InnerPixel += pixelJump;
                    eye.m_Unwrapper->create();
                    dirtyFlag = true;
                    break;
                case 's': // make inner circle smaller
                    if (eye.m_Unwrapper->m_InnerPixel > 0) {
                        eye.m_Unwrapper->m_InnerPixel -= pixelJump;
                        eye.m_Unwrapper->m_InnerPixel =
                                std::max(0, eye.m_Unwrapper->m_InnerPixel);
                        eye.m_Unwrapper->create();
                        dirtyFlag = true;
                    }
                    break;
                case 'q': // make outer circle bigger
                    eye.m_Unwrapper->m_OuterPixel += pixelJump;
                    eye.m_Unwrapper->create();
                    dirtyFlag = true;
                    break;
                case 'a': // make outer circle smaller
                    if (eye.m_Unwrapper->m_OuterPixel > 0) {
                        eye.m_Unwrapper->m_OuterPixel -= pixelJump;
                        eye.m_Unwrapper->m_OuterPixel =
                                std::max(0, eye.m_Unwrapper->m_OuterPixel);
                        eye.m_Unwrapper->create();
                        dirtyFlag = true;
                    }
                    break;
                case KB_UP: // move centre up
                    eye.m_Unwrapper->m_CentrePixel.y -= pixelJump;
                    eye.m_Unwrapper->create();
                    dirtyFlag = true;
                    break;
                case KB_DOWN: // move centre down
                    eye.m_Unwrapper->m_CentrePixel.y += pixelJump;
                    eye.m_Unwrapper->create();
                    dirtyFlag = true;
                    break;
                case KB_LEFT: // move centre left
                    eye.m_Unwrapper->m_CentrePixel.x -= pixelJump;
                    eye.m_Unwrapper->create();
                    dirtyFlag = true;
                    break;
                case KB_RIGHT: // move centre right
                    eye.m_Unwrapper->m_CentrePixel.x += pixelJump;
                    eye.m_Unwrapper->create();
                    dirtyFlag = true;
                    break;
                }
            }
        }
    }

    if (calibrationEnabled && dirtyFlag) {
        // write params to file
        // in particular we want to remember our calibration settings so we
        // don't have to recalibrate the next time we start the program
        eye.m_Unwrapper->writeFile();
    }
}
}
