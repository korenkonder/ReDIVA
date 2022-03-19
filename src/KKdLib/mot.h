/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "farc.h"
#include "string.h"
#include "vec.h"
#include "vector.h"

typedef enum mot_key_set_type {
    MOT_KEY_SET_NONE            = 0x00,
    MOT_KEY_SET_STATIC          = 0x01,
    MOT_KEY_SET_HERMITE         = 0x02,
    MOT_KEY_SET_HERMITE_TANGENT = 0x03,
} mot_key_set_type;

typedef enum mot_key_set_data_type {
    MOT_KEY_SET_DATA_F32 = 0x00,
    MOT_KEY_SET_DATA_F16 = 0x01,
} mot_key_set_data_type;

class mot_bone_info {
public:
    std::string name;
    uint16_t index;

    mot_bone_info();
    ~mot_bone_info();
};

class mot_key_set_data {
public:
    mot_key_set_type type;
    std::vector<uint16_t> frames;
    std::vector<float_t> values;
    uint16_t keys_count : 16;
    mot_key_set_data_type data_type : 16;

    mot_key_set_data();
    ~mot_key_set_data();
};

class mot_data {
public:
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
    std::string name;
    std::vector<mot_bone_info> bone_info;
    std::vector<mot_key_set_data> key_set;

    mot_data();
    ~mot_data();
};

class mot_set {
public:
    bool is_x;

    std::string name;
    std::vector<mot_data> vec;

    mot_set();
    ~mot_set();
};

class mot_set_farc {
public:
    bool ready;
    bool modern;

    std::vector<mot_set> vec;

    mot_set_farc();
    ~mot_set_farc();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t length, bool modern);
    void write(const char* path, farc_compress_mode mode);
    void write(const wchar_t* path, farc_compress_mode mode);
    void write(void** data, size_t* length, farc_compress_mode mode);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);

};
