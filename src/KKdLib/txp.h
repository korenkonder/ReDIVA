/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

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

typedef struct txp_sub_data {
    uint32_t width;
    uint32_t height;
    txp_format format;
    uint32_t size;
    void* data;
} txp_sub_data;

typedef struct txp_data {
    uint32_t array_size;
    uint32_t mipmaps_count;
    txp_sub_data** data;
} txp_data;

typedef struct txp {
    uint32_t count;
    txp_data* data;
} txp;

extern txp* txp_init();
extern void txp_pack_file(txp* t, void** data, size_t* length, bool use_big_endian);
extern bool txp_unpack_file(txp* t, void* data, bool use_big_endian);
extern void txp_dispose(txp* t);
