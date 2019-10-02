// C++ includes
#include <stdexcept>

// OpenCV
#include <opencv2/core/directx.hpp>
#include <opencv2/opencv.hpp>

class TextureCapture : public cv::VideoCapture
{
public:
    TextureCapture()
      : cv::VideoCapture(0)
	{}

	/*
	 * This function reads a new frame from the camera (blocking) and
	 * converts it to a texture.
	 */
    void getTexture(ID3D11Texture2D *texture)
    {
        if (!read(m_Frame)) {
            throw std::runtime_error("Could not read from camera");
        }
        cv::directx::convertToD3D11Texture2D(m_Frame, texture);
    }

private:
    cv::Mat m_Frame;
};

int
main()
{
    TextureCapture cap;

	// example of displaying camera on screen
    cv::Mat frame;
    do {
        cap >> frame;
        cv::imshow("camera", frame);
    } while (cv::waitKey(1) != 27); // press ESC to quit
}