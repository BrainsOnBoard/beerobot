#pragma once

#include <iostream>
#include "os/net.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif

static const size_t MAIN_BUFFSIZE = 512;
static const int MAIN_PORT = 2000;

int readLine(socket_t sock, char* buff);
bool send(socket_t sock, const char* msg, int len);
