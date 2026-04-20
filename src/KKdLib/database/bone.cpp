/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "bone.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../str_utils.hpp"

struct bone_database_skeleton_header {
    int64_t offset;
    int64_t bones_offset;
    int64_t chain_pos_rad_offset;
    int64_t heel_height_offset;
    int64_t bone_name_offset;
    int64_t bone_node_offset;
    int64_t parent_node_offset;
};

static void bone_database_classic_read_inner(bone_database* bone_data, stream& s);
static void bone_database_classic_write_inner(bone_database* bone_data, stream& s);
static void bone_database_modern_read_inner(bone_database* bone_data, stream& s, uint32_t header_length);
static void bone_database_modern_write_inner(bone_database* bone_data, stream& s);
static int64_t bone_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, const char* str);
static bool bone_database_strings_push_back_check(std::vector<std::string>& vec, const char* str);

BODYTYPE::BODYTYPE() : ik_type(), inherit_type(),
inherit_mat_id(), up_vector_id(), flip_block_id(), expression_id() {

}

BODYTYPE::~BODYTYPE() {

}

BoneData::BoneData() : heel_height() {

}

BoneData::~BoneData() {

}

bone_database::bone_database() : ready(), modern(), big_endian(), is_x() {

}

bone_database::~bone_database() {

}

void bone_database::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (!path_bin)
            return;

        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, "rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc<uint8_t>(s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                bone_database_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_bon = str_utils_add(path, ".bon");
        if (!path_bon)
            return;

        if (path_check_file_exists(path_bon)) {
            f2_struct st;
            st.read(path_bon);
            if (st.header.signature == reverse_endianness_uint32_t('BONE')) {
                memory_stream s_bone;
                s_bone.open(st.data);
                s_bone.big_endian = st.header.attrib.get_big_endian();
                bone_database_modern_read_inner(this, s_bone, st.header.get_length());
            }
        }
        free_def(path_bon);
    }
}

void bone_database::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (!path_bin)
            return;

        if (path_check_file_exists(path_bin)) {
            file_stream s;
            s.open(path_bin, L"rb");
            if (s.check_not_null()) {
                uint8_t* data = force_malloc<uint8_t>(s.length);
                s.read(data, s.length);
                memory_stream s_bin;
                s_bin.open(data, s.length);
                bone_database_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_add(path, L".bon");
        if (!path_bon)
            return;

        if (path_check_file_exists(path_bon)) {
            f2_struct st;
            st.read(path_bon);
            if (st.header.signature == reverse_endianness_uint32_t('BONE')) {
                memory_stream s_bone;
                s_bone.open(st.data);
                s_bone.big_endian = st.header.attrib.get_big_endian();
                bone_database_modern_read_inner(this, s_bone, st.header.get_length());
            }
        }
        free_def(path_bon);
    }
}

void bone_database::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        bone_database_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('BONE')) {
            memory_stream s_bone;
            s_bone.open(st.data);
            s_bone.big_endian = st.header.attrib.get_big_endian();
            bone_database_modern_read_inner(this, s_bone, st.header.get_length());
        }
    }
}

void bone_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (!path_bin)
            return;

        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            bone_database_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_bon = str_utils_add(path, ".bon");
        if (!path_bon)
            return;

        file_stream s;
        s.open(path_bon, "wb");
        if (s.check_not_null())
            bone_database_modern_write_inner(this, s);
        free_def(path_bon);
    }
}

void bone_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (!path_bin)
            return;

        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            bone_database_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_add(path, L".bon");
        if (!path_bon)
            return;

        file_stream s;
        s.open(path_bon, L"wb");
        if (s.check_not_null())
            bone_database_modern_write_inner(this, s);
        free_def(path_bon);
    }
}

void bone_database::write(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        bone_database_classic_write_inner(this, s);
    else
        bone_database_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

void bone_database::clear() {
    bonedata.clear();
    bonedata.shrink_to_fit();
    bonedata_map.clear();
    bonedata_map.shrink_to_fit();
}

void bone_database::update() {
    bonedata_map.clear();

    bonedata_map.reserve(bonedata.size());

    for (BoneData& i : bonedata) {
        bonedata_map.push_back(hash_string_murmurhash(i.name), &i);

        i.body_type_map.clear();
        i.bone_name_map.clear();
        i.bone_node_name_map.clear();

        i.body_type_map.reserve(i.body_type.size());
        i.bone_name_map.reserve(i.bone_name.size());
        i.bone_node_name_map.reserve(i.bone_node_name.size());

        for (BODYTYPE& j : i.body_type)
            i.body_type_map.push_back(hash_string_murmurhash(j.name), &j);

        for (std::string& j : i.bone_name)
            i.bone_name_map.push_back(hash_string_murmurhash(j), &j);

        for (std::string& j : i.bone_node_name)
            i.bone_node_name_map.push_back(hash_string_murmurhash(j), &j);

        i.body_type_map.sort_unique();
        i.bone_name_map.sort_unique();
        i.bone_node_name_map.sort_unique();
    }

    bonedata_map.sort_unique();
}

const BoneData* bone_database::get_bone_data(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return elem->second;
    return 0;
}

int32_t bone_database::get_block_index(const char* kind_name, const char* name) const {
    if (!kind_name || !name)
        return -1;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end()) {
        const BoneData* skel = elem->second;
        auto elem = skel->body_type_map.find(hash_utf8_murmurhash(name));
        if (elem != skel->body_type_map.end())
            return (int32_t)(elem->second - skel->body_type.data());
    }
    return -1;
}

