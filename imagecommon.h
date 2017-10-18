#pragma once

#include <cstdint>
#include <cstddef>

using namespace std;

struct __attribute__ ((packed)) packinfo
{
    int id;
    uint8_t num;
    uint8_t tot;
};

static const size_t MAX_UDP_PACKET_SIZE = 65507;
static const size_t MAX_IM_BYTES = MAX_UDP_PACKET_SIZE - sizeof (packinfo);
static const int IMAGE_PORT = 5555;