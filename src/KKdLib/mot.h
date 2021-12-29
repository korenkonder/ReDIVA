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

typedef union mot_bone_info {
    string name;
    uint16_t index;
} mot_bone_info;

typedef struct mot_key_set_data {
    mot_key_set_type type;
    union {
        uint16_t* frames;
        int16_t* frames_modern;
    };
    float_t* values;
    uint16_t keys_count : 16;
    mot_key_set_data_type data_type : 16;
} mot_key_set_data;

typedef struct mot_data {
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
    string name;
    mot_bone_info* bone_info;
    mot_key_set_data* key_set;
} mot_data;

vector(mot_data)

typedef struct mot_set {
    bool is_x;
    string name;
    vector_mot_data vec;
} mot_set;

vector(mot_set)

typedef struct mot_set_farc {
    bool ready;
    bool modern;
    vector_mot_set vec;
} mot_set_farc;

extern void mot_init(mot_set_farc* msf);
extern void mot_read(mot_set_farc* msf, char* path, bool modern);
extern void mot_wread(mot_set_farc* msf, wchar_t* path, bool modern);
extern void mot_mread(mot_set_farc* msf, void* data, size_t length, bool modern);
extern void mot_write(mot_set_farc* msf, char* path, farc_compress_mode mode);
extern void mot_wwrite(mot_set_farc* msf, wchar_t* path, farc_compress_mode mode);
extern void mot_mwrite(mot_set_farc* msf, void** data, size_t* length, farc_compress_mode mode);
extern bool mot_load_file(void* data, char* path, char* file, uint32_t hash);
extern void mot_free(mot_set_farc* msf);

extern void mot_set_init(mot_set* ms);
extern void mot_set_free(mot_set* ms);
