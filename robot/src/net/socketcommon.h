#pragma once

#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#define MSG_NOSIGNAL 0
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <unistd.h>
#include <errno.h>

static const size_t MAIN_BUFFSIZE = 512;
static const int MAIN_PORT = 2000;

int readline(int connfd, char* buff);
bool send(int connfd, const char* msg, int len);
