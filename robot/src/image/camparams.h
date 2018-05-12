#pragma once

#include "opencv2/opencv.hpp"
#include "videotype.h"
#include "yaml-cpp/yaml.h"

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
        m_FilePath = filePath;

        YAML::Node params = YAML::LoadFile(filePath);

        YAML::Node resolution = params["resolution"];
        m_SizeDest.width = resolution[0].as<int>();
        m_SizeDest.height = resolution[1].as<int>();

        YAML::Node center = params["center"];
        m_Center.x = (int) round(m_SizeSource.width * center[0].as<double>());
        m_Center.y = (int) round(m_SizeSource.height * center[1].as<double>());

        YAML::Node radii = params["radius"];
        m_RadiusInner =
                (int) round(m_SizeSource.height * radii["inner"].as<double>());
        m_RadiusOuter =
                (int) round(m_SizeSource.height * radii["outer"].as<double>());

        m_Flipped = params["flipped"].as<bool>();
        m_DegreeOffset = params["degreeoffset"].as<int>();

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
        std::ofstream fd(m_FilePath);

        YAML::Emitter out(fd);
        out << YAML::BeginMap;

        // resolution
        std::vector<int> resolution(2);
        resolution.at(0) = m_SizeDest.width;
        resolution.at(1) = m_SizeDest.height;
        out << YAML::Key << "resolution" << YAML::Value << YAML::Flow
            << resolution;

        // centre
        std::vector<double> center(2);
        center.at(0) = (double) m_Center.x / (double) m_SizeSource.width;
        center.at(1) = (double) m_Center.y / (double) m_SizeSource.height;
        out << YAML::Key << "center" << YAML::Value << YAML::Flow << center;

        // radii
        out << YAML::Key << "radius" << YAML::BeginMap;
        out << YAML::Key << "inner"
            << (double) m_RadiusInner / (double) m_SizeSource.height;
        out << YAML::Key << "outer"
            << (double) m_RadiusOuter / (double) m_SizeSource.height;
        out << YAML::EndMap;

        // other
        out << YAML::Key << "flipped" << YAML::Value << m_Flipped;
        out << YAML::Key << "degreeoffset" << YAML::Value << m_DegreeOffset;

        out << YAML::EndMap;
        fd.close();
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
