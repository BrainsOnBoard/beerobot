#pragma once

#include <string>
#include <vector>

// Forward declarations
namespace cv
{
    class Mat;
}

class HttpClient
{
public:
    HttpClient(const std::string &address, int port);
    ~HttpClient();
    
    bool get_image(cv::Mat &imorig);
    
private:
    int m_Socket;
    bool m_HttpHeaderRead;
    std::vector<unsigned char> m_ImageData;
};