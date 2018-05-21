#pragma once

// C++ includes
#include <chrono>
#include <errno.h>
#include <thread>

// POSIX includes
#ifndef _MSC_VER
#include <unistd.h>
#endif

// local includes
#include "eye/beeeye.h"
#include "image/common.h"
#include "os/net.h"

namespace Net {
class ImageSender
{
public:
    static bool m_Running;
    static void startSending(const sockaddr_in *destAddress);

    ImageSender(Video::Input &cam, const sockaddr_in *destAddress);
    ~ImageSender();

private:
    static ImageSender *Instance;

    void run();

    socket_t m_Socket = INVALID_SOCKET;
    const sockaddr_in *m_DestAddr;
    Eye::BeeEye m_Eye;
};

using namespace std::chrono;

static const double max_fps = 40;
static const long max_period = (long) (1000000000.0 / max_fps);

/* Create socket, start BeeEye (camera etc.) */
ImageSender::ImageSender(Video::Input &cam, const sockaddr_in *dest)
  : m_DestAddr(dest)
  , m_Eye(cam)
{
    std::cout << "Starting image sender" << std::endl;

    // Create socket
    m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_Socket == INVALID_SOCKET) {
        throw std::runtime_error("Cannot open socket");
    }
}

/* Close socket if needed */
ImageSender::~ImageSender()
{
    if (m_Socket != INVALID_SOCKET) {
        close(m_Socket);
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
    Image::PacketInfo info;
    info.Id = -1;

    // set running flag to true
    m_Running = true;
    while (m_Running) {
        auto t0 = high_resolution_clock::now();

        // read bee eye frame
        if (!m_Eye.getEyeView(view)) {
            throw std::runtime_error("Could not read from webcam");
        }

        // convert image to JPEG; store in buff
        imencode(".jpg", view, buff);

        // increment ID number
        info.Id++;

        // this is the first packet
        info.Number = 0;

        if (buff.size() <
            Image::MAX_IM_BYTES) { // whole frame fits in one packet
            // there is one packet in series
            info.Count = 1;

            // insert header into buffer
            buff.insert(buff.begin(), (uchar *) &info, (uchar *) (&info + 1));

            // send packet
            Image::debugImagePacket(info, buff.size());
            if (sendto(m_Socket,
                       (buff_t *) buff.data(),
                       (socklen_t) buff.size(),
                       MSG_NOSIGNAL,
                       (const sockaddr *) m_DestAddr,
                       (socklen_t) sizeof(sockaddr_in)) == -1)
                std::cerr << "Error: " << strerror(errno) << std::endl;
        } else if (buff.size() <
                   2 * Image::MAX_IM_BYTES) { // frame fits in two packets
            // there are two packets in series
            info.Count = 2;

            // insert header for first packet
            buff.insert(buff.begin(), (uchar *) &info, (uchar *) ((&info) + 1));

            // send header plus as many of the image's bytes as we can fit in
            // packet
            debugImagePacket(info, Image::MAX_UDP_PACKET_SIZE);
            if (sendto(m_Socket,
                       (buff_t *) buff.data(),
                       Image::MAX_UDP_PACKET_SIZE,
                       MSG_NOSIGNAL,
                       (const sockaddr *) m_DestAddr,
                       sizeof(sockaddr_in)) == -1)
                std::cerr << "Error: " << strerror(errno) << std::endl;

            // start second packet
            info.Number = 1;
            buff2.clear();

            // insert header for second packet
            buff2.insert(
                    buff2.begin(), (uchar *) &info, (uchar *) ((&info) + 1));

            // insert second half of image data
            buff2.insert(buff2.end(),
                         buff.begin() + Image::MAX_UDP_PACKET_SIZE,
                         buff.end());

            // send second packet
            debugImagePacket(info, buff2.size());
            if (sendto(m_Socket,
                       (buff_t *) buff2.data(),
                       (socklen_t) buff2.size(),
                       MSG_NOSIGNAL,
                       (const sockaddr *) m_DestAddr,
                       (socklen_t) sizeof(sockaddr_in)) == -1)
                std::cerr << "Error: " << strerror(errno) << std::endl;
        } else { // can't handle more than two packets in a series
            std::cerr << "Too big!" << std::endl;
        }

#ifdef DEBUG_IMAGEPACK
        std::cout << "image len: " << buff.size() << std::endl;
#endif

        // throttle framerate at max_fps
        auto t1 = high_resolution_clock::now();
        int64_t tdiff = duration_cast<nanoseconds>(t1 - t0).count();
        if (tdiff < max_period) {
            std::this_thread::sleep_for(nanoseconds(max_period - tdiff));
        }
    }
}

void
ImageSender::startSending(const sockaddr_in *destAddress)
{
    // create new image sender and run in loop
    Video::PanoramicCamera cam;
    ImageSender sender(cam, destAddress);
    sender.run();
}
}
