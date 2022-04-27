/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"

struct diva {
    size_t size;
    uint32_t sample_rate;
    uint32_t samples_count;
    uint32_t loop_start;
    uint32_t loop_end;
    uint32_t channels;
};

extern diva* diva_init();
extern void diva_read(diva* d, const char* path);
extern void diva_read(diva* d, const wchar_t* path);
extern void diva_write(diva* d, const char* path);
extern void diva_write(diva* d, const wchar_t* path);
extern void diva_dispose(diva* d);
