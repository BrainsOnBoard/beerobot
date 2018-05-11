#pragma once

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>

#define MSG_NOSIGNAL 0
#define INET_ADDRSTRLEN 22
typedef int socklen_t;

typedef const char buff_t;
typedef char mybuff_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef unsigned char buff_t;
typedef unsigned char mybuff_t;

#define INVALID_SOCKET -1
#endif

#ifdef _MSC_VER
typedef SOCKET socket_t;

inline void
close(socket_t sock)
{
    closesocket(sock);
}

#pragma comment(lib, "Ws2_32.lib")
#else
typedef int socket_t;
#endif

#ifdef _WIN32
#define WSAStartup()                                                           \
{                                                                              \
        WSADATA wsaData;                                                       \
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);                     \
        if (result != NO_ERROR) {                                              \
            throw std::runtime_error("Error at WSAStartup");                   \
        }                                                                      \
}
#else
#define WSAStartup() {}
#define WSACleanup() {}
#endif
