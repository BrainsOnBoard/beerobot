#include <thread>
#include <cstring>
#include "socketcommon.h"

//#define SOCKET_TRACE

using namespace std;

/* Read a single line in */
int readline(socket_t connfd, char *buff)
{
	int len;
#ifdef _MSC_VER
	len = recv(connfd, buff, MAIN_BUFFSIZE, 0);
#else
	while ((len = read(connfd, buff, MAIN_BUFFSIZE)) == 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
#endif
	if (len == -1)
		return -1;
	if (buff[--len] != '\n')
		throw new runtime_error("Error: Received message does not end in \\n");

	buff[len] = 0;

#ifdef SOCKET_TRACE
	cout << "<<< " << buff << endl;
#endif

	return len;
}

/* Send a message, return false on error */
bool send(socket_t connfd, const char* msg, int len)
{
	bool ok = send(connfd, msg, len, MSG_NOSIGNAL) != -1;

#ifdef SOCKET_TRACE
	cout << ">>> " << msg;
#endif

	return ok;
}
