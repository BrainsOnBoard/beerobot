#include "imagereceiver.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

using namespace std;
using namespace cv;

/* Bind to port to receive UDP packets */
ImageReceiver::ImageReceiver()
{
    // create socket
    if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        goto error;

    sockaddr_in addr;
    memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(IMAGE_PORT);

    if (bind(listenfd, (struct sockaddr*) &addr, sizeof (addr)))
        goto error;

    cout << "Listening for images on port " << IMAGE_PORT << endl;
    return;

error:
    cerr << "Error (" << errno << "): Could not bind to port " << IMAGE_PORT << endl;
    exit(1);
}

/* Close socket if needed */
ImageReceiver::~ImageReceiver()
{
    if (listenfd != -1)
        close(listenfd);
}

/* Read a single frame in. This will usually take two UDP packets' worth of data. */
bool ImageReceiver::read(Mat *view)
{
    // loop until we have a whole frame to copy to view
    for (;;) {
        // read UDP packet
        int len = recvfrom(listenfd, buff, MAX_UDP_PACKET_SIZE, 0, NULL, NULL);
        if (len == -1) {
            cerr << "Error: " << strerror(errno) << endl;
            continue;
        }

        // there is a header at the start of the packet
        packinfo *info = (packinfo*) buff;
        /*cout << "id: " << info->id << endl
                << "num: " << (int) info->num << endl
                << "tot: " << (int) info->tot << endl;*/

        if (info->tot == 1) { // whole frame in single packet
            // the data beyond the header is a JPEG
            vector<uchar> v(&buff[sizeof (packinfo)], &buff[len]);

            // parse JPEG
            imdecode(v, IMREAD_UNCHANGED, view);
            break;
        } else if (lastbuff.size() > 0 && info->id == lastid) { // split frame, second packet
            if (info->num != 1) {
                cerr << "Warning: packet out of sequence" << endl;
                continue;
            }

            // append the second half of the JPEG to the first half
            lastbuff.insert(lastbuff.end(), &buff[sizeof (packinfo)], &buff[len]);

            // parse JPEG
            imdecode(lastbuff, IMREAD_UNCHANGED, view);

            // clear the buffer
            lastbuff.clear();
            break;
        } else { // split frame, first packet
            if (info->num != 0) {
                cerr << "Warning: packet out of sequence" << endl;
                continue;
            }

            // save the ID of this packet so we can match it up with its partner
            lastid = info->id;

            // copy this packet's JPEG data to buffer
            lastbuff.insert(lastbuff.begin(), &buff[sizeof (packinfo)], &buff[len]);
        }
    }
    return true;
}