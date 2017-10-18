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

static const size_t imbuffsize = 65507;
static const size_t maximsize = imbuffsize - sizeof (packinfo);