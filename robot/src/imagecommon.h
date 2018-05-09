#pragma once

#include <cstdint>
#include <cstddef>
#include <iostream>

#ifdef _MSC_VER
#pragma pack(push,1)
struct packinfo
#else
struct __attribute__ ((packed)) packinfo
#endif
{
    int id;
    uint8_t num;
    uint8_t tot;
};

static const size_t MAX_UDP_PACKET_SIZE = 65507;
static const size_t MAX_IM_BYTES = MAX_UDP_PACKET_SIZE - sizeof (packinfo);
static const int IMAGE_PORT = 5555;

//#define DEBUG_IMAGEPACK

inline void debug_imagepack(packinfo &info, int len)
{
#ifdef DEBUG_IMAGEPACK
    std::cout << "packet " << (int) (info.num + 1) << "/" << (int) info.tot << endl;
    std::cout << "- id: " << info.id << endl;
    std::cout << "- len: " << len << endl;
#endif
}

#ifdef _MSC_VER
#pragma pack(pop)
#endif
