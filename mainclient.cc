#include <iostream>
#include <cstring>

#include "mainclient.h"

using namespace std;

MainClient::MainClient(const string host, const int port)
{
    // Create socket
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0)
        throw std::runtime_error("Cannot open socket");

    // Create socket address structure
    sockaddr_in destAddress = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr =
        {.s_addr = inet_addr(host.c_str())}
    };

    // Connect socket
    if (connect(connfd, reinterpret_cast<sockaddr*> (&destAddress), sizeof (destAddress)) < 0)
        throw runtime_error("Cannot connect socket to " + host + ":" + std::to_string(port));

    printf("Opened socket\n");

    // NB: this will give info about "image server" in future
    if (readline(connfd, buff) == -1)
        throw new runtime_error(string("Error: ") + strerror(errno));
}

MainClient::~MainClient()
{
    if (connfd >= 0)
        close(connfd);
}

void MainClient::tank(float left, float right)
{
    // don't send a command if it's the same as the last one
    if (left == oldleft && right == oldright)
        return;

    int len = sprintf(buff, "TNK %g %g\n", left, right);
    if (!send(connfd, buff, len))
        throw new runtime_error(string("Could not send steering command: ") + strerror(errno));

    oldleft = left;
    oldright = right;
}
