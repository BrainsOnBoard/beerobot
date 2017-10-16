#include "socketcommon.h"
#include <cstring>

using namespace std;

int readline(int connfd, char* buff)
{
    int len;
    while ((len = read(connfd, buff, buffsize)) == 0) {
        usleep(250000);
    }
    if (len == -1)
        return -1;
    if (buff[--len] != '\n')
        throw new runtime_error("Error: Received message does not end in \\n");

    buff[len] = 0;
    cout << "<<< " << buff << endl;
    return len;
}

bool send(int connfd, const char* msg, int len)
{
    bool ok = send(connfd, msg, len, MSG_NOSIGNAL) != -1;
    cout << ">>> " << msg;
    return ok;
}