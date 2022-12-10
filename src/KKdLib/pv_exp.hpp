/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "prj/shared_ptr.hpp"
#include "alloc_data.hpp"

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
    const char* name;

    pv_exp_mot();
    ~pv_exp_mot();
};

struct pv_exp {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    pv_exp_mot* motion_data;
    uint32_t motion_num;

    pv_exp();

    void pack_file(void** data, size_t* size);
    void unpack_file(prj::shared_ptr<alloc_data> alloc, const void* data, size_t size, bool modern);
};
