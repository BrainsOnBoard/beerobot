#include <iostream>
#include "camparams.h"

int
main(int argc, char **argv)
{
    CamParams params("yaml_read_test.yaml");
    std::cout << "Size: " << params.m_SizeSource << std::endl
              << "Centre: " << params.m_Center << std::endl
              << "Inner radius: " << params.m_RadiusInner << std::endl
              << "Outer radius: " << params.m_RadiusOuter << std::endl
              << "Flipped: " << params.m_Flipped << std::endl
              << "Offset: " << params.m_DegreeOffset << " deg" << std::endl;

    return 0;
}
