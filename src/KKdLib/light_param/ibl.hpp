/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../default.hpp"
#include "../half_t.hpp"
#include "../mat.hpp"
#include "../vec.hpp"

struct light_param_ibl_diffuse {
    std::vector<half_t> data;
    int32_t size;
    int32_t level;

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

    vec4 lit_col[4];
    vec4 lit_dir[4];
    mat4 diff_coef[4][3];
    light_param_ibl_diffuse diffuse[2];
    light_param_ibl_specular specular[4];

    light_param_ibl();
    ~light_param_ibl();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
