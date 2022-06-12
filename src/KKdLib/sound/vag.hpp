/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"

enum vag_option {
    VAG_OPTION_VAG,
    VAG_OPTION_HEVAG_FASTEST,
    VAG_OPTION_HEVAG_FAST,
    VAG_OPTION_HEVAG_MEDIUM,
    VAG_OPTION_HEVAG_SLOW,
    VAG_OPTION_HEVAG_SLOWEST,
    VAG_OPTION_HEVAG_SLOWASHELL,
};

struct vag {
    uint32_t version;
    uint32_t sample_rate;
    uint32_t channels;
    size_t size;
};

extern vag* vag_init();
extern void vag_read(vag* v, const char* path);
extern void vag_read(vag* v, const wchar_t* path);
extern void vag_write(vag* v, const char* path, vag_option option);
extern void vag_write(vag* v, const wchar_t* path, vag_option option);
extern void vag_dispose(vag* f);
