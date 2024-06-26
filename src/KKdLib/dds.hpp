/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "txp.hpp"

struct dds {
    txp_format format;
    int32_t width;
    int32_t height;
    int32_t mipmaps_count;
    bool has_cube_map;
    std::vector<void*> data;

    dds();
    ~dds();

    void read(const char* path);
    void read(const wchar_t* path);
    void write(const char* path);
    void write(const wchar_t* path);

    int32_t get_size(int32_t mip_level = 0);
};
