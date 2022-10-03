/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"

struct pv_exp_data {
    float_t frame;
    int16_t type;
    int16_t id;
    float_t value;
    float_t trans;
};

struct pv_exp_mot {
    pv_exp_data* face_data;
    pv_exp_data* face_cl_data;
    std::string name;

    pv_exp_mot();
    ~pv_exp_mot();
};

struct pv_exp {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<pv_exp_mot> motion_data;

    pv_exp();
    virtual ~pv_exp();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
