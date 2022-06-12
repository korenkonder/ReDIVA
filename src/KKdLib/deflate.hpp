/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"

namespace deflate {
    enum mode {
        MODE_DEFLATE = 0,
        MODE_GZIP    = 1,
        MODE_ZLIB    = 2,
    };

    extern size_t compress(void* src, size_t src_length, void** dst,
        size_t* dst_length, int32_t compression_level, mode mode);
    extern size_t decompress(void* src, size_t src_length, void** dst,
        size_t* dst_length, mode mode);
}