const std::vector<BODYTYPE>* bone_database::get_body_type(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return &elem->second->body_type;
    return 0;
}

const std::vector<CHAINPOSRADIUS>* bone_database::get_chain_pos_rad(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return &elem->second->chain_pos_rad;
    return 0;
}

int32_t bone_database::get_bone_index(const char* kind_name, const char* name) const {
    if (!kind_name || !name)
        return -1;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end()) {
        const BoneData* skel = elem->second;
        auto elem = skel->bone_name_map.find(hash_utf8_murmurhash(name));
        if (elem != skel->bone_name_map.end())
            return (int32_t)(elem->second - skel->bone_name.data());
    }
    return -1;
}

const std::vector<std::string>* bone_database::get_bone_name(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return &elem->second->bone_name;
    return 0;
}

int32_t bone_database::get_bone_node_index(const char* kind_name, const char* name) const {
    if (!kind_name || !name)
        return -1;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end()) {
        const BoneData* skel = elem->second;
        auto elem = skel->bone_node_name_map.find(hash_utf8_murmurhash(name));
        if (elem != skel->bone_node_name_map.end())
            return (int32_t)(elem->second - skel->bone_node_name.data());
    }
    return -1;
}

const std::vector<std::string>* bone_database::get_bone_node_name(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return &elem->second->bone_node_name;
    return 0;
}

const std::vector<std::uint16_t>* bone_database::get_parent_node(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return &elem->second->parent_node;
    return 0;
}

const float_t* bone_database::get_heel_height(const char* kind_name) const {
    if (!kind_name)
        return 0;

    auto elem = bonedata_map.find(hash_utf8_murmurhash(kind_name));
    if (elem != bonedata_map.end())
        return &elem->second->heel_height;
    return 0;
}

bool bone_database::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    bone_database* bone_data = (bone_database*)data;
    bone_data->read(path.c_str(), bone_data->modern);

    return bone_data->ready;
}

void bone_database_bones_calculate_count(const std::vector<BODYTYPE>* bones,
    size_t& bone_name_num, size_t& bone_node_num,
    size_t& node_count, size_t& leaf_pos, size_t& chain_pos) {
    bone_name_num = 0;
    bone_node_num = 0;
    node_count = 0;
    leaf_pos = 0;
    chain_pos = 0;

    for (const BODYTYPE& i : *bones) {
        bone_name_num++;
        node_count++;
        if (i.ik_type >= IKT_2)
            bone_name_num += 2;
        chain_pos++;

        if (i.ik_type >= IKT_1) {
            chain_pos++;
            node_count += 2;

            if (i.ik_type >= IKT_2) {
                chain_pos++;
                node_count++;
            }
        }

        if (i.ik_type >= IKT_ROOT)
            leaf_pos++;
        bone_node_num++;
    }
}

const char* bone_database_skeleton_type_to_string(BONE_KIND kind) {
    switch (kind) {
    case BONE_KIND_CMN:
        return "CMN";
    case BONE_KIND_MIK:
        return "MIK";
    case BONE_KIND_KAI:
        return "KAI";
    case BONE_KIND_LEN:
        return "LEN";
    case BONE_KIND_LUK:
        return "LUK";
    case BONE_KIND_MEI:
        return "MEI";
    case BONE_KIND_RIN:
        return "RIN";
    case BONE_KIND_HAK:
        return "HAK";
    case BONE_KIND_NER:
        return "NER";
    case BONE_KIND_SAK:
        return "SAK";
    case BONE_KIND_TET:
        return "TET";
    default:
        return 0;
    }
}

