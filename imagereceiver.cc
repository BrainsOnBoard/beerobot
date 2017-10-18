#include "imagereceiver.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

using namespace std;
using namespace cv;

ImageReceiver::ImageReceiver()
{
    if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        goto error;

    memset(&serv_addr, 0, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(IMAGE_PORT);

    if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)))
        goto error;

    cout << "Listening for images on port " << IMAGE_PORT << endl;

    return;

error:
    cerr << "Error (" << errno << "): Could not bind to port " << IMAGE_PORT << endl;
    exit(1);
}

ImageReceiver::~ImageReceiver()
{
    if (listenfd != -1)
        close(listenfd);
}

void ImageReceiver::read(Mat &view)
{
    for (;;) {
        int len = recvfrom(listenfd, buff, MAX_UDP_PACKET_SIZE, 0, NULL, NULL);
        if (len == -1) {
            cerr << "Error: " << strerror(errno) << endl;
            continue;
        }

        packinfo *info = (packinfo*) buff;
        /*cout << "id: " << info->id << endl
                << "num: " << (int) info->num << endl
                << "tot: " << (int) info->tot << endl;*/

        if (info->tot == 1) {
            vector<uchar> v(&buff[sizeof (packinfo)], &buff[len]);
            imdecode(v, IMREAD_UNCHANGED, &view);
            break;
        } else if (lastbuff.size() > 0 && info->id == lastinfo.id) {
            if (info->num != 1) {
                cerr << "Warning: packet out of sequence" << endl;
                continue;
            }
            lastbuff.insert(lastbuff.end(), &buff[sizeof (packinfo)], &buff[len]);
            imdecode(lastbuff, IMREAD_UNCHANGED, &view);
            lastbuff.clear();
            break;
        } else {
            if (info->num != 0) {
                cerr << "Warning: packet out of sequence" << endl;
                continue;
            }
            memcpy(&lastinfo, info, sizeof (packinfo));
            lastbuff.clear();
            lastbuff.insert(lastbuff.begin(), &buff[sizeof (packinfo)], &buff[len]);
        }
    }
}