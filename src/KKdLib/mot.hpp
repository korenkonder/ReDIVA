/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "prj/shared_ptr.hpp"
#include "alloc_data.hpp"
#include "default.hpp"
#include "vec.hpp"

enum mot_key_set_type {
    MOT_KEY_SET_NONE            = 0x00,
    MOT_KEY_SET_STATIC          = 0x01,
    MOT_KEY_SET_HERMITE         = 0x02,
    MOT_KEY_SET_HERMITE_TANGENT = 0x03,
};

enum mot_key_set_data_type {
    MOT_KEY_SET_DATA_F32 = 0x00,
    MOT_KEY_SET_DATA_F16 = 0x01,
};

struct mot_bone_info {
    const char* name;
    uint16_t index;

    mot_bone_info();
};

struct mot_key_set_data {
    mot_key_set_type type;
    uint16_t* frames;
    float_t* values;
    uint16_t keys_count : 16;
    mot_key_set_data_type data_type : 16;

    mot_key_set_data();
};

struct mot_data {
    union {
        struct {
            uint16_t key_set_count : 14;
            uint16_t skeleton_select : 1;
            uint16_t high_bit : 1;
        };
        uint16_t info;
    };
    uint16_t frame_count;
    int32_t bone_info_count;

    uint32_t murmurhash;
    uint16_t div_frames;
    uint8_t div_count;
    const char* name;
    mot_bone_info* bone_info_array;
    mot_key_set_data* key_set_array;

    mot_data();
};

struct mot_set {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    const char* name;
    mot_data* mot_data;
    uint32_t mot_num;

    mot_set();

    void pack_file(void** data, size_t* size);
    void unpack_file(prj::shared_ptr<alloc_data> alloc, const void* data, size_t size, bool modern);

    static mot_key_set_type fit_keys_into_curve(std::vector<float_t>& values_src,
        prj::shared_ptr<alloc_data> alloc, uint16_t*& frames, float_t*& values, size_t& keys_count);
    static mot_key_set_type fit_keys_into_curve(std::vector<float_t>& values_src,
        std::vector<uint16_t>& frames, std::vector<float_t>& values);
};