static void bone_database_classic_read_inner(bone_database* bone_data, stream& s) {
    uint32_t signature = s.read_uint32_t();
    if (signature != 0x09102720)
        return;

    uint32_t skeleton_count = s.read_uint32_t();
    uint32_t skeleton_offsets_offset = s.read_uint32_t();
    uint32_t skeleton_name_offsets_offset = s.read_uint32_t();
    s.read(0x14);

    bone_data->bonedata.resize(skeleton_count);

    s.position_push(skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        BoneData* skel = &bone_data->bonedata[i];
        uint32_t skeleton_offset = s.read_uint32_t();

        s.position_push(skeleton_offset, SEEK_SET);
        uint32_t bones_offset = s.read_uint32_t();
        uint32_t chain_pos_rad_num = s.read_uint32_t();
        uint32_t chain_pos_rad_offset = s.read_uint32_t();
        uint32_t heel_height_offset = s.read_uint32_t();
        uint32_t bone_name_num = s.read_uint32_t();
        uint32_t bone_name_offset = s.read_uint32_t();
        uint32_t bone_node_num = s.read_uint32_t();
        uint32_t bone_node_offset = s.read_uint32_t();
        uint32_t parent_node_offset = s.read_uint32_t();
        s.read(0x14);

        uint32_t body_type_num = 0;
        s.position_push(bones_offset, SEEK_SET);
        while (true) {
            if (s.read_uint8_t() == 0xFF)
                break;

            s.read(0x0B);
            body_type_num++;
        }
        s.position_pop();

        skel->body_type.resize(body_type_num);

        BODYTYPE* body_type = skel->body_type.data();

        s.position_push(bones_offset, SEEK_SET);
        for (uint32_t j = 0; j < body_type_num; j++) {
            BODYTYPE* bone = &body_type[j];
            bone->ik_type = (IK_TYPE)s.read_uint8_t();
            bone->inherit_type = (IH_TYPE)s.read_uint8_t();
            bone->inherit_mat_id = s.read_uint8_t();
            bone->up_vector_id = s.read_uint8_t();
            bone->flip_block_id = s.read_uint8_t();
            bone->expression_id = s.read_uint8_t();
            s.read(0x02);
            bone->name = s.read_string_null_terminated_offset(s.read_uint32_t());
        }
        s.position_pop();

        skel->chain_pos_rad.resize(chain_pos_rad_num);

        CHAINPOSRADIUS* chain_pos_rad = skel->chain_pos_rad.data();

        s.position_push(chain_pos_rad_offset, SEEK_SET);
        for (uint32_t j = 0; j < chain_pos_rad_num; j++) {
            vec3& chain_pos = chain_pos_rad[j].chain_pos;
            chain_pos.x = s.read_float_t();
            chain_pos.y = s.read_float_t();
            chain_pos.z = s.read_float_t();
        }
        s.position_pop();

        s.position_push(heel_height_offset, SEEK_SET);
        skel->heel_height = s.read_float_t();
        s.position_pop();

        skel->bone_name.resize(bone_name_num);

        std::string* bone_name = skel->bone_name.data();

        s.position_push(bone_name_offset, SEEK_SET);
        for (uint32_t j = 0; j < bone_name_num; j++)
            bone_name[j] = s.read_string_null_terminated_offset(s.read_uint32_t());
        s.position_pop();

        skel->bone_node_name.resize(bone_node_num);

        std::string* bone_node_name = skel->bone_node_name.data();

        s.position_push(bone_node_offset, SEEK_SET);
        for (uint32_t j = 0; j < bone_node_num; j++)
            bone_node_name[j] = s.read_string_null_terminated_offset(s.read_uint32_t());
        s.position_pop();

        skel->parent_node.resize(bone_node_num);

        s.position_push(parent_node_offset, SEEK_SET);
        s.read(skel->parent_node.data(), bone_node_num * sizeof(uint16_t));
        s.position_pop();
        s.position_pop();
    }
    s.position_pop();

    s.position_push(skeleton_name_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++)
        bone_data->bonedata[i].name =  s.read_string_null_terminated_offset(s.read_uint32_t());
    s.position_pop();

    bone_data->ready = true;
    bone_data->modern = false;
    bone_data->big_endian = false;
    bone_data->is_x = false;

    bone_data->update();
}

static void bone_database_classic_write_inner(bone_database* bone_data, stream& s) {
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);

    uint32_t skeleton_count = (uint32_t)bone_data->bonedata.size();
    int64_t skeleton_offsets_offset = s.get_position();
    s.write(skeleton_count * 0x04ULL);

    int64_t skeleton_name_offset = s.get_position();
    for (BoneData& i : bone_data->bonedata)
        s.write_string_null_terminated(i.name);

    int64_t skeleton_name_offsets_offset = s.get_position();
    for (BoneData& i : bone_data->bonedata) {
        s.write_uint32_t((uint32_t)skeleton_name_offset);
        skeleton_name_offset += i.name.size() + 1;
    }

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;

    int64_t* skeleton_offsets = force_malloc<int64_t>(skeleton_count);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        BoneData* skel = &bone_data->bonedata[i];

        uint32_t body_type_num = (uint32_t)skel->body_type.size();
        uint32_t chain_pos_rad_num = (uint32_t)skel->chain_pos_rad.size();
        uint32_t bone_name_num = (uint32_t)skel->bone_name.size();
        uint32_t bone_node_num = (uint32_t)skel->bone_node_name.size();

        strings.reserve(body_type_num);
        string_offsets.reserve(body_type_num);

        for (BODYTYPE& j : skel->body_type)
            if (bone_database_strings_push_back_check(strings, j.name.c_str())) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j.name);
            }

        if (bone_database_strings_push_back_check(strings, "End")) {
            string_offsets.push_back(s.get_position());
            s.write_utf8_string_null_terminated("End");
        }
        s.align_write(0x04);

        int64_t bones_offset = s.get_position();
        for (BODYTYPE& j : skel->body_type) {
            s.write_uint8_t((uint8_t)j.ik_type);
            s.write_uint8_t((uint8_t)j.inherit_type);
            s.write_uint8_t((uint8_t)j.inherit_mat_id);
            s.write_uint8_t((uint8_t)j.up_vector_id);
            s.write_uint8_t((uint8_t)j.flip_block_id);
            s.write_uint8_t(j.expression_id);
            s.write(0x02);
            s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.name.c_str()));
        }

        s.write_uint32_t(0xFF);
        s.write_uint32_t(0xFF);
        s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
            string_offsets, "End"));

        int64_t chain_pos_rad_offset = s.get_position();
        s.write(skel->chain_pos_rad.data(), sizeof(vec3) * chain_pos_rad_num);

        int64_t heel_height_offset = s.get_position();
        s.write_float_t(skel->heel_height);

        for (std::string& j : skel->bone_name)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j);
            }
        s.align_write(0x04);

        int64_t bone_name_offset = s.get_position();
        for (std::string& j : skel->bone_name)
            s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.c_str()));

        for (std::string& j : skel->bone_node_name)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j);
            }
        s.align_write(0x04);

        int64_t bone_node_offset = s.get_position();
        for (std::string& j : skel->bone_node_name)
            s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.c_str()));

        int64_t parent_node_offset = s.get_position();
        s.write(skel->parent_node.data(), sizeof(uint16_t) * bone_node_num);
        s.align_write(0x04);

        skeleton_offsets[skel - bone_data->bonedata.data()] = s.get_position();
        s.write_uint32_t((uint32_t)bones_offset);
        s.write_uint32_t(chain_pos_rad_num);
        s.write_uint32_t((uint32_t)chain_pos_rad_offset);
        s.write_uint32_t((uint32_t)heel_height_offset);
        s.write_uint32_t(bone_name_num);
        s.write_uint32_t((uint32_t)bone_name_offset);
        s.write_uint32_t(bone_node_num);
        s.write_uint32_t((uint32_t)bone_node_offset);
        s.write_uint32_t((uint32_t)parent_node_offset);
        s.write(0x14);

        strings.clear();
        string_offsets.clear();
    }

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(0x09102720);
    s.write_uint32_t(skeleton_count);
    s.write_uint32_t((uint32_t)skeleton_offsets_offset);
    s.write_uint32_t((uint32_t)skeleton_name_offsets_offset);
    s.write(0x14);
    s.position_pop();

    s.position_push(skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++)
        s.write_uint32_t((uint32_t)skeleton_offsets[i]);
    s.position_pop();
    free_def(skeleton_offsets);
}

