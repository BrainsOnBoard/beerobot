#include "os/screen.h"
#include "video/input.h"
#include "video/simpledisplay.h"

#include <opencv2/opencv.hpp>

namespace Image {

class OverlayDisplay : public Video::SimpleDisplay
{
public:
    OverlayDisplay(bool showOverlay)
      : m_ShowOverlay(showOverlay)
    {
        if (!showOverlay) {
            std::cout << "Image overlay disabled" << std::endl;
            return;
        }

        // create overlay
        cv::Size screenSize;
        if (!OS::Screen::getResolution(screenSize)) {
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

    void getNextFrame(Video::Input &videoInput, cv::Mat &frame)
    {
        if (!videoInput.readFrame(m_ImEyeOut)) {
            throw std::runtime_error("Error reading from video input");
        }
        if (!m_ShowOverlay) {
            frame = m_ImEyeOut;
            return;
        }

        cv::resize(m_ImEyeOut, m_ImInner, m_ImInner.size());
        m_ImInner.copyTo(m_OverlayInner, m_Mask);
        frame = m_Overlay;
    }

private:
    bool m_ShowOverlay;
    cv::Mat m_Overlay, m_OverlayInner, m_ImInner, m_Mask, m_ImEyeOut;
};
}
