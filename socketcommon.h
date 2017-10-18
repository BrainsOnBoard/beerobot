#pragma once

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

static const size_t MAIN_BUFFSIZE = 512;
static const int MAIN_PORT = 2000;

int readline(int connfd, char* buff);
bool send(int connfd, const char* msg, int len);