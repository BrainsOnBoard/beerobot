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
}

bool ImageSender::running;

void ImageSender::run()
{
    Mat view;
    vector<uchar> buff, buff2;
    packinfo info{
        .id = -1};
    running = true;
    while (running) {
        if (!eye.get_eye_view(view))
            throw new runtime_error("Error: Could not read from webcam");

        // convert image to JPEG; store in buff
        imencode(".jpg", view, buff);

        info.id++;
        info.num = 0;
        if (buff.size() < MAX_IM_BYTES) {
            info.tot = 1;
            buff.insert(buff.begin(), (uchar*) & info, (uchar*) (& info + 1));
            if (sendto(connfd, buff.data(), buff.size(), MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1)
                cerr << "Error: " << strerror(errno) << endl;
        } else if (buff.size() < 2 * MAX_IM_BYTES) {
            info.tot = 2;
            buff.insert(buff.begin(), (uchar*) & info, (uchar*) ((&info) + 1));
            if (sendto(connfd, buff.data(), MAX_UDP_PACKET_SIZE, MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1)
                cerr << "Error: " << strerror(errno) << endl;

            info.num = 1;
            buff2.clear();
            buff2.insert(buff2.begin(), (uchar*) & info, (uchar*) ((&info) + 1));
            buff2.insert(buff2.end(), buff.begin() + MAX_UDP_PACKET_SIZE, buff.end());
            if (sendto(connfd, buff2.data(), buff2.size(), MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1)
                cerr << "Error: " << strerror(errno) << endl;
        } else
            cerr << "Too big!" << endl;
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
