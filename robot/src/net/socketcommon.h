#pragma once

#include <iostream>
#include "os/net.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif

static const size_t MAIN_BUFFSIZE = 512;
static const int MAIN_PORT = 2000;

int readline(socket_t connfd, char* buff);
bool send(socket_t connfd, const char* msg, int len);
