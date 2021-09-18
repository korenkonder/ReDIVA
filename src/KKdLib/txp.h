/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "f2_enrs.h"
#include "vector.h"

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

typedef struct txp_mipmap {
    uint32_t width;
    uint32_t height;
    txp_format format;
    uint32_t size;
    void* data;
} txp_mipmap;

vector(txp_mipmap)

typedef struct txp {
    bool has_cube_map;
    uint32_t array_size;
    uint32_t mipmaps_count;
    vector_txp_mipmap data;
} txp;

vector(txp)

extern void txp_copy(txp* src, txp* dst);
extern uint32_t txp_get_size(txp_format format, uint32_t width, uint32_t height);
extern void txp_free(txp* t);
extern bool tex_set_pack_file(vector_txp* t, void** data, size_t* length, bool use_big_endian);
extern bool tex_set_produce_enrs(vector_txp* t, vector_enrs_entry* enrs);
extern bool tex_set_unpack_file(vector_txp* t, void* data, bool use_big_endian);
extern void tex_set_free(vector_txp* t);
