/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include <string>

struct stage_param_litproj {
    bool ready;

    std::string tex_name;

    stage_param_litproj();
    ~stage_param_litproj();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
