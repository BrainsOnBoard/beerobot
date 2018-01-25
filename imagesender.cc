#include "imagesender.h"

#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>

using namespace std;

/* Create socket, start BeeEye (camera etc.) */
ImageSender::ImageSender(const sockaddr_in *dest)
: dest(dest), eye(get_usb())
{
    cout << "Starting image sender" << endl;

    // Create socket
    connfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (connfd < 0)
        throw runtime_error("Cannot open socket");
}

/* Close socket if needed */
ImageSender::~ImageSender()
{
    if (connfd != -1)
        close(connfd);
}

// flag to stop send process
bool ImageSender::running;

/* Constantly read bee eye frames and send over UDP, until running == false */
void ImageSender::run()
{
    // matrix to store bee eye frame
    Mat view;

    // buffers for first and second UDP packet in sequence
    vector<uchar> buff, buff2;

    // header for packets, containing ID, number of packets in this series and
    // packet number
    packinfo info{
        .id = -1};

    // set running flag to true
    running = true;
    while (running) {
        // read bee eye frame
        if (!eye.get_eye_view(view))
            throw new runtime_error("Error: Could not read from webcam");

        // convert image to JPEG; store in buff
        imencode(".jpg", view, buff);

        // increment ID number
        info.id++;

        // this is the first packet
        info.num = 0;

        if (buff.size() < MAX_IM_BYTES) { // whole frame fits in one packet
            // there is one packet in series
            info.tot = 1;

            // insert header into buffer
            buff.insert(buff.begin(), (uchar*) & info, (uchar*) (& info + 1));

            // send packet
            debug_imagepack(info, buff.size());
            if (sendto(connfd, buff.data(), buff.size(), MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1)
                cerr << "Error: " << strerror(errno) << endl;
        } else if (buff.size() < 2 * MAX_IM_BYTES) { // frame fits in two packets
            // there are two packets in series
            info.tot = 2;

            // insert header for first packet
            buff.insert(buff.begin(), (uchar*) & info, (uchar*) ((&info) + 1));

            // send header plus as many of the image's bytes as we can fit in packet
            debug_imagepack(info, MAX_UDP_PACKET_SIZE);
            if (sendto(connfd, buff.data(), MAX_UDP_PACKET_SIZE, MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1)
                cerr << "Error: " << strerror(errno) << endl;

            // start second packet
            info.num = 1;
            buff2.clear();

            // insert header for second packet
            buff2.insert(buff2.begin(), (uchar*) & info, (uchar*) ((&info) + 1));

            // insert second half of image data
            buff2.insert(buff2.end(), buff.begin() + MAX_UDP_PACKET_SIZE, buff.end());

            // send second packet
            debug_imagepack(info, buff2.size());
            if (sendto(connfd, buff2.data(), buff2.size(), MSG_NOSIGNAL, (const sockaddr*) dest, sizeof (sockaddr_in)) == -1)
                cerr << "Error: " << strerror(errno) << endl;
        } else // can't handle more than two packets in a series
            cerr << "Too big!" << endl;

#ifdef DEBUG_IMAGEPACK
        cout << "image len: " << buff.size() << endl;
#endif
    }
}

// the currently running instance of ImageSender
ImageSender * ImageSender::Instance = NULL;

void* ImageSender::start_sending(void *destAddress)
{
    // if there is an ImageSender already running, stop it
    if (Instance)
        delete Instance;

    // create new image sender and run in loop
    Instance = new ImageSender((const sockaddr_in*) destAddress);
    Instance->run();
}
