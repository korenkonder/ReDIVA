/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../default.h"
#include "../half_t.h"
#include "../mat.h"
#include "../vec.h"

struct light_param_ibl_diffuse {
    std::vector<half_t> data;
    int32_t size;

    light_param_ibl_diffuse();
    ~light_param_ibl_diffuse();
};

struct light_param_ibl_specular {
    std::vector<std::vector<half_t>> data;
    int32_t max_level;
    int32_t size;

    light_param_ibl_specular();
    ~light_param_ibl_specular();
};

struct light_param_ibl {
    bool ready;

    vec4u lit_col[4];
    vec4u lit_dir[4];
    mat4u diff_coef[4][3];
    light_param_ibl_diffuse diffuse[2];
    light_param_ibl_specular specular[4];

    light_param_ibl();
    ~light_param_ibl();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t length);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
