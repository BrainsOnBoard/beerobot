#include "imagereceiver.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

using namespace std;
using namespace cv;

ImageReceiver::ImageReceiver(int port)
{
    if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        goto error;
    /*int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) < 0) {
        goto error;
    }*/

    memset(&serv_addr, 0, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr))) {
        goto error;
    }
    cout << "Listening on port " << port << endl;

    return;

error:
    cerr << "Error (" << errno << "): Could not bind to port " << port << endl;
    exit(1);
}

ImageReceiver::~ImageReceiver()
{
    if (listenfd != -1)
        close(listenfd);
}

void ImageReceiver::read(Mat &view)
{
    int len = recvfrom(listenfd, buff, buffsize, 0, NULL, NULL);
    if (len == -1)
        cerr << "Error" << endl;

    vector<uchar> v(buff, &buff[len]);
    imdecode(v, IMREAD_UNCHANGED, &view);
}