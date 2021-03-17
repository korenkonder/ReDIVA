/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef enum deflate_mode {
    DEFLATE_MODE_DEFLATE = 0,
    DEFLATE_MODE_GZIP    = 1,
    DEFLATE_MODE_ZLIB    = 2,
} deflate_mode;

extern size_t deflate_compress(void* src, size_t src_length, void** dst,
    size_t* dst_length, int32_t compression_level, deflate_mode mode);
extern size_t deflate_decompress(void* src, size_t src_length, void** dst,
    size_t dst_length, deflate_mode mode);
extern size_t deflate_decompress_unknown(void* src, size_t src_length, void** dst,
    size_t* dst_length, deflate_mode mode);
