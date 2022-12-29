/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

struct diva {
    size_t size;
    uint32_t sample_rate;
    uint32_t samples_count;
    uint32_t loop_start;
    uint32_t loop_end;
    uint32_t channels;

    diva();
    ~diva();

    void read(const char* path);
    void read(const wchar_t* path);
    void write(const char* path);
    void write(const wchar_t* path);
};
