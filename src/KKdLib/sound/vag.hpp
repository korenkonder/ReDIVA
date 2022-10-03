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

    vag();
    virtual ~vag();

    void read(const char* path);
    void read(const wchar_t* path);
    void write(const char* path, vag_option option);
    void write(const wchar_t* path, vag_option option);
};
