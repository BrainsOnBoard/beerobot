#include <iostream>
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

class CamParams
{
public:
    cv::Size m_SizeSource, m_SizeDest;
    cv::Point m_Center;
    int m_RadiusInner, m_RadiusOuter;
    bool m_Flipped;
    int m_DegreeOffset;

    CamParams(std::string filePath) {
        YAML::Node params = YAML::LoadFile(filePath);

        YAML::Node resolution = params["resolution"];
        m_SizeSource.width = resolution[0].as<int>();
        m_SizeSource.height = resolution[1].as<int>();

        YAML::Node center = params["center"];
        m_Center.x = round(m_SizeSource.width * center[0].as<double>());
        m_Center.y = round(m_SizeSource.height * center[1].as<double>());

        YAML::Node radii = params["radius"];
        m_RadiusInner = round(m_SizeSource.height * radii["inner"].as<double>());
        m_RadiusOuter = round(m_SizeSource.height * radii["outer"].as<double>());

        m_Flipped = params["flipped"].as<bool>();
        m_DegreeOffset = params["degreeoffset"].as<int>();
    }
};

int
main(int argc, char **argv)
{
    CamParams params("webcam_usb.yaml");
    std::cout << "Size: " << params.m_SizeSource << std::endl
              << "Centre: " << params.m_Center << std::endl
              << "Inner radius: " << params.m_RadiusInner << std::endl
              << "Outer radius: " << params.m_RadiusOuter << std::endl
              << "Flipped: " << params.m_Flipped << std::endl
              << "Offset: " << params.m_DegreeOffset << std::endl;

    return 0;
}
