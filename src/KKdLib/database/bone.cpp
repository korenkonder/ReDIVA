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
    int64_t positions_offset;
    int64_t unknown_value_offset;
    int64_t object_bone_names_offset;
    int64_t motion_bone_names_offset;
    int64_t parent_indices_offset;
};

static void bone_database_classic_read_inner(bone_database* bone_data, stream& s);
static void bone_database_classic_write_inner(bone_database* bone_data, stream& s);
static void bone_database_modern_read_inner(bone_database* bone_data, stream& s, uint32_t header_length);
static void bone_database_modern_write_inner(bone_database* bone_data, stream& s);
static int64_t bone_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<int64_t>& vec_off, const char* str);
static bool bone_database_strings_push_back_check(std::vector<std::string>& vec, const char* str);

bone_database_bone::bone_database_bone() : type(),
has_parent(), parent(), pole_target(), mirror(), flags() {

}

bone_database_bone::~bone_database_bone() {

}

bone_database_skeleton::bone_database_skeleton() : heel_height() {

}

bone_database_skeleton::~bone_database_skeleton() {

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
        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            bone_database_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_bon = str_utils_add(path, ".bon");
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
        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            bone_database_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_add(path, L".bon");
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
    skeleton.clear();
    skeleton.shrink_to_fit();
    skeleton_names.clear();
    skeleton_names.shrink_to_fit();
}

void bone_database::update() {
    skeleton_names.clear();

    skeleton_names.reserve(skeleton.size());

    for (bone_database_skeleton& i : skeleton) {
        skeleton_names.push_back(hash_string_murmurhash(i.name), &i);

        i.bone_names.clear();
        i.object_bone_names.clear();
        i.motion_bone_names.clear();

        i.bone_names.reserve(i.bone.size());
        i.object_bone_names.reserve(i.object_bone.size());
        i.motion_bone_names.reserve(i.motion_bone.size());

        for (bone_database_bone& j : i.bone)
            i.bone_names.push_back(hash_string_murmurhash(j.name), &j);

        for (std::string& j : i.object_bone)
            i.object_bone_names.push_back(hash_string_murmurhash(j), &j);

        for (std::string& j : i.motion_bone)
            i.motion_bone_names.push_back(hash_string_murmurhash(j), &j);

        i.bone_names.sort_unique();
        i.object_bone_names.sort_unique();
        i.motion_bone_names.sort_unique();
    }

    skeleton_names.sort_unique();
}

const bone_database_skeleton* bone_database::get_skeleton(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
        return elem->second;
    return 0;
}

int32_t bone_database::get_skeleton_bone_index(const char* name, const char* bone_name) const {
    if (!name || !bone_name)
        return -1;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end()) {
        const bone_database_skeleton* skel = elem->second;
        auto elem = skel->bone_names.find(hash_utf8_murmurhash(bone_name));
        if (elem != skel->bone_names.end())
            return (int32_t)(elem->second - skel->bone.data());
    }
    return -1;
}

const std::vector<bone_database_bone>* bone_database::get_skeleton_bones(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
        return &elem->second->bone;
    return 0;
}

const std::vector<vec3>* bone_database::get_skeleton_positions(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
        return &elem->second->position;
    return 0;
}

int32_t bone_database::get_skeleton_object_bone_index(const char* name, const char* bone_name) const {
    if (!name || !bone_name)
        return -1;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end()) {
        const bone_database_skeleton* skel = elem->second;
        auto elem = skel->object_bone_names.find(hash_utf8_murmurhash(bone_name));
        if (elem != skel->object_bone_names.end())
            return (int32_t)(elem->second - skel->object_bone.data());
    }
    return -1;
}

const std::vector<std::string>* bone_database::get_skeleton_object_bones(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
        return &elem->second->object_bone;
    return 0;
}

int32_t bone_database::get_skeleton_motion_bone_index(const char* name, const char* bone_name) const {
    if (!name || !bone_name)
        return -1;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end()) {
        const bone_database_skeleton* skel = elem->second;
        auto elem = skel->motion_bone_names.find(hash_utf8_murmurhash(bone_name));
        if (elem != skel->motion_bone_names.end())
            return (int32_t)(elem->second - skel->motion_bone.data());
    }
    return -1;
}

const std::vector<std::string>* bone_database::get_skeleton_motion_bones(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
        return &elem->second->motion_bone;
    return 0;
}

