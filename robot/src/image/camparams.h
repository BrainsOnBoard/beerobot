#pragma once

#include "opencv2/opencv.hpp"
#include "videotype.h"

namespace Image {
class CamParams
{
public:
    std::string m_FilePath;

    // source and dest image sizes
    cv::Size m_SizeSource, m_SizeDest;

    // calibration values
    cv::Point m_Center;
    int m_RadiusInner, m_RadiusOuter;

    // x and y pixel maps
    cv::Mat m_MapX, m_MapY;

    bool m_Flipped;
    int m_DegreeOffset;

    CamParams()
      : m_SizeDest(1280, 400)
    {}

    CamParams(const CameraInfo *vid)
      : CamParams(vid->configFile, vid->width, vid->height)
    {}

    /*
     * Read parameters from YAML file.
     */
    CamParams(std::string filePath, int width = 1280, int height = 400)
      : m_SizeSource(width, height)
    {
        // open YAML file
        m_FilePath = filePath;
        cv::FileStorage fs(m_FilePath, cv::FileStorage::READ);

        // resolution
        fs["resolution"] >> m_SizeDest;

        // centre
        std::vector<double> center(2);
        fs["center"] >> center;
        m_Center.x = (int) round(m_SizeSource.width * center[0]);
        m_Center.y = (int) round(m_SizeSource.height * center[1]);

        // inner and outer radius
        cv::FileNode radii = fs["radius"];
        m_RadiusInner =
                (int) round(m_SizeSource.height * (double) radii["inner"]);
        m_RadiusOuter =
                (int) round(m_SizeSource.height * (double) radii["outer"]);

        // other params
        fs["flipped"] >> m_Flipped;
        fs["degreeoffset"] >> m_DegreeOffset;

        // close YAML file
        fs.release();

        // define our x and y pixel maps
        m_MapX = cv::Mat(m_SizeDest, CV_32FC1);
        m_MapY = cv::Mat(m_SizeDest, CV_32FC1);
    }

    /*
     * Write parameters to YAML file.
     */
    void write()
    {
        std::cout << "Writing to " << m_FilePath << "..." << std::endl;
        cv::FileStorage fs(m_FilePath, cv::FileStorage::WRITE);

        // resolution
        fs << "resolution" << m_SizeDest;

        // centre
        cv::Point2d center = { (double) m_Center.x /
                                       (double) m_SizeSource.width,
                               m_Center.y / (double) m_SizeSource.height };
        fs << "center" << center;

        // radii
        fs << "radius"
           << "{"
           << "inner" << (double) m_RadiusInner / (double) m_SizeSource.height
           << "outer" << (double) m_RadiusOuter / (double) m_SizeSource.height
           << "}";

        // other
        fs << "flipped" << m_Flipped;
        fs << "degreeoffset" << m_DegreeOffset;

        // close file
        fs.release();
    }

    /* generate a new pixel map, based on the current calibration settings */
    void generateMap()
    {
        for (int i = 0; i < m_SizeDest.height; i++) {
            for (int j = 0; j < m_SizeDest.width; j++) {
                // Get i as a fraction of unwrapped height, flipping if desired
                float frac =
                        m_Flipped
                                ? 1.0f - ((float) i / (float) m_SizeDest.height)
                                : ((float) i / (float) m_SizeDest.height);

                // Convert i and j to polar
                float r =
                        frac * (m_RadiusOuter - m_RadiusInner) + m_RadiusInner;

                float th = 2 * (float) M_PI *
                           (((float) j / (float) m_SizeDest.width) -
                            ((float) m_DegreeOffset / 360.0f));
                float x = m_Center.x - r * sin(th);
                float y = m_Center.y + r * cos(th);
                m_MapX.at<float>(i, j) = x;
                m_MapY.at<float>(i, j) = y;
            }
        }
    }
};
}
