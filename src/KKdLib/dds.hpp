/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "txp.hpp"

struct dds {
    txp_format format;
    uint32_t width;
    uint32_t height;
    uint32_t mipmaps_count;
    bool has_cube_map;
    std::vector<void*> data;

    dds();
    ~dds();

    void read(const char* path);
    void read(const wchar_t* path);
    void write(const char* path);
    void write(const wchar_t* path);

    uint32_t get_size(uint32_t mip_level = 0);
};
