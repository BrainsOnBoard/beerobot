#include "imagesender.h"

#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdexcept>

namespace Net {
using namespace std::chrono;

static const double max_fps = 40;
static const long max_period = (long) (1000000000.0 / max_fps);

/* Create socket, start BeeEye (camera etc.) */
ImageSender::ImageSender(const sockaddr_in *dest)
  : m_DestAddr(dest)
  , m_Eye(get_usb())
{
    std::cout << "Starting image sender" << std::endl;

    // Create socket
    m_Fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_Fd < 0) {
        throw std::runtime_error("Cannot open socket");
    }
}

/* Close socket if needed */
ImageSender::~ImageSender()
{
    if (m_Fd != -1) {
        close(m_Fd);
    }
}

// flag to stop send process
bool ImageSender::m_Running;

/* Constantly read bee eye frames and send over UDP, until running == false */
void
ImageSender::run()
{
    // matrix to store bee eye frame
    cv::Mat view;

    // buffers for first and second UDP packet in sequence
    std::vector<uchar> buff, buff2;

    // header for packets, containing ID, number of packets in this series and
    // packet number
    packinfo info{ .id = -1 };

    // set running flag to true
    m_Running = true;
    while (m_Running) {
        auto t0 = high_resolution_clock::now();

        // read bee eye frame
        if (!m_Eye.getEyeView(view)) {
            throw new std::runtime_error("Error: Could not read from webcam");
        }

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
            buff.insert(buff.begin(), (uchar *) &info, (uchar *) (&info + 1));

            // send packet
            debug_imagepack(info, buff.size());
            if (sendto(m_Fd,
                       (buff_t *) buff.data(),
                       buff.size(),
                       MSG_NOSIGNAL,
                       (const sockaddr *) m_DestAddr,
                       sizeof(sockaddr_in)) == -1)
                std::cerr << "Error: " << strerror(errno) << std::endl;
        } else if (buff.size() <
                   2 * MAX_IM_BYTES) { // frame fits in two packets
            // there are two packets in series
            info.tot = 2;

            // insert header for first packet
            buff.insert(buff.begin(), (uchar *) &info, (uchar *) ((&info) + 1));

            // send header plus as many of the image's bytes as we can fit in
            // packet
            debug_imagepack(info, MAX_UDP_PACKET_SIZE);
            if (sendto(m_Fd,
                       (buff_t *) buff.data(),
                       MAX_UDP_PACKET_SIZE,
                       MSG_NOSIGNAL,
                       (const sockaddr *) m_DestAddr,
                       sizeof(sockaddr_in)) == -1)
                std::cerr << "Error: " << strerror(errno) << std::endl;

            // start second packet
            info.num = 1;
            buff2.clear();

            // insert header for second packet
            buff2.insert(
                    buff2.begin(), (uchar *) &info, (uchar *) ((&info) + 1));

            // insert second half of image data
            buff2.insert(buff2.end(),
                         buff.begin() + MAX_UDP_PACKET_SIZE,
                         buff.end());

            // send second packet
            debug_imagepack(info, buff2.size());
            if (sendto(m_Fd,
                       (buff_t *) buff2.data(),
                       buff2.size(),
                       MSG_NOSIGNAL,
                       (const sockaddr *) m_DestAddr,
                       sizeof(sockaddr_in)) == -1)
                std::cerr << "Error: " << strerror(errno) << std::endl;
        } else { // can't handle more than two packets in a series
            std::cerr << "Too big!" << std::endl;
        }

#ifdef DEBUG_IMAGEPACK
        std::cout << "image len: " << buff.size() << std::endl;
#endif

        // throttle framerate at max_fps
        auto t1 = high_resolution_clock::now();
        long tdiff = duration_cast<nanoseconds>(t1 - t0).count();
        if (tdiff < max_period) {
            timespec ts{ .tv_sec = 0, .tv_nsec = max_period - tdiff };

            nanosleep(&ts, nullptr);
        }
    }
}

void
ImageSender::startSending(const sockaddr_in *destAddress)
{
    // create new image sender and run in loop
    ImageSender sender(destAddress);
    sender.run();
}
}
