#pragma once

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

const size_t buffsize = 512;

int readline(int connfd, char* buff);
bool send(int connfd, const char* msg, int len);