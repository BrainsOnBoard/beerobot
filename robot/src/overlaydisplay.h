// OpenCV
#include <opencv2/opencv.hpp>

// GeNN robotics includes
#include "video/display.h"
#include "video/input.h"
#include "os/screen.h"

using namespace GeNNRobotics;

class OverlayDisplay : public Video::Display
{
public:
    OverlayDisplay(Video::Input &videoInput, bool showOverlay)
      : Display(videoInput), m_ShowOverlay(showOverlay)
    {
        if (!showOverlay) {
            std::cout << "Image overlay disabled" << std::endl;
            return;
        }

        // create overlay
        cv::Size screenSize = OS::Screen::getResolution();
        if (screenSize.empty()) {
            throw std::runtime_error("Could not get screen resolution");
        }

        std::cout << "Screen resolution: " << screenSize << std::endl;

        int w = (int) round((float) screenSize.height * 970.0 / 1048.0);
        int xoff = (screenSize.width - w) / 2;

        cv::Mat overlay = cv::imread("honeycomb_overlay.png", 3);
        cv::resize(overlay, m_Overlay, screenSize, 0, 0, cv::INTER_CUBIC);

        m_OverlayInner = m_Overlay(cv::Range::all(), cv::Range(xoff, xoff + w));
        m_Mask = (m_OverlayInner == 0);
        m_ImInner.create(m_OverlayInner.size(), m_OverlayInner.type());
    }

protected:
    virtual bool readFrame(cv::Mat &frame) override
    {
        if (!m_VideoInput->readFrame(m_ImEyeOut)) {
            return false;
        }
        if (!m_ShowOverlay) {
            frame = m_ImEyeOut;
            return true;
        }

        cv::resize(m_ImEyeOut, m_ImInner, m_ImInner.size());
        m_ImInner.copyTo(m_OverlayInner, m_Mask);
        frame = m_Overlay;
        return true;
    }

private:
    bool m_ShowOverlay;
    cv::Mat m_Overlay, m_OverlayInner, m_ImInner, m_Mask, m_ImEyeOut;
};
