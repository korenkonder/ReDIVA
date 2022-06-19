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
};

extern wav* wav_init();
extern void wav_read(wav* w, const char* path, float_t** data, size_t* samples);
extern void wav_read(wav* w, const wchar_t* path, float_t** data, size_t* samples);
extern void wav_write(wav* w, const char* path, float_t* data, size_t samples);
extern void wav_write(wav* w, const wchar_t* path, float_t* data, size_t samples);
extern void wav_dispose(wav* w);