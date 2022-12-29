/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../default.hpp"

enum dof_flags {
    DOF_FOCUS         = 0x01,
    DOF_FOCUS_RANGE   = 0x02,
    DOF_FUZZING_RANGE = 0x04,
    DOF_RATIO         = 0x08,
    DOF_QUALITY       = 0x10,
    DOF_AUTO_FOCUS    = 0x20,
};

struct dof_data {
    dof_flags flags;
    float_t focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
    float_t quality;
    int32_t chara_id;
};

struct dof {
    bool ready;
    bool big_endian;
    bool is_x;

    uint32_t murmurhash;
    std::vector<dof_data> data;

    dof();
    ~dof();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
