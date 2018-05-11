#include "imagereceiver.h"
#include "os/net.h"

#include <errno.h>
#include <iostream>
#ifndef _MSC_VER
#include <unistd.h>
#endif

namespace Net {

/* Bind to port to receive UDP packets */
ImageReceiver::ImageReceiver()
{
    // create socket
    if ((m_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        goto error;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(Image::IMAGE_PORT);

    if (bind(m_Socket, (struct sockaddr *) &addr, sizeof(addr))) {
        goto error;
    }

    std::cout << "Listening for images on port " << Image::IMAGE_PORT
              << std::endl;
    return;

error:
    std::cerr << "Error (" << errno << "): Could not bind to port "
              << Image::IMAGE_PORT << std::endl;
    exit(1);
}

/* Close socket if needed */
ImageReceiver::~ImageReceiver()
{
    if (m_Socket != INVALID_SOCKET) {
        close(m_Socket);
    }
}

/* Read a single frame in. This will usually take two UDP packets' worth of
 * data. */
bool
ImageReceiver::readFrame(cv::Mat &view)
{
    // loop until we have a whole frame to copy to view
    for (;;) {
        // read UDP packet
        int len = recvfrom(
                m_Socket, m_Buffer, Image::MAX_UDP_PACKET_SIZE, 0, NULL, NULL);
        if (len == -1) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            continue;
        }

        // there is a header at the start of the packet
        Image::PacketInfo *info = (Image::PacketInfo *) m_Buffer;
        debugImagePacket(*info, len);

        if (info->Count == 1) { // whole frame in single packet
            // the data beyond the header is a JPEG
            std::vector<uchar> v(&m_Buffer[sizeof(Image::PacketInfo)],
                                 &m_Buffer[len]);

            // parse JPEG
#ifdef DEBUG_IMAGEPACK
            std::cout << "image len: " << v.size() << std::endl;
#endif
            imdecode(v, cv::IMREAD_UNCHANGED, &view);
            break;
        } else if (m_LastBuffer.size() > 0 &&
                   info->Id == m_LastId) { // split frame, second packet
            if (info->Number != 1) {
                std::cerr << "Warning: packet out of sequence" << std::endl;
                continue;
            }

            // append the second half of the JPEG to the first half
            m_LastBuffer.insert(m_LastBuffer.end(),
                                &m_Buffer[sizeof(Image::PacketInfo)],
                                &m_LastBuffer[len]);

            // parse JPEG
#ifdef DEBUG_IMAGEPACK
            std::cout << "image len: " << m_LastBuffer.size() << std::endl;
#endif
            imdecode(m_LastBuffer, cv::IMREAD_UNCHANGED, &view);
            break;
        } else { // split frame, first packet
            if (info->Number != 0) {
                std::cerr << "Warning: packet out of sequence" << std::endl;
                continue;
            }

            // save the ID of this packet so we can match it up with its partner
            m_LastId = info->Id;

            // clear the buffer
            m_LastBuffer.clear();

            // copy this packet's JPEG data to buffer
            m_LastBuffer.insert(m_LastBuffer.begin(),
                                &m_Buffer[sizeof(Image::PacketInfo)],
                                &m_Buffer[len]);
        }
    }
    return view.cols != 0;
}
}