#pragma once

#include <string>
#include <vector>

#include "motor.h"

// Forward declarations
namespace cv {
    class Mat;
}

class HttpClient : public Motor {
public:
    HttpClient(const std::string &address, int port);
    ~HttpClient();

    bool get_image(cv::Mat &imorig);
    virtual void tank(float left, float right);

private:
    int m_Socket;
    bool m_HttpHeaderRead;
    std::vector<unsigned char> m_ImageData;
};