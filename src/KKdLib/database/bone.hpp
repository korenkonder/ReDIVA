/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"
#include "../prj/vector_pair.hpp"
#include "../vec.hpp"

enum bone_database_bone_type {
    BONE_DATABASE_BONE_ROTATION          = 0x00,
    BONE_DATABASE_BONE_TYPE_1            = 0x01,
    BONE_DATABASE_BONE_POSITION          = 0x02,
    BONE_DATABASE_BONE_POSITION_ROTATION = 0x03,
    BONE_DATABASE_BONE_HEAD_IK_ROTATION  = 0x04,
    BONE_DATABASE_BONE_ARM_IK_ROTATION   = 0x05,
    BONE_DATABASE_BONE_LEGS_IK_ROTATION  = 0x06,
};

enum bone_database_skeleton_type {
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
};

struct bone_database_bone {
    bone_database_bone_type type;
    bool has_parent;
    uint8_t parent;
    uint8_t pole_target;
    uint8_t mirror;
    uint8_t flags;
    std::string name;

    bone_database_bone();
    ~bone_database_bone();
};

struct bone_database_skeleton {
    std::vector<bone_database_bone> bone;
    std::vector<vec3> position;
    float_t heel_height;
    std::vector<std::string> object_bone;
    std::vector<std::string> motion_bone;
    std::vector<uint16_t> parent_index;
    std::string name;
    prj::vector_pair<uint32_t, const bone_database_bone*> bone_names;
    prj::vector_pair<uint32_t, const std::string*> object_bone_names;
    prj::vector_pair<uint32_t, const std::string*> motion_bone_names;

    bone_database_skeleton();
    ~bone_database_skeleton();
};

struct bone_database {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<bone_database_skeleton> skeleton;
    prj::vector_pair<uint32_t, const bone_database_skeleton*> skeleton_names;

    bone_database();
    ~bone_database();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    void clear();
    void update();

    const bone_database_skeleton* get_skeleton(const char* name) const;
    int32_t get_skeleton_bone_index(const char* name, const char* bone_name) const;
    const std::vector<bone_database_bone>* get_skeleton_bones(const char* name) const;
    const std::vector<vec3>* get_skeleton_positions(const char* name) const;
    int32_t get_skeleton_object_bone_index(const char* name, const char* bone_name) const;
    const std::vector<std::string>* get_skeleton_object_bones(const char* name) const;
    int32_t get_skeleton_motion_bone_index(const char* name, const char* bone_name) const;
    const std::vector<std::string>* get_skeleton_motion_bones(const char* name) const;
    const std::vector<uint16_t>* get_skeleton_parent_indices(const char* name) const;
    const float_t* get_skeleton_heel_height(const char* name) const;

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};

extern void bone_database_bones_calculate_count(const std::vector<bone_database_bone>* bones,
    size_t* object_bone_count, size_t* motion_bone_count,
    size_t* total_bone_count, size_t* ik_bone_count, size_t* chain_pos);

extern const char* bone_database_skeleton_type_to_string(bone_database_skeleton_type type);