const std::vector<std::uint16_t>* bone_database::get_skeleton_parent_indices(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
        return &elem->second->parent_index;
    return 0;
}

const float_t* bone_database::get_skeleton_heel_height(const char* name) const {
    if (!name)
        return 0;

    auto elem = skeleton_names.find(hash_utf8_murmurhash(name));
    if (elem != skeleton_names.end())
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

void bone_database_bones_calculate_count(const std::vector<bone_database_bone>* bones,
    size_t* object_bone_count, size_t* motion_bone_count,
    size_t* total_bone_count, size_t* ik_bone_count, size_t* chain_pos) {
    size_t _object_bone_count = 0;
    size_t _motion_bone_count = 0;
    size_t _total_bone_count = 0;
    size_t _ik_bone_count = 0;
    size_t _chain_pos = 0;
    for (const bone_database_bone& i : *bones)
        switch (i.type) {
        case BONE_DATABASE_BONE_ROTATION:
        case BONE_DATABASE_BONE_TYPE_1:
        case BONE_DATABASE_BONE_POSITION:
            _object_bone_count++;
            _motion_bone_count++;
            _total_bone_count++;
            _chain_pos++;
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION:
            _object_bone_count++;
            _motion_bone_count++;
            _total_bone_count++;
            _ik_bone_count++;
            _chain_pos++;
            break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            _object_bone_count++;
            _motion_bone_count++;
            _total_bone_count += 3;
            _ik_bone_count++;
            _chain_pos += 2;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            _object_bone_count += 3;
            _motion_bone_count++;
            _total_bone_count += 4;
            _ik_bone_count++;
            _chain_pos += 3;
            break;
        }
    *object_bone_count = _object_bone_count;
    *motion_bone_count = _motion_bone_count;
    *total_bone_count = _total_bone_count;
    *ik_bone_count = _ik_bone_count;
    *chain_pos = _chain_pos;
}

const char* bone_database_skeleton_type_to_string(bone_database_skeleton_type type) {
    switch (type) {
    case BONE_DATABASE_SKELETON_COMMON:
        return "CMN";
    case BONE_DATABASE_SKELETON_MIKU:
        return "MIK";
    case BONE_DATABASE_SKELETON_KAITO:
        return "KAI";
    case BONE_DATABASE_SKELETON_LEN:
        return "LEN";
    case BONE_DATABASE_SKELETON_LUKA:
        return "LUK";
    case BONE_DATABASE_SKELETON_MEIKO:
        return "MEI";
    case BONE_DATABASE_SKELETON_RIN:
        return "RIN";
    case BONE_DATABASE_SKELETON_HAKU:
        return "HAK";
    case BONE_DATABASE_SKELETON_NERU:
        return "NER";
    case BONE_DATABASE_SKELETON_SAKINE:
        return "SAK";
    case BONE_DATABASE_SKELETON_TETO:
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

    bone_data->skeleton.resize(skeleton_count);

    s.position_push(skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];
        uint32_t skeleton_offset = s.read_uint32_t();

        s.position_push(skeleton_offset, SEEK_SET);
        uint32_t bones_offset = s.read_uint32_t();
        uint32_t position_count = s.read_uint32_t();
        uint32_t positions_offset = s.read_uint32_t();
        uint32_t unknown_value_offset = s.read_uint32_t();
        uint32_t object_bone_count = s.read_uint32_t();
        uint32_t object_bone_names_offset = s.read_uint32_t();
        uint32_t motion_bone_count = s.read_uint32_t();
        uint32_t motion_bone_names_offset = s.read_uint32_t();
        uint32_t parent_indices_offset = s.read_uint32_t();
        s.read(0x14);

        uint32_t bone_count = 0;
        s.position_push(bones_offset, SEEK_SET);
        while (true) {
            if (s.read_uint8_t() == 0xFF)
                break;

            s.read(0x0B);
            bone_count++;
        }
        s.position_pop();

        skel->bone.resize(bone_count);

        bone_database_bone* _bone = skel->bone.data();

        s.position_push(bones_offset, SEEK_SET);
        for (uint32_t j = 0; j < bone_count; j++) {
            bone_database_bone* bone = &_bone[j];
            bone->type = (bone_database_bone_type)s.read_uint8_t();
            bone->has_parent = s.read_uint8_t() ? true : false;
            bone->parent = s.read_uint8_t();
            bone->pole_target = s.read_uint8_t();
            bone->mirror = s.read_uint8_t();
            bone->flags = s.read_uint8_t();
            s.read(0x02);
            bone->name = s.read_string_null_terminated_offset(s.read_uint32_t());
        }
        s.position_pop();

        skel->position.resize(position_count);

        vec3* _position = skel->position.data();

        s.position_push(positions_offset, SEEK_SET);
        for (uint32_t j = 0; j < position_count; j++) {
            vec3* position = &_position[j];
            position->x = s.read_float_t();
            position->y = s.read_float_t();
            position->z = s.read_float_t();
        }
        s.position_pop();

        s.position_push(unknown_value_offset, SEEK_SET);
        skel->heel_height = s.read_float_t();
        s.position_pop();

        skel->object_bone.resize(object_bone_count);

        std::string* object_bone = skel->object_bone.data();

        s.position_push(object_bone_names_offset, SEEK_SET);
        for (uint32_t j = 0; j < object_bone_count; j++)
            object_bone[j] = s.read_string_null_terminated_offset(s.read_uint32_t());
        s.position_pop();

        skel->motion_bone.resize(motion_bone_count);

        std::string* motion_bone = skel->motion_bone.data();

        s.position_push(motion_bone_names_offset, SEEK_SET);
        for (uint32_t j = 0; j < motion_bone_count; j++)
            motion_bone[j] = s.read_string_null_terminated_offset(s.read_uint32_t());
        s.position_pop();

        skel->parent_index.resize(motion_bone_count);

        s.position_push(parent_indices_offset, SEEK_SET);
        s.read(skel->parent_index.data(), motion_bone_count * sizeof(uint16_t));
        s.position_pop();
        s.position_pop();
    }
    s.position_pop();

    s.position_push(skeleton_name_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++)
        bone_data->skeleton[i].name =  s.read_string_null_terminated_offset(s.read_uint32_t());
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

    uint32_t skeleton_count = (uint32_t)bone_data->skeleton.size();
    int64_t skeleton_offsets_offset = s.get_position();
    s.write(skeleton_count * 0x04ULL);

    int64_t skeleton_name_offset = s.get_position();
    for (bone_database_skeleton& i : bone_data->skeleton)
        s.write_string_null_terminated(i.name);

    int64_t skeleton_name_offsets_offset = s.get_position();
    for (bone_database_skeleton& i : bone_data->skeleton) {
        s.write_uint32_t((uint32_t)skeleton_name_offset);
        skeleton_name_offset += i.name.size() + 1;
    }

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;

    int64_t* skeleton_offsets = force_malloc<int64_t>(skeleton_count);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];

        uint32_t bone_count = (uint32_t)skel->bone.size();
        uint32_t position_count = (uint32_t)skel->position.size();
        uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
        uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

        strings.reserve(bone_count);
        string_offsets.reserve(bone_count);

        for (bone_database_bone& j : skel->bone)
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
        for (bone_database_bone& j : skel->bone) {
            s.write_uint8_t((uint8_t)j.type);
            s.write_uint8_t(j.has_parent ? 1 : 0);
            s.write_uint8_t(j.parent);
            s.write_uint8_t(j.pole_target);
            s.write_uint8_t(j.mirror);
            s.write_uint8_t(j.flags);
            s.write(0x02);
            s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.name.c_str()));
        }

        s.write_uint32_t(0xFF);
        s.write_uint32_t(0xFF);
        s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
            string_offsets, "End"));

        int64_t positions_offset = s.get_position();
        s.write(skel->position.data(), sizeof(vec3) * position_count);

        int64_t unknown_value_offset = s.get_position();
        s.write_float_t(skel->heel_height);

        for (std::string& j : skel->object_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j);
            }
        s.align_write(0x04);

        int64_t object_bone_names_offset = s.get_position();
        for (std::string& j : skel->object_bone)
            s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.c_str()));

        for (std::string& j : skel->motion_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s.get_position());
                s.write_string_null_terminated(j);
            }
        s.align_write(0x04);

        int64_t motion_bone_names_offset = s.get_position();
        for (std::string& j : skel->motion_bone)
            s.write_uint32_t((uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.c_str()));

        int64_t parent_indices_offset = s.get_position();
        s.write(skel->parent_index.data(), sizeof(uint16_t) * motion_bone_count);
        s.align_write(0x04);

        skeleton_offsets[skel - bone_data->skeleton.data()] = s.get_position();
        s.write_uint32_t((uint32_t)bones_offset);
        s.write_uint32_t(position_count);
        s.write_uint32_t((uint32_t)positions_offset);
        s.write_uint32_t((uint32_t)unknown_value_offset);
        s.write_uint32_t(object_bone_count);
        s.write_uint32_t((uint32_t)object_bone_names_offset);
        s.write_uint32_t(motion_bone_count);
        s.write_uint32_t((uint32_t)motion_bone_names_offset);
        s.write_uint32_t((uint32_t)parent_indices_offset);
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

    bone_data->skeleton.resize(skeleton_count);

    s.position_push(skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];
        int64_t skeleton_offset = s.read_offset(header_length, is_x);

        s.position_push(skeleton_offset, SEEK_SET);
        int64_t bones_offset;
        uint32_t position_count;
        int64_t positions_offset;
        int64_t unknown_value_offset;
        uint32_t object_bone_count;
        int64_t object_bone_names_offset;
        uint32_t motion_bone_count;
        int64_t motion_bone_names_offset;
        int64_t parent_indices_offset;
        if (!is_x) {
            bones_offset = s.read_offset_f2(header_length);
            position_count = s.read_uint32_t_reverse_endianness();
            positions_offset = s.read_offset_f2(header_length);
            unknown_value_offset = s.read_offset_f2(header_length);
            object_bone_count = s.read_uint32_t_reverse_endianness();
            object_bone_names_offset = s.read_offset_f2(header_length);
            motion_bone_count = s.read_uint32_t_reverse_endianness();
            motion_bone_names_offset = s.read_offset_f2(header_length);
            parent_indices_offset = s.read_offset_f2(header_length);
            s.read(0x14);
        }
        else {
            bones_offset = s.read_offset_x();
            position_count = s.read_uint32_t_reverse_endianness();
            positions_offset = s.read_offset_x();
            unknown_value_offset = s.read_offset_x();
            object_bone_count = s.read_uint32_t_reverse_endianness();
            object_bone_names_offset = s.read_offset_x();
            motion_bone_count = s.read_uint32_t_reverse_endianness();
            motion_bone_names_offset = s.read_offset_x();
            parent_indices_offset = s.read_offset_x();
            s.read(0x28);
        }

        uint32_t bone_count = 0;
        s.position_push(bones_offset, SEEK_SET);
        if (!is_x)
            while (true) {
                if (s.read_uint8_t() == 0xFF)
                    break;

                s.read(0x0B);
                bone_count++;
            }
        else
            while (true) {
                if (s.read_uint8_t() == 0xFF)
                    break;

                s.read(0x0F);
                bone_count++;
            }
        s.position_pop();

        skel->bone.resize(bone_count);

        bone_database_bone* _bone = skel->bone.data();

        s.position_push(bones_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < bone_count; j++) {
                bone_database_bone* bone = &_bone[j];
                bone->type = (bone_database_bone_type)s.read_uint8_t();
                bone->has_parent = s.read_uint8_t() ? true : false;
                bone->parent = s.read_uint8_t();
                bone->pole_target = s.read_uint8_t();
                bone->mirror = s.read_uint8_t();
                bone->flags = s.read_uint8_t();
                s.read(0x02);
                bone->name = s.read_string_null_terminated_offset(s.read_offset_f2(header_length));
            }
        else
            for (uint32_t j = 0; j < bone_count; j++) {
                bone_database_bone* bone = &_bone[j];
                bone->type = (bone_database_bone_type)s.read_uint8_t();
                bone->has_parent = s.read_uint8_t() ? true : false;
                bone->parent = s.read_uint8_t();
                bone->pole_target = s.read_uint8_t();
                bone->mirror = s.read_uint8_t();
                bone->flags = s.read_uint8_t();
                s.read(0x02);
                bone->name = s.read_string_null_terminated_offset(s.read_offset_x());
            }
        s.position_pop();

        skel->position.resize(position_count);

        vec3* _position = skel->position.data();

        s.position_push(positions_offset, SEEK_SET);
        for (uint32_t j = 0; j < position_count; j++) {
            vec3* position = &_position[j];
            position->x = s.read_float_t_reverse_endianness();
            position->y = s.read_float_t_reverse_endianness();
            position->z = s.read_float_t_reverse_endianness();
        }
        s.position_pop();

        s.position_push(unknown_value_offset, SEEK_SET);
        skel->heel_height = s.read_float_t_reverse_endianness();
        s.position_pop();

        skel->object_bone.resize(object_bone_count);

        std::string* object_bone = skel->object_bone.data();

        s.position_push(object_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < object_bone_count; j++)
                object_bone[j] = s.read_string_null_terminated_offset(
                    s.read_offset_f2(header_length));
        else
            for (uint32_t j = 0; j < object_bone_count; j++)
                object_bone[j] = s.read_string_null_terminated_offset(
                    s.read_offset_x());
        s.position_pop();

        skel->motion_bone.resize(motion_bone_count);

        std::string* motion_bone = skel->motion_bone.data();

        s.position_push(motion_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < motion_bone_count; j++)
                motion_bone[j] = s.read_string_null_terminated_offset(
                    s.read_offset_f2(header_length));
        else
            for (uint32_t j = 0; j < motion_bone_count; j++)
                motion_bone[j] = s.read_string_null_terminated_offset(
                    s.read_offset_x());
        s.position_pop();

        skel->parent_index.resize(motion_bone_count);

        s.position_push(parent_indices_offset, SEEK_SET);
        s.read(skel->parent_index.data(), motion_bone_count * sizeof(uint16_t));
        if (big_endian) {
            uint16_t* parent_index = skel->parent_index.data();
            for (uint32_t j = 0; j < motion_bone_count; j++)
                parent_index[j] = reverse_endianness_uint16_t(parent_index[j]);
        }
        s.position_pop();
        s.position_pop();
    }
    s.position_pop();

    s.position_push(skeleton_name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++)
            bone_data->skeleton[i].name = s.read_string_null_terminated_offset(
                s.read_offset_f2(header_length));
    else
        for (uint32_t i = 0; i < skeleton_count; i++)
            bone_data->skeleton[i].name = s.read_string_null_terminated_offset(
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

    uint32_t skeleton_count = (uint32_t)bone_data->skeleton.size();

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
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            uint32_t bone_count = (uint32_t)skel->bone.size();
            uint32_t position_count = (uint32_t)skel->position.size();
            uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
            uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

            ee = { off, 1, 56, 1 };
            ee.append(0, 14, ENRS_DWORD);
            e.vec.push_back(ee);
            pos += off = 56;

            bone_count++;
            off += 8;
            pos += 8;
            ee = { off, 1, 12, bone_count };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(bone_count * 12ULL);
            if (pos + off % 0x10)
                off -= 8;
            pos += off = align_val(pos + off, 0x10) - pos;
            bone_count--;

            ee = { off, 1, 12, position_count };
            ee.append(0, 3, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(position_count * 12ULL);
            pos += off = align_val(off, 0x10);

            ee = { off, 1, 4, 1 };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            pos += off = 4;

            object_bone_count += motion_bone_count;
            ee = { off, 1, (uint32_t)(object_bone_count * 4ULL), 1 };
            ee.append(0, object_bone_count, ENRS_DWORD);
            e.vec.push_back(ee);
            pos += off = (uint32_t)(object_bone_count * 4ULL);
            object_bone_count -= motion_bone_count;

            ee = { off, 1, (uint32_t)(motion_bone_count * 2ULL), 1 };
            ee.append(0, motion_bone_count, ENRS_WORD);
            e.vec.push_back(ee);
            off = (uint32_t)(motion_bone_count * 2ULL);
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
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            uint32_t bone_count = (uint32_t)skel->bone.size();
            uint32_t position_count = (uint32_t)skel->position.size();
            uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
            uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

            bone_count++;
            off += 8;
            ee = { off, 1, 16, bone_count };
            ee.append(0, 1, ENRS_QWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(bone_count * 16ULL);
            bone_count--;

            ee = { off, 1, 12, position_count };
            ee.append(0, 3, ENRS_DWORD);
            e.vec.push_back(ee);
            off = (uint32_t)(position_count * 12ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 4, 1 };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            off = 4;
            off = align_val(off, 0x10);

            if (object_bone_count % 1) {
                ee = { off, 1, 12, position_count };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(position_count * 12ULL);
                off = align_val(off, 0x10);
            }

            if (skeleton_count % 2) {
                ee = { off, 1, (uint32_t)(object_bone_count * 8ULL), 1 };
                ee.append(0, object_bone_count, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(object_bone_count * 8ULL);
                off = align_val(off, 0x10);

                ee = { off, 1, (uint32_t)(motion_bone_count * 8ULL), 1 };
                ee.append(0, motion_bone_count, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(motion_bone_count * 8ULL);
                off = align_val(off, 0x10);
            }
            else {
                object_bone_count += motion_bone_count;
                ee = { off, 1, (uint32_t)(object_bone_count * 8ULL), 1 };
                ee.append(0, object_bone_count, ENRS_QWORD);
                e.vec.push_back(ee);
                off = (uint32_t)(object_bone_count * 8ULL);
                off = align_val(off, 0x10);
                object_bone_count -= motion_bone_count;
            }

            ee = { off, 1, (uint32_t)(motion_bone_count * 2ULL), 1 };
            ee.append(0, motion_bone_count, ENRS_WORD);
            e.vec.push_back(ee);
            off = (uint32_t)(motion_bone_count * 2ULL);
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
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
    else {
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_x_pof_add(s_bone, 0, &pof);
        s_bone.align_write(0x10);
    }

    int64_t skeleton_name_offsets_offset = s_bone.get_position();
    if (!is_x)
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
    else {
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_x_pof_add(s_bone, 0, &pof);
        s_bone.align_write(0x10);
    }

    bone_database_skeleton_header* skh = force_malloc<bone_database_skeleton_header>(skeleton_count);

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            uint32_t bone_count = (uint32_t)skel->bone.size();
            uint32_t position_count = (uint32_t)skel->position.size();
            uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
            uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

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
            for (bone_database_bone& j : skel->bone) {
                s_bone.write(0x08);
                io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            }

            s_bone.write(0x08);
            io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);
            s_bone.align_write(0x10);

            skh[i].positions_offset = s_bone.get_position();
            s_bone.write(sizeof(vec3) * position_count);
            s_bone.align_write(0x10);

            skh[i].unknown_value_offset = s_bone.get_position();
            s_bone.write_float_t(0.0f);

            skh[i].object_bone_names_offset = s_bone.get_position();
            for (std::string& j : skel->object_bone)
                io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);

            skh[i].motion_bone_names_offset = s_bone.get_position();
            for (std::string& j : skel->motion_bone)
                io_write_offset_f2_pof_add(s_bone, 0, 0x40, &pof);

            skh[i].parent_indices_offset = s_bone.get_position();
            s_bone.write(sizeof(uint16_t) * motion_bone_count);
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
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            uint32_t bone_count = (uint32_t)skel->bone.size();
            uint32_t position_count = (uint32_t)skel->position.size();
            uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
            uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

            skh[i].bones_offset = s_bone.get_position();
            for (bone_database_bone& j : skel->bone) {
                s_bone.write(0x08);
                io_write_offset_x_pof_add(s_bone, 0, &pof);
            }

            s_bone.write(0x08);
            io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.align_write(0x10);

            skh[i].positions_offset = s_bone.get_position();
            s_bone.write(&s_bone, sizeof(vec3) * position_count);
            s_bone.align_write(0x10);

            skh[i].unknown_value_offset = s_bone.get_position();
            s_bone.write_float_t(0.0f);
            s_bone.align_write(0x10);

            skh[i].object_bone_names_offset = s_bone.get_position();
            for (std::string& j : skel->object_bone)
                io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.align_write(0x10);

            skh[i].motion_bone_names_offset = s_bone.get_position();
            for (std::string& j : skel->motion_bone)
                io_write_offset_x_pof_add(s_bone, 0, &pof);
            s_bone.align_write(0x10);

            skh[i].parent_indices_offset = s_bone.get_position();
            s_bone.write(sizeof(uint16_t) * motion_bone_count);
            s_bone.align_write(0x10);
        }
    }

    std::vector<std::string> strings;
    std::vector<int64_t> string_offsets;

    if (is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            if (bone_database_strings_push_back_check(strings, skel->name.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_utf8_string_null_terminated(skel->name.c_str());
            }
        }

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];

        for (bone_database_bone& j : skel->bone)
            if (bone_database_strings_push_back_check(strings, j.name.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_string_null_terminated(j.name);
            }

        if (bone_database_strings_push_back_check(strings, "End")) {
            string_offsets.push_back(s_bone.get_position());
            s_bone.write_utf8_string_null_terminated("End");
        }

        for (std::string& j : skel->object_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_string_null_terminated(j);
            }

        for (std::string& j : skel->motion_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_string_null_terminated(j);
            }
    }

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            if (bone_database_strings_push_back_check(strings, skel->name.c_str())) {
                string_offsets.push_back(s_bone.get_position());
                s_bone.write_utf8_string_null_terminated(skel->name.c_str());
            }
        }
    s_bone.align_write(0x10);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];

        uint32_t bone_count = (uint32_t)skel->bone.size();
        uint32_t position_count = (uint32_t)skel->position.size();
        uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
        uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

        s_bone.position_push(skh[i].offset, SEEK_SET);
        if (!is_x) {
            s_bone.write_offset_f2(skh[i].bones_offset, 0x40);
            s_bone.write_uint32_t_reverse_endianness(position_count);
            s_bone.write_offset_f2(skh[i].positions_offset, 0x40);
            s_bone.write_offset_f2(skh[i].unknown_value_offset, 0x40);
            s_bone.write_uint32_t_reverse_endianness(object_bone_count);
            s_bone.write_offset_f2(skh[i].object_bone_names_offset, 0x40);
            s_bone.write_uint32_t_reverse_endianness(motion_bone_count);
            s_bone.write_offset_f2(skh[i].motion_bone_names_offset, 0x40);
            s_bone.write_offset_f2(skh[i].parent_indices_offset, 0x40);
            s_bone.write(0x14);
        }
        else {
            s_bone.write_offset_x(skh[i].bones_offset);
            s_bone.write_uint32_t_reverse_endianness(position_count);
            s_bone.write_offset_x(skh[i].positions_offset);
            s_bone.write_offset_x(skh[i].unknown_value_offset);
            s_bone.write_uint32_t_reverse_endianness(object_bone_count);
            s_bone.write_offset_x(skh[i].object_bone_names_offset);
            s_bone.write_uint32_t_reverse_endianness(motion_bone_count);
            s_bone.write_offset_x(skh[i].motion_bone_names_offset);
            s_bone.write_offset_x(skh[i].parent_indices_offset);
            s_bone.write(0x28);
        }
        s_bone.position_pop();

        s_bone.position_push(skh[i].bones_offset, SEEK_SET);
        if (!is_x)
            for (bone_database_bone& j : skel->bone) {
                s_bone.write_uint8_t((uint8_t)j.type);
                s_bone.write_uint8_t(j.has_parent ? 1 : 0);
                s_bone.write_uint8_t(j.parent);
                s_bone.write_uint8_t(j.pole_target);
                s_bone.write_uint8_t(j.mirror);
                s_bone.write_uint8_t(j.flags);
                s_bone.write(0x02);
                s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.name.c_str()), 0x40);
            }
        else
            for (bone_database_bone& j : skel->bone) {
                s_bone.write_uint8_t((uint8_t)j.type);
                s_bone.write_uint8_t(j.has_parent ? 1 : 0);
                s_bone.write_uint8_t(j.parent);
                s_bone.write_uint8_t(j.pole_target);
                s_bone.write_uint8_t(j.mirror);
                s_bone.write_uint8_t(j.flags);
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

        s_bone.position_push(skh[i].positions_offset, SEEK_SET);
        for (vec3& j : skel->position) {
            s_bone.write_float_t_reverse_endianness(j.x);
            s_bone.write_float_t_reverse_endianness(j.y);
            s_bone.write_float_t_reverse_endianness(j.z);
        }
        s_bone.position_pop();

        s_bone.position_push(skh[i].unknown_value_offset, SEEK_SET);
        s_bone.write_float_t_reverse_endianness(skel->heel_height);
        s_bone.position_pop();

        s_bone.position_push(skh[i].object_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (std::string& j : skel->object_bone)
                s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()), 0x40);
        else
            for (std::string& j : skel->object_bone)
                s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()));

        s_bone.position_push(skh[i].motion_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (std::string& j : skel->motion_bone)
                s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()), 0x40);
        else
            for (std::string& j : skel->motion_bone)
                s_bone.write_offset_x(bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()));
        s_bone.position_pop();

        s_bone.position_push(skh[i].parent_indices_offset, SEEK_SET);
        for (uint16_t& j : skel->parent_index)
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
        for (bone_database_skeleton& i : bone_data->skeleton)
            s_bone.write_offset_f2(bone_database_strings_get_string_offset(strings,
                string_offsets, i.name.c_str()), 0x40);
    else
        for (bone_database_skeleton& i : bone_data->skeleton)
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
