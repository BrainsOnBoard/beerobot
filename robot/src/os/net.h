#pragma once

#ifdef _WIN32
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#endif
	#include <winsock2.h>

	#define MSG_NOSIGNAL 0
	#define INET_ADDRSTRLEN 22
	typedef const char buff_t;
	typedef int socklen_t;
	typedef char mybuff_t;
#else
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	typedef uchar buff_t;
	typedef uchar mybuff_t;
#endif

#ifdef _MSC_VER
	typedef SOCKET socket_t;

	inline void close(socket_t sock)
	{
		closesocket(sock);
	}
#else
	typedef int socket_t;
#endif
