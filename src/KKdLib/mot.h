/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "kf.h"
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

typedef union mot_bone_info {
    string name;
    uint16_t index;
} mot_bone_info;

typedef struct mot_key_set {
    mot_key_set_type type;
    kft2* keys;
    uint32_t keys_count;
    mot_key_set_data_type data_type;
} mot_key_set;

typedef struct mot {
    union {
        struct {
            uint16_t key_set_count : 14;
            uint16_t high_bits : 2;
        };
        uint16_t info;
    };
    uint16_t frame_count;
    int32_t bone_info_count;

    uint32_t murmurhash;
    uint16_t div_frames;
    uint8_t div_count;
    string name;
    mot_bone_info* bone_info;
    mot_key_set* key_set;
} mot;

vector(mot)

typedef struct mot_set {
    bool ready;
    bool modern;
    bool is_x;
    vector_mot vec;
} mot_set;

typedef struct mot_skeleton_rotation_reverse {
    bool x;
    bool y;
    bool z;
    vec3 rotation;
} mot_skeleton_rotation_reverse;

extern mot_set* mot_init();
extern void mot_read(mot_set* ms, char* path, bool modern);
extern void mot_wread(mot_set* ms, wchar_t* path, bool modern);
extern void mot_mread(mot_set* ms, void* data, size_t length, bool modern);
extern void mot_write(mot_set* ms, char* path);
extern void mot_wwrite(mot_set* ms, wchar_t* path);
extern void mot_mwrite(mot_set* ms, void** data, size_t* length);
extern void mot_dispose(mot_set* ms);
