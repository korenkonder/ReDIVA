/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

struct light_param_face {
    bool ready;

    float_t offset;
    float_t scale;
    vec3 position;
    vec3 direction;

    light_param_face();
    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t length);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* length);
    ~light_param_face();

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
