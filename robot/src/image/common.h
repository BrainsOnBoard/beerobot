#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace Image {
#ifdef _MSC_VER
#pragma pack(push, 1)
struct PacketInfo
#else
struct __attribute__((packed)) PacketInfo
#endif
{
    int Id;
    uint8_t Number;
    uint8_t Count;
};

static const size_t MAX_UDP_PACKET_SIZE = 65507;
static const size_t MAX_IM_BYTES = MAX_UDP_PACKET_SIZE - sizeof(PacketInfo);
static const int IMAGE_PORT = 5555;

//#define DEBUG_IMAGEPACK

inline void
debugImagePacket(PacketInfo &info, int len)
{
#ifdef DEBUG_IMAGEPACK
    std::cout << "packet " << (int) (info.Number + 1) << "/" << (int) info.Count
              << std::endl;
    std::cout << "- id: " << info.Id << std::endl;
    std::cout << "- len: " << len << std::endl;
#endif
}

#ifdef _MSC_VER
#pragma pack(pop)
#endif
}
