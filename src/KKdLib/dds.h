/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "txp.hpp"
#include "vector.h"

struct dds {
    txp_format format;
    uint32_t width;
    uint32_t height;
    uint32_t mipmaps_count;
    bool has_cube_map;
    vector_old_ptr_void data;
};

extern dds* dds_init();
extern void dds_read(dds* d, const char* path);
extern void dds_read(dds* d, const wchar_t* path);
extern void dds_write(dds* d, const char* path);
extern void dds_write(dds* d, const wchar_t* path);
extern void dds_dispose(dds* d);
