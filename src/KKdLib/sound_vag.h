/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef enum vag_option {
    VAG_OPTION_VAG,
    VAG_OPTION_HEVAG_FASTEST,
    VAG_OPTION_HEVAG_FAST,
    VAG_OPTION_HEVAG_MEDIUM,
    VAG_OPTION_HEVAG_SLOW,
    VAG_OPTION_HEVAG_SLOWEST,
    VAG_OPTION_HEVAG_SLOWASHELL,
} vag_option;

typedef struct vag {
    uint32_t version;
    uint32_t sample_rate;
    uint32_t channels;
    size_t size;
} vag;

extern vag* vag_init();
extern void vag_dispose(vag* f);
extern void vag_read(vag* v, char* path);
extern void vag_wread(vag* v, wchar_t* path);
extern void vag_write(vag* v, char* path, vag_option option);
extern void vag_wwrite(vag* v, wchar_t* path, vag_option option);
