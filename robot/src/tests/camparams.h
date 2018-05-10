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
    std::string m_FilePath;

    CamParams()
      : m_SizeDest(1280, 400)
    {}

    CamParams(std::string filePath)
      : m_SizeDest(1280, 400)
    {
        m_FilePath = filePath;

        YAML::Node params = YAML::LoadFile(filePath);

        YAML::Node resolution = params["resolution"];
        m_SizeSource.width = resolution[0].as<int>();
        m_SizeSource.height = resolution[1].as<int>();

        YAML::Node center = params["center"];
        m_Center.x = round(m_SizeSource.width * center[0].as<double>());
        m_Center.y = round(m_SizeSource.height * center[1].as<double>());

        YAML::Node radii = params["radius"];
        m_RadiusInner =
                round(m_SizeSource.height * radii["inner"].as<double>());
        m_RadiusOuter =
                round(m_SizeSource.height * radii["outer"].as<double>());

        m_Flipped = params["flipped"].as<bool>();
        m_DegreeOffset = params["degreeoffset"].as<int>();
    }

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
        std::vector<int> center(2);
        center.at(0) = m_Center.x;
        center.at(1) = m_Center.y;
        out << YAML::Key << "center" << YAML::Value << YAML::Flow << center;

        // radii
        out << YAML::Key << "radius" << YAML::BeginMap;
        out << YAML::Key << "inner" << m_RadiusInner;
        out << YAML::Key << "outer" << m_RadiusOuter;
        out << YAML::EndMap;

        // other
        out << YAML::Key << "flipped" << YAML::Value << m_Flipped;
        out << YAML::Key << "degreeoffset" << YAML::Value << m_DegreeOffset;

        out << YAML::EndMap;
        fd.close();
    }
};
