/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

struct wav {
    int32_t size;
    int32_t sample_rate;
    int32_t header_size;
    uint32_t channel_mask;
    bool is_supported;
    uint16_t bytes;
    uint16_t format;
    uint16_t channels;

    wav();
    ~wav();

    void read(const char* path, float_t*& data, size_t& samples);
    void read(const wchar_t* path, float_t*& data, size_t& samples);
    void write(const char* path, const float_t* data, size_t samples);
    void write(const wchar_t* path, const float_t* data, size_t samples);
};
