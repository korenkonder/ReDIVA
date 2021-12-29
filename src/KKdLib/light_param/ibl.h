/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../half_t.h"
#include "../mat.h"
#include "../vec.h"

typedef struct light_param_ibl_diffuse {
    half_t* data;
    int32_t size;
} light_param_ibl_diffuse;

typedef struct light_param_ibl_specular {
    half_t** data;
    int32_t max_level;
    int32_t size;
} light_param_ibl_specular;

typedef struct light_param_ibl {
    bool ready;

    vec4 lit_col[4];
    vec4 lit_dir[4];
    mat4 diff_coef[4][3];
    light_param_ibl_diffuse diffuse[2];
    light_param_ibl_specular specular[4];
} light_param_ibl;

extern void light_param_ibl_init(light_param_ibl* id);
extern void light_param_ibl_read(light_param_ibl* id, char* path);
extern void light_param_ibl_wread(light_param_ibl* id, wchar_t* path);
extern void light_param_ibl_mread(light_param_ibl* id, void* data, size_t length);
extern bool light_param_ibl_load_file(void* data, char* path, char* file, uint32_t hash);
extern void light_param_ibl_free(light_param_ibl* id);