static void bone_database_modern_read_inner(bone_database* bone_data, stream& s, uint32_t header_length) {
    uint32_t signature = s.read_uint32_t_reverse_endianness();
    if (signature != 0x09102720)
        return;

    bool big_endian = s.big_endian;
    bool is_x = true;

    s.set_position(0x0C, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x04, SEEK_SET);

    uint32_t skeleton_count = s.read_uint32_t_reverse_endianness();
    int64_t skeleton_offsets_offset = s.read_offset(header_length, is_x);
    int64_t skeleton_name_offsets_offset = s.read_offset(header_length, is_x);

    bone_data->bonedata.resize(skeleton_count);

    s.position_push(skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        BoneData* skel = &bone_data->bonedata[i];
        int64_t skeleton_offset = s.read_offset(header_length, is_x);

        s.position_push(skeleton_offset, SEEK_SET);
        int64_t bones_offset;
        uint32_t chain_pos_rad_num;
        int64_t chain_pos_rad_offset;
        int64_t heel_height_offset;
        uint32_t bone_name_num;
        int64_t bone_name_offset;
        uint32_t bone_node_num;
        int64_t bone_node_offset;
        int64_t parent_node_offset;
        if (!is_x) {
            bones_offset = s.read_offset_f2(header_length);
            chain_pos_rad_num = s.read_uint32_t_reverse_endianness();
            chain_pos_rad_offset = s.read_offset_f2(header_length);
            heel_height_offset = s.read_offset_f2(header_length);
            bone_name_num = s.read_uint32_t_reverse_endianness();
            bone_name_offset = s.read_offset_f2(header_length);
            bone_node_num = s.read_uint32_t_reverse_endianness();
            bone_node_offset = s.read_offset_f2(header_length);
            parent_node_offset = s.read_offset_f2(header_length);
            s.read(0x14);
        }
        else {
            bones_offset = s.read_offset_x();
            chain_pos_rad_num = s.read_uint32_t_reverse_endianness();
            chain_pos_rad_offset = s.read_offset_x();
            heel_height_offset = s.read_offset_x();
            bone_name_num = s.read_uint32_t_reverse_endianness();
            bone_name_offset = s.read_offset_x();
            bone_node_num = s.read_uint32_t_reverse_endianness();
            bone_node_offset = s.read_offset_x();
            parent_node_offset = s.read_offset_x();
            s.read(0x28);
        }

        uint32_t body_type_num = 0;
        s.position_push(bones_offset, SEEK_SET);
        if (!is_x)
            while (true) {
                if (s.read_uint8_t() == 0xFF)
                    break;

                s.read(0x0B);
                body_type_num++;
            }
        else
            while (true) {
                if (s.read_uint8_t() == 0xFF)
                    break;

                s.read(0x0F);
                body_type_num++;
            }
        s.position_pop();

        skel->body_type.resize(body_type_num);

        BODYTYPE* body_type = skel->body_type.data();

        s.position_push(bones_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < body_type_num; j++) {
                BODYTYPE* bone = &body_type[j];
                bone->ik_type = (IK_TYPE)s.read_uint8_t();
                bone->inherit_type = (IH_TYPE)s.read_uint8_t();
                bone->inherit_mat_id = s.read_uint8_t();
                bone->up_vector_id = s.read_uint8_t();
                bone->flip_block_id = s.read_uint8_t();
                bone->expression_id = s.read_uint8_t();
                s.read(0x02);
                bone->name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            }
        else
            for (uint32_t j = 0; j < body_type_num; j++) {
                BODYTYPE* bone = &body_type[j];
                bone->ik_type = (IK_TYPE)s.read_uint8_t();
                bone->inherit_type = (IH_TYPE)s.read_uint8_t();
                bone->inherit_mat_id = s.read_uint8_t();
                bone->up_vector_id = s.read_uint8_t();
                bone->flip_block_id = s.read_uint8_t();
                bone->expression_id = s.read_uint8_t();
                s.read(0x02);
                bone->name = s.read_string_null_terminated_offset(s.read_offset_x());
            }
        s.position_pop();

        skel->chain_pos_rad.resize(chain_pos_rad_num);

        CHAINPOSRADIUS* chain_pos_rad = skel->chain_pos_rad.data();

        s.position_push(chain_pos_rad_offset, SEEK_SET);
        for (uint32_t j = 0; j < chain_pos_rad_num; j++) {
            vec3& chain_pos = chain_pos_rad[j].chain_pos;
            chain_pos.x = s.read_float_t_reverse_endianness();
            chain_pos.y = s.read_float_t_reverse_endianness();
            chain_pos.z = s.read_float_t_reverse_endianness();
        }
        s.position_pop();

        s.position_push(heel_height_offset, SEEK_SET);
        skel->heel_height = s.read_float_t_reverse_endianness();
        s.position_pop();

        skel->bone_name.resize(bone_name_num);

        std::string* bone_name = skel->bone_name.data();

        s.position_push(bone_name_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < bone_name_num; j++)
                bone_name[j] = s.read_string_null_terminated_offset(
                    s.read_offset_f2(header_length));
        else
            for (uint32_t j = 0; j < bone_name_num; j++)
                bone_name[j] = s.read_string_null_terminated_offset(
                    s.read_offset_x());
        s.position_pop();

        skel->bone_node_name.resize(bone_node_num);

        std::string* bone_node_name = skel->bone_node_name.data();

        s.position_push(bone_node_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < bone_node_num; j++)
                bone_node_name[j] = s.read_string_null_terminated_offset(
                    s.read_offset_f2(header_length));
        else
            for (uint32_t j = 0; j < bone_node_num; j++)
                bone_node_name[j] = s.read_string_null_terminated_offset(
                    s.read_offset_x());
        s.position_pop();

        skel->parent_node.resize(bone_node_num);

        s.position_push(parent_node_offset, SEEK_SET);
        s.read(skel->parent_node.data(), bone_node_num * sizeof(uint16_t));
        if (big_endian) {
            uint16_t* parent_node = skel->parent_node.data();
            for (uint32_t j = 0; j < bone_node_num; j++)
                parent_node[j] = reverse_endianness_uint16_t(parent_node[j]);
        }
        s.position_pop();
        s.position_pop();
    }
    s.position_pop();

    s.position_push(skeleton_name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++)
            bone_data->bonedata[i].name = s.read_string_null_terminated_offset(
                s.read_offset_f2(header_length));
    else
        for (uint32_t i = 0; i < skeleton_count; i++)
            bone_data->bonedata[i].name = s.read_string_null_terminated_offset(
                s.read_offset_x());
    s.position_pop();

    bone_data->ready = true;
    bone_data->modern = true;
    bone_data->big_endian = big_endian;
    bone_data->is_x = is_x;

    bone_data->update();
}

