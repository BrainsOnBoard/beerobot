#include "imagesender.h"

#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>

using namespace std;

ImageSender::ImageSender(const sockaddr_in *dest)
: dest(dest), eye(get_usb())
{
    cout << "Starting image sender" << endl;

    // Create socket
    connfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (connfd < 0)
        throw runtime_error("Cannot open socket");
}

ImageSender::~ImageSender()
{
    if (connfd != -1)
        close(connfd);
    delete dest;
}

void ImageSender::run()
{
    // input and final output image matrices
    Mat view;
    vector<uchar> buff;
    while (true) {
        if (!eye.get_eye_view(view))
            throw new runtime_error("Error: Could not read from webcam");

        // convert image to JPEG; store in buff
        imencode(".jpg", view, buff);

        if (sendto(connfd, buff.data(), buff.size(), MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1) {
            cerr << "Error: " << strerror(errno) << endl;
            //throw new runtime_error("err");
        }
    }
}

ImageSender * ImageSender::Instance = NULL;

void* ImageSender::start_sending(void *destAddress)
{
    if (Instance)
        delete Instance;

    Instance = new ImageSender((const sockaddr_in*) destAddress);
    Instance->run();
}
