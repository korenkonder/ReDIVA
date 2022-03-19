/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "default.h"
#include "f2/enrs.h"

typedef enum txp_format {
    TXP_A8     = 0,
    TXP_RGB8   = 1,
    TXP_RGBA8  = 2,
    TXP_RGB5   = 3,
    TXP_RGB5A1 = 4,
    TXP_RGBA4  = 5,
    TXP_DXT1   = 6,
    TXP_DXT1a  = 7,
    TXP_DXT3   = 8,
    TXP_DXT5   = 9,
    TXP_ATI1   = 10,
    TXP_ATI2   = 11,
    TXP_L8     = 12,
    TXP_L8A8   = 13,
} txp_format;

class txp_mipmap {
public:
    uint32_t width;
    uint32_t height;
    txp_format format;
    uint32_t size;
    std::vector<uint8_t> data;

    txp_mipmap();
    ~txp_mipmap();
};

class txp {
public:
    bool has_cube_map;
    uint32_t array_size;
    uint32_t mipmaps_count;
    std::vector<txp_mipmap> mipmaps;

    txp();
    ~txp();

    static uint32_t get_size(txp_format format, uint32_t width, uint32_t height);
};

class txp_set {
public:
    bool ready;

    std::vector<txp> textures;

    txp_set();
    ~txp_set();

    bool pack_file(void** data, size_t* length, bool big_endian);
    bool pack_file_modern(void** data, size_t* length, bool big_endian);
    bool produce_enrs(vector_old_enrs_entry* enrs);
    bool unpack_file(void* data, bool big_endian);
    bool unpack_file_modern(void* data, size_t length);
};
