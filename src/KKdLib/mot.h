/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef struct mot_struct {
    float_t time;
    int32_t flags;
    int32_t frame;
    int32_t padding_0C;
    int32_t pv_branch;
    int32_t id;
    int32_t value;
    int32_t padding_1C;
} mot_struct;

typedef struct mot {
    bool ready;
    size_t length;
    mot_struct* data;
} mot;

extern mot* mot_init();
extern void mot_read_mot(mot* m, char* path);
extern void mot_wread_mot(mot* m, wchar_t* path);
extern void mot_write_mot(mot* m, char* path);
extern void mot_wwrite_mot(mot* m, wchar_t* path);
extern void mot_read_mp(mot* m, char* path, bool json);
extern void mot_wread_mp(mot* m, wchar_t* path, bool json);
extern void mot_write_mp(mot* m, char* path, bool json);
extern void mot_wwrite_mp(mot* m, wchar_t* path, bool json);
extern void mot_dispose(mot* m);
