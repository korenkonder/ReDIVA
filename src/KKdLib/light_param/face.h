/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../vec.h"

class light_param_face {
public:
    bool ready;

    float_t offset;
    float_t scale;
    vec3 position;
    vec3 direction;

    light_param_face();
    void read(char* path);
    void read(wchar_t* path);
    void read(void* data, size_t length);
    void write(char* path);
    void write(wchar_t* path);
    void write(void** data, size_t* length);
    ~light_param_face();
};

extern bool light_param_face_load_file(void* data, char* path, char* file, uint32_t hash);
