/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef struct add_param_struct {
    float_t time;
    int32_t flags;
    int32_t frame;
    int32_t padding_0C;
    int32_t pv_branch;
    int32_t id;
    int32_t value;
    int32_t padding_1C;
} add_param_struct;

typedef struct add_param {
    bool ready;
    size_t length;
    add_param_struct* data;
} add_param;

extern add_param* add_param_init();
extern void add_param_dispose(add_param* a);
extern void add_param_read_adp(add_param* a, char* path);
extern void add_param_wread_adp(add_param* a, wchar_t* path);
extern void add_param_write_adp(add_param* a, char* path);
extern void add_param_wwrite_adp(add_param* a, wchar_t* path);
extern void add_param_read_mp(add_param* a, char* path, bool json);
extern void add_param_wread_mp(add_param* a, wchar_t* path, bool json);
extern void add_param_write_mp(add_param* a, char* path, bool json);
extern void add_param_wwrite_mp(add_param* a, wchar_t* path, bool json);
