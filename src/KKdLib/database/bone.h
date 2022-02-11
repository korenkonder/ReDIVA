/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vec.h"
#include "../vector.h"

typedef enum bone_database_bone_type {
    BONE_DATABASE_BONE_ROTATION          = 0x00,
    BONE_DATABASE_BONE_TYPE_1            = 0x01,
    BONE_DATABASE_BONE_POSITION          = 0x02,
    BONE_DATABASE_BONE_POSITION_ROTATION = 0x03,
    BONE_DATABASE_BONE_HEAD_IK_ROTATION  = 0x04,
    BONE_DATABASE_BONE_ARM_IK_ROTATION   = 0x05,
    BONE_DATABASE_BONE_LEGS_IK_ROTATION  = 0x06,
} bone_database_bone_type;

typedef enum bone_database_skeleton_type {
    BONE_DATABASE_SKELETON_COMMON = 0,
    BONE_DATABASE_SKELETON_MIKU   = 1,
    BONE_DATABASE_SKELETON_KAITO  = 2,
    BONE_DATABASE_SKELETON_LEN    = 3,
    BONE_DATABASE_SKELETON_LUKA   = 4,
    BONE_DATABASE_SKELETON_MEIKO  = 5,
    BONE_DATABASE_SKELETON_RIN    = 6,
    BONE_DATABASE_SKELETON_HAKU   = 7,
    BONE_DATABASE_SKELETON_NERU   = 8,
    BONE_DATABASE_SKELETON_SAKINE = 9,
    BONE_DATABASE_SKELETON_TETO   = 10,
    BONE_DATABASE_SKELETON_NONE   = -1,
} bone_database_skeleton_type;

typedef struct bone_database_bone {
    bone_database_bone_type type;
    bool has_parent;
    uint8_t parent;
    uint8_t pole_target;
    uint8_t mirror;
    uint8_t flags;
    string name;
} bone_database_bone;

vector(bone_database_bone)

typedef struct bone_database_skeleton {
    vector_bone_database_bone bone;
    vector_vec3 position;
    float_t heel_height;
    vector_string object_bone;
    vector_string motion_bone;
    vector_uint16_t parent_index;
    string name;
} bone_database_skeleton;

vector(bone_database_skeleton)

typedef struct bone_database {
    bool ready;
    bool modern;
    bool is_x;

    vector_bone_database_skeleton skeleton;
} bone_database;

extern void bone_database_init(bone_database* bone_data);
extern void bone_database_read(bone_database* bone_data, char* path, bool modern);
extern void bone_database_wread(bone_database* bone_data, wchar_t* path, bool modern);
extern void bone_database_mread(bone_database* bone_data, void* data, size_t length, bool modern);
extern void bone_database_write(bone_database* bone_data, char* path);
extern void bone_database_wwrite(bone_database* bone_data, wchar_t* path);
extern void bone_database_mwrite(bone_database* bone_data, void** data, size_t* length);
extern bool bone_database_load_file(void* data, char* path, char* file, uint32_t hash);
extern void bone_database_bones_calculate_count(vector_bone_database_bone* bones, size_t* object_bone_count,
    size_t* motion_bone_count, size_t* total_bone_count, size_t* ik_bone_count, size_t* chain_pos);
extern bool bone_database_get_skeleton(bone_database* bone_data,
    char* name, bone_database_skeleton** skeleton);
extern int32_t bone_database_get_skeleton_bone_index(bone_database* bone_data, char* name, char* bone_name);
extern bool bone_database_get_skeleton_bones(bone_database* bone_data,
    char* name, vector_bone_database_bone** bone);
extern bool bone_database_get_skeleton_positions(bone_database* bone_data,
    char* name, vector_vec3** positions);
extern int32_t bone_database_get_skeleton_object_bone_index(bone_database* bone_data,
    char* name, char* bone_name);
extern bool bone_database_get_skeleton_object_bones(bone_database* bone_data,
    char* name, vector_string** object_bones);
extern int32_t bone_database_get_skeleton_motion_bone_index(bone_database* bone_data,
    char* name, char* bone_name);
extern bool bone_database_get_skeleton_motion_bones(bone_database* bone_data,
    char* name, vector_string** motion_bones);
extern bool bone_database_get_skeleton_parent_indices(bone_database* bone_data,
    char* name, vector_uint16_t** parent_indices);
extern bool bone_database_get_skeleton_heel_height(bone_database* bone_data,
    char* name, float_t** unknown_value);
extern void bone_database_free(bone_database* bone_data);

extern const char* bone_database_skeleton_type_to_string(bone_database_skeleton_type type);
extern void bone_database_bone_free(bone_database_bone* bone);
extern void bone_database_skeleton_free(bone_database_skeleton* skel);