static void bone_database_modern_write_inner(bone_database* bone_data, stream& s) {
    bool big_endian = bone_data->big_endian;
    bool is_x = bone_data->is_x;

    memory_stream s_bone;
    s_bone.open();
    s_bone.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    uint32_t skeleton_count = (uint32_t)bone_data->bonedata.size();

    if (!is_x) {
        uint32_t pos;
        ee = { 0, 1, 16, 1 };
        ee.append(0, 4, ENRS_DWORD);
        e.vec.push_back(ee);
        pos = off = 16;

        skeleton_count *= 2;
        ee = { off, 1, (uint32_t)(skeleton_count * 4ULL), 1 };
        ee.append(0, skeleton_count, ENRS_DWORD);
        e.vec.push_back(ee);
        pos += off = (uint32_t)(skeleton_count * 4ULL);
        skeleton_count /= 2;

        for (uint32_t i = 0; i < skeleton_count; i++) {
            BoneData* skel = &bone_data->bonedata[i];

            uint32_t body_type_num = (uint32_t)skel->body_type.size();
            uint32_t chain_pos_rad_num = (uint32_t)skel->chain_pos_rad.size();
            uint32_t bone_name_num = (uint32_t)skel->bone_name.size();
            uint32_t bone_node_num = (uint32_t)skel->bone_node_name.size();

            ee = { off, 1, 56, 1 };
            ee.append(0, 14, ENRS_DWORD);
            e.vec.push_back(ee);
            pos += off = 56;

            body_type_num++;
            off += 8;
            pos += 8;
            ee = { off, 1, 12, body_type_num };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(body_type_num * 12ULL);
            if (pos + off % 0x10)
                off -= 8;
            pos += off = align_val(pos + off, 0x10) - pos;
            body_type_num--;

            ee = { off, 1, 12, chain_pos_rad_num };
            ee.append(0, 3, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(chain_pos_rad_num * 12ULL);
            pos += off = align_val(off, 0x10);

            ee = { off, 1, 4, 1 };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            pos += off = 4;

            bone_name_num += bone_node_num;
            ee = { off, 1, (uint32_t)(bone_name_num * 4ULL), 1 };
            ee.append(0, bone_name_num, ENRS_DWORD);
            e.vec.push_back(ee);
            pos += off = (uint32_t)(bone_name_num * 4ULL);
            bone_name_num -= bone_node_num;

            ee = { off, 1, (uint32_t)(bone_node_num * 2ULL), 1 };
            ee.append(0, bone_node_num, ENRS_WORD);
            e.vec.push_back(ee);
            off = (uint32_t)(bone_node_num * 2ULL);
            pos += off = align_val(off, 0x04);
        }
    }
    else {
        ee = { 0, 2, 24, 1 };
        ee.append(0, 2, ENRS_DWORD);
        ee.append(0, 2, ENRS_QWORD);
        e.vec.push_back(ee);
        off = 24;
        off = align_val(off, 0x10);

        if (skeleton_count % 2) {
            ee = { off, 1, (uint32_t)(skeleton_count * 8ULL), 1 };
            ee.append(0, skeleton_count, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, (uint32_t)(skeleton_count * 8ULL), 1 };
            ee.append(0, skeleton_count, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);
        }
        else {
            skeleton_count *= 2;
            ee = { off, 1, (uint32_t)(skeleton_count * 8ULL), 1 };
            ee.append(0, skeleton_count, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);
            skeleton_count /= 2;
        }

        ee = { off, 7, 112, skeleton_count };
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 2, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, 7, ENRS_QWORD);
        e.vec.push_back(ee);
        off = (uint32_t)(skeleton_count * 112ULL);
        off = align_val(off, 0x10);

        for (uint32_t i = 0; i < skeleton_count; i++) {
            BoneData* skel = &bone_data->bonedata[i];

            uint32_t body_type_num = (uint32_t)skel->body_type.size();
            uint32_t chain_pos_rad_num = (uint32_t)skel->chain_pos_rad.size();
            uint32_t bone_name_num = (uint32_t)skel->bone_name.size();
            uint32_t bone_node_num = (uint32_t)skel->bone_node_name.size();

            body_type_num++;
            off += 8;
            ee = { off, 1, 16, body_type_num };
            ee.append(0, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(body_type_num * 16ULL);
            body_type_num--;

            ee = { off, 1, 12, chain_pos_rad_num };
            ee.append(0, 3, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(chain_pos_rad_num * 12ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 4, 1 };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = 4;
            off = align_val(off, 0x10);

            if (bone_name_num % 1) {
                ee = { off, 1, 12, chain_pos_rad_num };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(chain_pos_rad_num * 12ULL);
                off = align_val(off, 0x10);
            }

            if (skeleton_count % 2) {
                ee = { off, 1, (uint32_t)(bone_name_num * 8ULL), 1 };
                ee.append(0, bone_name_num, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(bone_name_num * 8ULL);
                off = align_val(off, 0x10);

                ee = { off, 1, (uint32_t)(bone_node_num * 8ULL), 1 };
                ee.append(0, bone_node_num, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(bone_node_num * 8ULL);
                off = align_val(off, 0x10);
            }
            else {
                bone_name_num += bone_node_num;
                ee = { off, 1, (uint32_t)(bone_name_num * 8ULL), 1 };
                ee.append(0, bone_name_num, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(bone_name_num * 8ULL);
                off = align_val(off, 0x10);
                bone_name_num -= bone_node_num;
            }

            ee = { off, 1, (uint32_t)(bone_node_num * 2ULL), 1 };
            ee.append(0, bone_node_num, ENRS_WORD);
            e.vec.push_back(ee);
            off = (uint32_t)(bone_node_num * 2ULL);
            off = align_val(off, 0x10);
        }
    }

    s_bone.write_int32_t(0);
    s_bone.write_int32_t(0);
    io_write_offset_pof_add(s_bone, 0, 0x40, is_x, &pof);
    io_write_offset_pof_add(s_bone, 0, 0x40, is_x, &pof);
    s_bone.align_write(0x10);

    int64_t skeleton_offsets_offset = s_bone.get_position();
    if (!is_x)
        for (BoneData& i : bone_data->bonedata)
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
    else {
        for (BoneData& i : bone_data->bonedata)
            io_write_offset_x_pof_add(s_bone, 0, &pof);
        s_bone.align_write(0x10);
    }

    int64_t skeleton_name_offsets_offset = s_bone.get_position();
    if (!is_x)
        for (BoneData& i : bone_data->bonedata)
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
    else {
        for (BoneData& i : bone_data->bonedata)
            io_write_offset_x_pof_add(s_bone, 0, &pof);
        s_bone.align_write(0x10);
    }

    bone_database_skeleton_header* skh = force_malloc<bone_database_skeleton_header>(skeleton_count);

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            BoneData* skel = &bone_data->bonedata[i];

            uint32_t body_type_num = (uint32_t)skel->body_type.size();
            uint32_t chain_pos_rad_num = (uint32_t)skel->chain_pos_rad.size();
            uint32_t bone_name_num = (uint32_t)skel->bone_name.size();
            uint32_t bone_node_num = (uint32_t)skel->bone_node_name.size();

            skh[i].offset = s_bone.get_position();
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            s_bone.write_uint32_t(0);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            s_bone.write_uint32_t(0);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            s_bone.write_uint32_t(0);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            s_bone.write(0x14);

            skh[i].bones_offset = s_bone.get_position();
            for (BODYTYPE& j : skel->body_type) {
                s_bone.write(0x08);
                io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            }

            s_bone.write(0x08);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            s_bone.align_write(0x10);

            skh[i].chain_pos_rad_offset = s_bone.get_position();
            s_bone.write(sizeof(vec3) * chain_pos_rad_num);
            s_bone.align_write(0x10);

            skh[i].heel_height_offset = s_bone.get_position();
            s_bone.write_float_t(0.0f);

            skh[i].bone_name_offset = s_bone.get_position();
            for (std::string& j : skel->bone_name)
                io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);

            skh[i].bone_node_offset = s_bone.get_position();
            for (std::string& j : skel->bone_node_name)
                io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);

            skh[i].parent_node_offset = s_bone.get_position();
            s_bone.write(sizeof(uint16_t) * bone_node_num);
            s_bone.align_write(0x04);
        }
    else {
        for (uint32_t i = 0; i < skeleton_count; i++) {
            skh[i].offset = s_bone.get_position();
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.write_uint32_t(0);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.write_uint32_t(0);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.write_uint32_t(0);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.write(0x28);
            s_bone.align_write(0x10);
        }

        for (uint32_t i = 0; i < skeleton_count; i++) {
            BoneData* skel = &bone_data->bonedata[i];

            uint32_t body_type_num = (uint32_t)skel->body_type.size();
            uint32_t chain_pos_rad_num = (uint32_t)skel->chain_pos_rad.size();
            uint32_t bone_name_num = (uint32_t)skel->bone_name.size();
            uint32_t bone_node_num = (uint32_t)skel->bone_node_name.size();

            skh[i].bones_offset = s_bone.get_position();
            for (BODYTYPE& j : skel->body_type) {
                s_bone.write(0x08);
                io_write_offset_x_pof_add(s_bone, 0, &pof);
            }

            s_bone.write(0x08);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.align_write(0x10);

            skh[i].chain_pos_rad_offset = s_bone.get_position();
            s_bone.write(&s_bone, sizeof(vec3) * chain_pos_rad_num);
            s_bone.align_write(0x10);

            skh[i].heel_height_offset = s_bone.get_position();
            s_bone.write_float_t(0.0f);
            s_bone.align_write(0x10);

            skh[i].bone_name_offset = s_bone.get_position();
            for (std::string& j : skel->bone_name)
                io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.align_write(0x10);

            skh[i].bone_node_offset = s_bone.get_position();
            for (std::string& j : skel->bone_node_name)
                io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.align_write(0x10);

            skh[i].parent_node_offset = s_bone.get_position();
            s_bone.write(sizeof(uint16_t) * bone_node_num);
            s_bone.align_write(0x10);
        }
    }

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;

    if (is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            BoneData* skel = &bone_data->bonedata[i];

            if (bone_database_strings_push_back_check(strings, skel->name.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_utf8_string_null_terminated(skel->name.c_str());
            }
        }

    for (uint32_t i = 0; i < skeleton_count; i++) {
        BoneData* skel = &bone_data->bonedata[i];

        for (BODYTYPE& j : skel->body_type)
            if (bone_database_strings_push_back_check(strings, j.name.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_string_null_terminated(j.name);
            }

        if (bone_database_strings_push_back_check(strings, "End")) {
            string_offsets.push_back(s_bone.get_position());
            s_bone.write_utf8_string_null_terminated("End");
        }

        for (std::string& j : skel->bone_name)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_string_null_terminated(j);
            }

        for (std::string& j : skel->bone_node_name)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_string_null_terminated(j);
            }
    }

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            BoneData* skel = &bone_data->bonedata[i];

            if (bone_database_strings_push_back_check(strings, skel->name.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_utf8_string_null_terminated(skel->name.c_str());
            }
        }
    s_bone.align_write(0x10);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        BoneData* skel = &bone_data->bonedata[i];

        uint32_t body_type_num = (uint32_t)skel->body_type.size();
        uint32_t chain_pos_rad_num = (uint32_t)skel->chain_pos_rad.size();
        uint32_t bone_name_num = (uint32_t)skel->bone_name.size();
        uint32_t bone_node_num = (uint32_t)skel->bone_node_name.size();

        s_bone.position_push(skh[i].offset, SEEK_SET);
        if (!is_x) {
            s_bone.write_offset_f2(skh[i].bones_offset, 0x40);
            s_bone.write_uint32_t_reverse_endianness(chain_pos_rad_num);
            s_bone.write_offset_f2(skh[i].chain_pos_rad_offset, 0x40);
            s_bone.write_offset_f2(skh[i].heel_height_offset, 0x40);
            s_bone.write_uint32_t_reverse_endianness(bone_name_num);
            s_bone.write_offset_f2(skh[i].bone_name_offset, 0x40);
            s_bone.write_uint32_t_reverse_endianness(bone_node_num);
            s_bone.write_offset_f2(skh[i].bone_node_offset, 0x40);
            s_bone.write_offset_f2(skh[i].parent_node_offset, 0x40);
            s_bone.write(0x14);
        }
        else {
            s_bone.write_offset_x(skh[i].bones_offset);
            s_bone.write_uint32_t_reverse_endianness(chain_pos_rad_num);
            s_bone.write_offset_x(skh[i].chain_pos_rad_offset);
            s_bone.write_offset_x(skh[i].heel_height_offset);
            s_bone.write_uint32_t_reverse_endianness(bone_name_num);
            s_bone.write_offset_x(skh[i].bone_name_offset);
            s_bone.write_uint32_t_reverse_endianness(bone_node_num);
            s_bone.write_offset_x(skh[i].bone_node_offset);
            s_bone.write_offset_x(skh[i].parent_node_offset);
            s_bone.write(0x28);
        }
        s_bone.position_pop();

        s_bone.position_push(skh[i].bones_offset, SEEK_SET);
        if (!is_x)
            for (BODYTYPE& j : skel->body_type) {
                s_bone.write_uint8_t((uint8_t)j.ik_type);
                s_bone.write_uint8_t((uint8_t)j.inherit_type);
                s_bone.write_uint8_t((uint8_t)j.inherit_mat_id);
                s_bone.write_uint8_t((uint8_t)j.up_vector_id);
                s_bone.write_uint8_t((uint8_t)j.flip_block_id);
                s_bone.write_uint8_t(j.expression_id);
                s_bone.write(0x02);
                s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.name.c_str()), 0x40);
            }
        else
            for (BODYTYPE& j : skel->body_type) {
                s_bone.write_uint8_t((uint8_t)j.ik_type);
                s_bone.write_uint8_t((uint8_t)j.inherit_type);
                s_bone.write_uint8_t((uint8_t)j.inherit_mat_id);
                s_bone.write_uint8_t((uint8_t)j.up_vector_id);
                s_bone.write_uint8_t((uint8_t)j.flip_block_id);
                s_bone.write_uint8_t(j.expression_id);
                s_bone.write(&s_bone, 0x02);
                s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.name.c_str()));
            }

        if (!is_x) {
            s_bone.write_uint32_t(0xFF);
            s_bone.write_uint32_t(0xFF);
            s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                string_offsets, "End"), 0x40);
        }
        else {
            s_bone.write_uint32_t(0xFF);
            s_bone.write_uint32_t(0xFF);
            s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                string_offsets, "End"));
        }
        s_bone.position_pop();

        s_bone.position_push(skh[i].chain_pos_rad_offset, SEEK_SET);
        for (CHAINPOSRADIUS& j : skel->chain_pos_rad) {
            vec3& chain_pos = j.chain_pos;
            s_bone.write_float_t_reverse_endianness(chain_pos.x);
            s_bone.write_float_t_reverse_endianness(chain_pos.y);
            s_bone.write_float_t_reverse_endianness(chain_pos.z);
        }
        s_bone.position_pop();

        s_bone.position_push(skh[i].heel_height_offset, SEEK_SET);
        s_bone.write_float_t_reverse_endianness(skel->heel_height);
        s_bone.position_pop();

        s_bone.position_push(skh[i].bone_name_offset, SEEK_SET);
        if (!is_x)
            for (std::string& j : skel->bone_name)
                s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()), 0x40);
        else
            for (std::string& j : skel->bone_name)
                s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()));

        s_bone.position_push(skh[i].bone_node_offset, SEEK_SET);
        if (!is_x)
            for (std::string& j : skel->bone_node_name)
                s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()), 0x40);
        else
            for (std::string& j : skel->bone_node_name)
                s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()));
        s_bone.position_pop();

        s_bone.position_push(skh[i].parent_node_offset, SEEK_SET);
        for (uint16_t& j : skel->parent_node)
            s_bone.write_uint16_t_reverse_endianness(j);
        s_bone.position_pop();
    }

    s_bone.position_push(0x00, SEEK_SET);
    s_bone.write_uint32_t_reverse_endianness(0x09102720);
    s_bone.write_uint32_t_reverse_endianness(skeleton_count);
    s_bone.write_offset(skeleton_offsets_offset, 0x40, is_x);
    s_bone.write_offset(skeleton_name_offsets_offset, 0x40, is_x);
    s_bone.position_pop();

    s_bone.position_push(skeleton_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++)
            s_bone.write_offset_f2(skh[i].offset, 0x40);
    else
        for (uint32_t i = 0; i < skeleton_count; i++)
            s_bone.write_offset_x(skh[i].offset);
    s_bone.position_pop();

    s_bone.position_push(skeleton_name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (BoneData& i : bone_data->bonedata)
            s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                string_offsets, i.name.c_str()), 0x40);
    else
        for (BoneData& i : bone_data->bonedata)
            s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                string_offsets, i.name.c_str()));
    s_bone.position_pop();

    free_def(skh);

    f2_struct st;
    s_bone.align_write(0x10);
    s_bone.copy(st.data);
    s_bone.close();

    st.enrs = e;
    st.pof = pof;

    new (&st.header) f2_header('BONE');
    st.header.set_length(is_x ? F2_HEADER_DEFAULT_LENGTH : F2_HEADER_EXTENDED_LENGTH);
    st.header.attrib.set_big_endian(big_endian);

    st.write(s, true, is_x);
}

inline static int64_t bone_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min_def(len, i.size()) + 1))
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool bone_database_strings_push_back_check(std::vector<std::string>& vec, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min_def(len, i.size()) + 1))
            return false;

    vec.push_back(std::string(str, len));
    return true;
}
