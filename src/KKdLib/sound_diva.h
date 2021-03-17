/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef struct diva {
    size_t size;
    uint32_t sample_rate;
    uint32_t samples_count;
    uint32_t loop_start;
    uint32_t loop_end;
    uint32_t channels;
} diva;

extern diva* diva_init();
extern void diva_dispose(diva* d);
extern void diva_read(diva* d, char* path);
extern void diva_wread(diva* d, wchar_t* path);
extern void diva_write(diva* d, char* path);
extern void diva_wwrite(diva* d, wchar_t* path);
