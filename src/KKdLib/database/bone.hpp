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

enum BONE_KIND {
    BONE_KIND_NONE = -1,
    BONE_KIND_CMN = 0,
    BONE_KIND_MIK = 1,
    BONE_KIND_KAI = 2,
    BONE_KIND_LEN = 3,
    BONE_KIND_LUK = 4,
    BONE_KIND_MEI = 5,
    BONE_KIND_RIN = 6,
    BONE_KIND_HAK = 7,
    BONE_KIND_NER = 8,
    BONE_KIND_SAK = 9,
    BONE_KIND_TET = 10,
    BONE_KIND_MAX = 11,
};

enum IH_TYPE {
    IHT_NO  = 0x00,
    IHT_MAT = 0x01,
    IHT_ANG = 0x02,
};

enum IK_TYPE {
    IKT_0    = 0x00,
    IKT_0N   = 0x01,
    IKT_0T   = 0x02,
    IKT_ROOT = 0x03,
    IKT_1    = 0x04,
    IKT_2    = 0x05,
    IKT_2R   = 0x06,
};

struct CHAINPOSRADIUS {
    vec3 chain_pos;
};

struct BODYTYPE {
    IK_TYPE ik_type;
    IH_TYPE inherit_type;
    uint8_t inherit_mat_id;
    uint8_t up_vector_id;
    uint8_t flip_block_id;
    uint8_t expression_id;
    std::string name;

    BODYTYPE();
    ~BODYTYPE();
};

struct BoneData {
    std::vector<BODYTYPE> body_type;
    std::vector<CHAINPOSRADIUS> chain_pos_rad;
    float_t heel_height;
    std::vector<std::string> bone_name;
    std::vector<std::string> bone_node_name;
    std::vector<uint16_t> parent_node;
    std::string name;
    prj::vector_pair<uint32_t, const BODYTYPE*> body_type_map;
    prj::vector_pair<uint32_t, const std::string*> bone_name_map;
    prj::vector_pair<uint32_t, const std::string*> bone_node_name_map;

    BoneData();
    ~BoneData();
};

struct bone_database {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<BoneData> bonedata;
    prj::vector_pair<uint32_t, const BoneData*> bonedata_map;

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

    const BoneData* get_bone_data(const char* kind_name) const;
    int32_t get_block_index(const char* kind_name, const char* name) const;
    const std::vector<BODYTYPE>* get_body_type(const char* kind_name) const;
    const std::vector<CHAINPOSRADIUS>* get_chain_pos_rad(const char* kind_name) const;
    int32_t get_bone_index(const char* kind_name, const char* name) const;
    const std::vector<std::string>* get_bone_name(const char* kind_name) const;
    int32_t get_bone_node_index(const char* kind_name, const char* name) const;
    const std::vector<std::string>* get_bone_node_name(const char* kind_name) const;
    const std::vector<uint16_t>* get_parent_node(const char* kind_name) const;
    const float_t* get_heel_height(const char* kind_name) const;

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);

    const BoneData* get_bone_data(BONE_KIND kind) const;
    int32_t get_block_index(BONE_KIND kind, const char* name) const;
    const std::vector<BODYTYPE>* get_body_type(BONE_KIND kind) const;
    const std::vector<CHAINPOSRADIUS>* get_chain_pos_rad(BONE_KIND kind) const;
    int32_t get_bone_index(BONE_KIND kind, const char* name) const;
    const std::vector<std::string>* get_bone_name(BONE_KIND kind) const;
    int32_t get_bone_node_index(BONE_KIND kind, const char* name) const;
    const std::vector<std::string>* get_bone_node_name(BONE_KIND kind) const;
    const std::vector<uint16_t>* get_parent_node(BONE_KIND kind) const;
    const float_t* get_heel_height(BONE_KIND kind) const;
};

extern void bone_database_bones_calculate_count(const std::vector<BODYTYPE>* bones,
    size_t& object_bone_count, size_t& motion_bone_count,
    size_t& node_count, size_t& leaf_pos, size_t& chain_pos);

extern const char* bone_database_skeleton_type_to_string(BONE_KIND kind);

inline const BoneData* bone_database::get_bone_data(BONE_KIND kind) const {
    return get_bone_data(bone_database_skeleton_type_to_string(kind));
}

inline int32_t bone_database::get_block_index(BONE_KIND kind, const char* name) const {
    return get_block_index(bone_database_skeleton_type_to_string(kind), name);
}

inline const std::vector<BODYTYPE>* bone_database::get_body_type(BONE_KIND kind) const {
    return get_body_type(bone_database_skeleton_type_to_string(kind));
}

inline const std::vector<CHAINPOSRADIUS>* bone_database::get_chain_pos_rad(BONE_KIND kind) const {
    return get_chain_pos_rad(bone_database_skeleton_type_to_string(kind));
}

inline int32_t bone_database::get_bone_index(BONE_KIND kind, const char* name) const {
    return get_bone_index(bone_database_skeleton_type_to_string(kind), name);
}

inline const std::vector<std::string>* bone_database::get_bone_name(BONE_KIND kind) const {
    return get_bone_name(bone_database_skeleton_type_to_string(kind));
}

inline int32_t bone_database::get_bone_node_index(BONE_KIND kind, const char* name) const {
    return get_bone_node_index(bone_database_skeleton_type_to_string(kind), name);
}

inline const std::vector<std::string>* bone_database::get_bone_node_name(BONE_KIND kind) const {
    return get_bone_node_name(bone_database_skeleton_type_to_string(kind));
}

inline const std::vector<uint16_t>* bone_database::get_parent_node(BONE_KIND kind) const {
    return get_parent_node(bone_database_skeleton_type_to_string(kind));
}

inline const float_t* bone_database::get_heel_height(BONE_KIND kind) const {
    return get_heel_height(bone_database_skeleton_type_to_string(kind));
}
