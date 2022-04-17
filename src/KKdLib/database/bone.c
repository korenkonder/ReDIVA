/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "bone.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

typedef struct bone_database_skeleton_header {
    ssize_t offset;
    ssize_t bones_offset;
    ssize_t positions_offset;
    ssize_t unknown_value_offset;
    ssize_t object_bone_names_offset;
    ssize_t motion_bone_names_offset;
    ssize_t parent_indices_offset;
} bone_database_skeleton_header;

static void bone_database_classic_read_inner(bone_database* bone_data, stream* s);
static void bone_database_classic_write_inner(bone_database* bone_data, stream* s);
static void bone_database_modern_read_inner(bone_database* bone_data, stream* s, uint32_t header_length);
static void bone_database_modern_write_inner(bone_database* bone_data, stream* s);
static ssize_t bone_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<ssize_t>& vec_off, const char* str);
static bool bone_database_strings_push_back_check(std::vector<std::string>& vec, const char* str);

bone_database::bone_database() : ready(), modern(), is_x() {

}

bone_database::~bone_database() {

}

void bone_database::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                bone_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        char* path_bon = str_utils_add(path, ".bon");
        if (path_check_file_exists(path_bon)) {
            f2_struct st;
            st.read(path_bon);
            if (st.header.signature == reverse_endianness_uint32_t('BONE')) {
                stream s_bone;
                io_open(&s_bone, &st.data);
                s_bone.is_big_endian = st.header.use_big_endian;
                bone_database_modern_read_inner(this, &s_bone, st.header.length);
                io_free(&s_bone);
            }
        }
        free(path_bon);
    }
}

void bone_database::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                bone_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_add(path, L".bon");
        if (path_check_file_exists(path_bon)) {
            f2_struct st;
            st.read(path_bon);
            if (st.header.signature == reverse_endianness_uint32_t('BONE')) {
                stream s_bone;
                io_open(&s_bone, &st.data);
                s_bone.is_big_endian = st.header.use_big_endian;
                bone_database_modern_read_inner(this, &s_bone, st.header.length);
                io_free(&s_bone);
            }
        }
        free(path_bon);
    }
}

void bone_database::read(const void* data, size_t length, bool modern) {
    if (!data || !length)
        return;

    if (!modern) {
        stream s;
        io_open(&s, data, length);
        bone_database_classic_read_inner(this, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        st.read(data, length);
        if (st.header.signature == reverse_endianness_uint32_t('BONE')) {
            stream s_bone;
            io_open(&s_bone, &st.data);
            s_bone.is_big_endian = st.header.use_big_endian;
            bone_database_modern_read_inner(this, &s_bone, st.header.length);
            io_free(&s_bone);
        }
    }
}

void bone_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            bone_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_bon = str_utils_add(path, ".bon");
        stream s;
        io_open(&s, path_bon, "wb");
        if (s.io.stream)
            bone_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_bon);
    }
}

void bone_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        stream s;
        io_open(&s, path_bin, L"wb");
        if (s.io.stream)
            bone_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_add(path, L".bon");
        stream s;
        io_open(&s, path_bon, L"wb");
        if (s.io.stream)
            bone_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_bon);
    }
}

void bone_database::write(void** data, size_t* length) {
    if (!data || !ready)
        return;

    stream s;
    io_open(&s);
    if (!modern)
        bone_database_classic_write_inner(this, &s);
    else
        bone_database_modern_write_inner(this, &s);
    io_align_write(&s, 0x10);
    io_copy(&s, data, length);
    io_free(&s);
}

bool bone_database::get_skeleton(const char* name, bone_database_skeleton** skeleton) {
    if (!skeleton)
        return false;

    *skeleton = 0;
    if ( !name)
        return false;

    for (bone_database_skeleton& i : this->skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *skeleton = &i;
            return true;
        }
    return false;
}

int32_t bone_database::get_skeleton_bone_index(const char* name, const char* bone_name) {
    if (!name || !bone_name)
        return -1;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            for (bone_database_bone& j : i.bone)
                if (!str_utils_compare(j.name.c_str(), bone_name))
                    return (int32_t)(&j - i.bone.data());
            return -1;
        }
    return -1;
}

bool bone_database::get_skeleton_bones(const char* name, std::vector<bone_database_bone>** bone) {
    if (!bone)
        return false;

    *bone = 0;
    if (!name)
        return false;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *bone = &i.bone;
            return true;
        }
    return false;
}

bool bone_database::get_skeleton_positions(const char* name, std::vector<vec3>** positions) {
    if (!positions)
        return false;

    *positions = 0;
    if (!name)
        return false;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *positions = &i.position;
            return true;
        }
    return false;
}

int32_t bone_database::get_skeleton_object_bone_index(const char* name, const char* bone_name) {
    if (!name || !bone_name)
        return -1;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            for (std::string& j : i.object_bone)
                if (!str_utils_compare(j.c_str(), bone_name))
                    return (int32_t)(&j - i.object_bone.data());
            return -1;
        }
    return -1;
}

bool bone_database::get_skeleton_object_bones(const char* name, std::vector<std::string>** object_bones) {
    if (!object_bones)
        return false;

    *object_bones = 0;
    if (!name)
        return false;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *object_bones = &i.object_bone;
            return true;
        }
    return false;
}

int32_t bone_database::get_skeleton_motion_bone_index(const char* name, const char* bone_name) {
    if (!name || !bone_name)
        return -1;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            for (std::string& j : i.motion_bone)
                if (!str_utils_compare(j.c_str(), bone_name))
                    return (int32_t)(&j - i.motion_bone.data());
            return -1;
        }
    return -1;
}

bool bone_database::get_skeleton_motion_bones(const char* name, std::vector<std::string>** motion_bones) {
    if (!motion_bones)
        return false;

    *motion_bones = 0;
    if (!name)
        return false;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *motion_bones = &i.motion_bone;
            return true;
        }
    return false;
}

bool bone_database::get_skeleton_parent_indices(const char* name, std::vector<std::uint16_t>** parent_indices) {
    if (!parent_indices)
        return false;

    *parent_indices = 0;
    if (!name)
        return false;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *parent_indices = &i.parent_index;
            return true;
        }
    return false;
}

bool bone_database::get_skeleton_heel_height(const char* name, float_t** unknown_value) {
    if (!unknown_value)
        return false;

    *unknown_value = 0;
    if (!name)
        return false;

    for (bone_database_skeleton& i : skeleton)
        if (!str_utils_compare(i.name.c_str(), name)) {
            *unknown_value = &i.heel_height;
            return true;
        }
    return false;
}

bool bone_database::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    bone_database* bone_data = (bone_database*)data;
    bone_data->read(string_data(&s), bone_data->modern);

    string_free(&s);
    return bone_data->ready;
}

void bone_database_bones_calculate_count(std::vector<bone_database_bone>* bones, size_t* object_bone_count,
    size_t* motion_bone_count, size_t* total_bone_count, size_t* ik_bone_count, size_t* chain_pos) {
    size_t _object_bone_count = 0;
    size_t _motion_bone_count = 0;
    size_t _total_bone_count = 0;
    size_t _ik_bone_count = 0;
    size_t _chain_pos = 0;
    for (bone_database_bone& i : *bones)
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

bone_database_bone::bone_database_bone() : type(),
has_parent(), parent(), pole_target(), mirror(), flags() {

}

bone_database_bone::~bone_database_bone() {

}

bone_database_skeleton::bone_database_skeleton() : heel_height() {

}

bone_database_skeleton::~bone_database_skeleton() {

}

static void bone_database_classic_read_inner(bone_database* bone_data, stream* s) {
    uint32_t signature = io_read_uint32_t(s);
    if (signature != 0x09102720)
        return;

    uint32_t skeleton_count = io_read_uint32_t(s);
    uint32_t skeleton_offsets_offset = io_read_uint32_t(s);
    uint32_t skeleton_name_offsets_offset = io_read_uint32_t(s);
    io_read(s, 0x14);

    bone_data->skeleton.resize(skeleton_count);

    io_position_push(s, skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];
        uint32_t skeleton_offset = io_read_uint32_t(s);

        io_position_push(s, skeleton_offset, SEEK_SET);
        uint32_t bones_offset = io_read_uint32_t(s);
        uint32_t position_count = io_read_uint32_t(s);
        uint32_t positions_offset = io_read_uint32_t(s);
        uint32_t unknown_value_offset = io_read_uint32_t(s);
        uint32_t object_bone_count = io_read_uint32_t(s);
        uint32_t object_bone_names_offset = io_read_uint32_t(s);
        uint32_t motion_bone_count = io_read_uint32_t(s);
        uint32_t motion_bone_names_offset = io_read_uint32_t(s);
        uint32_t parent_indices_offset = io_read_uint32_t(s);
        io_read(s, 0x14);

        uint32_t bone_count = 0;
        io_position_push(s, bones_offset, SEEK_SET);
        while (true) {
            if (io_read_uint8_t(s) == 0xFF)
                break;

            io_read(s, 0x0B);
            bone_count++;
        }
        io_position_pop(s);

        skel->bone.resize(bone_count);

        io_position_push(s, bones_offset, SEEK_SET);
        for (uint32_t j = 0; j < bone_count; j++) {
            bone_database_bone* bone = &skel->bone[j];
            bone->type = (bone_database_bone_type)io_read_uint8_t(s);
            bone->has_parent = io_read_uint8_t(s) ? true : false;
            bone->parent = io_read_uint8_t(s);
            bone->pole_target = io_read_uint8_t(s);
            bone->mirror = io_read_uint8_t(s);
            bone->flags = io_read_uint8_t(s);
            io_read(s, 0x02);
            io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &bone->name);
        }
        io_position_pop(s);

        skel->position.resize(position_count);

        io_position_push(s, positions_offset, SEEK_SET);
        for (uint32_t j = 0; j < position_count; j++) {
            vec3* position = &skel->position[j];
            position->x = io_read_float_t(s);
            position->y = io_read_float_t(s);
            position->z = io_read_float_t(s);
        }
        io_position_pop(s);

        io_position_push(s, unknown_value_offset, SEEK_SET);
        skel->heel_height = io_read_float_t(s);
        io_position_pop(s);

        skel->object_bone.resize(object_bone_count);

        io_position_push(s, object_bone_names_offset, SEEK_SET);
        for (uint32_t j = 0; j < object_bone_count; j++) {
            std::string* object_bone_name = &skel->object_bone[j];
            io_read_string_null_terminated_offset(s, io_read_uint32_t(s), object_bone_name);
        }
        io_position_pop(s);

        skel->motion_bone.resize(motion_bone_count);

        io_position_push(s, motion_bone_names_offset, SEEK_SET);
        for (uint32_t j = 0; j < motion_bone_count; j++) {
            std::string* motion_bone_name = &skel->motion_bone[j];
            io_read_string_null_terminated_offset(s, io_read_uint32_t(s), motion_bone_name);
        }
        io_position_pop(s);

        skel->parent_index.resize(motion_bone_count);

        io_position_push(s, parent_indices_offset, SEEK_SET);
        io_read(s, skel->parent_index.data(), motion_bone_count * sizeof(uint16_t));
        io_position_pop(s);
        io_position_pop(s);
    }
    io_position_pop(s);

    io_position_push(s, skeleton_name_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skeleton = &bone_data->skeleton[i];
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &skeleton->name);
    }
    io_position_pop(s);

    bone_data->is_x = false;
    bone_data->modern = false;
    bone_data->ready = true;
}

static void bone_database_classic_write_inner(bone_database* bone_data, stream* s) {
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);

    uint32_t skeleton_count = (uint32_t)bone_data->skeleton.size();
    ssize_t skeleton_offsets_offset = io_get_position(s);
    io_write(s, skeleton_count * 0x04ULL);

    ssize_t skeleton_name_offset = io_get_position(s);
    for (bone_database_skeleton& i : bone_data->skeleton)
        io_write_string_null_terminated(s, &i.name);

    ssize_t skeleton_name_offsets_offset = io_get_position(s);
    for (bone_database_skeleton& i : bone_data->skeleton) {
        io_write_uint32_t(s, (uint32_t)skeleton_name_offset);
        skeleton_name_offset += i.name.size() + 1;
    }

    std::vector<std::string> strings;
    std::vector<ssize_t> string_offsets;

    ssize_t* skeleton_offsets = force_malloc_s(ssize_t, skeleton_count);

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
                string_offsets.push_back(io_get_position(s));
                io_write_string_null_terminated(s, &j.name);
            }

        if (bone_database_strings_push_back_check(strings, "End")) {
            string_offsets.push_back(io_get_position(s));
            io_write_utf8_string_null_terminated(s, "End");
        }
        io_align_write(s, 0x04);

        ssize_t bones_offset = io_get_position(s);
        for (bone_database_bone& j : skel->bone) {
            io_write_uint8_t(s, (uint8_t)j.type);
            io_write_uint8_t(s, j.has_parent ? 1 : 0);
            io_write_uint8_t(s, j.parent);
            io_write_uint8_t(s, j.pole_target);
            io_write_uint8_t(s, j.mirror);
            io_write_uint8_t(s, j.flags);
            io_write(s, 0x02);
            io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.name.c_str()));
        }

        io_write_uint32_t(s, 0xFF);
        io_write_uint32_t(s, 0xFF);
        io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(strings,
            string_offsets, "End"));

        ssize_t positions_offset = io_get_position(s);
        io_write(s, skel->position.data(), sizeof(vec3) * position_count);

        ssize_t unknown_value_offset = io_get_position(s);
        io_write_float_t(s, skel->heel_height);

        for (std::string& j : skel->object_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(io_get_position(s));
                io_write_string_null_terminated(s, &j);
            }
        io_align_write(s, 0x04);

        ssize_t object_bone_names_offset = io_get_position(s);
        for (std::string& j : skel->object_bone)
            io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.c_str()));

        for (std::string& j : skel->motion_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(io_get_position(s));
                io_write_string_null_terminated(s, &j);
            }
        io_align_write(s, 0x04);

        ssize_t motion_bone_names_offset = io_get_position(s);
        for (std::string& j : skel->motion_bone)
            io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(strings,
                string_offsets, j.c_str()));

        ssize_t parent_indices_offset = io_get_position(s);
        io_write(s, skel->parent_index.data(), sizeof(uint16_t) * motion_bone_count);
        io_align_write(s, 0x04);

        skeleton_offsets[skel - bone_data->skeleton.data()] = io_get_position(s);
        io_write_uint32_t(s, (uint32_t)bones_offset);
        io_write_uint32_t(s, position_count);
        io_write_uint32_t(s, (uint32_t)positions_offset);
        io_write_uint32_t(s, (uint32_t)unknown_value_offset);
        io_write_uint32_t(s, object_bone_count);
        io_write_uint32_t(s, (uint32_t)object_bone_names_offset);
        io_write_uint32_t(s, motion_bone_count);
        io_write_uint32_t(s, (uint32_t)motion_bone_names_offset);
        io_write_uint32_t(s, (uint32_t)parent_indices_offset);
        io_write(s, 0x14);

        strings.clear();
        string_offsets.clear();
    }

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, 0x09102720);
    io_write_uint32_t(s, skeleton_count);
    io_write_uint32_t(s, (uint32_t)skeleton_offsets_offset);
    io_write_uint32_t(s, (uint32_t)skeleton_name_offsets_offset);
    io_write(s, 0x14);
    io_position_pop(s);

    io_position_push(s, skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++)
        io_write_uint32_t(s, (uint32_t)skeleton_offsets[i]);
    io_position_pop(s);
    free(skeleton_offsets);
}

static void bone_database_modern_read_inner(bone_database* bone_data, stream* s, uint32_t header_length) {
    uint32_t signature = io_read_uint32_t_stream_reverse_endianness(s);
    if (signature != 0x09102720)
        return;

    bool is_x = true;

    io_set_position(s, 0x0C, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;

    io_set_position(s, 0x04, SEEK_SET);

    uint32_t skeleton_count = io_read_uint32_t_stream_reverse_endianness(s);
    ssize_t skeleton_offsets_offset = io_read_offset(s, header_length, is_x);
    ssize_t skeleton_name_offsets_offset = io_read_offset(s, header_length, is_x);

    bone_data->skeleton.resize(skeleton_count);

    io_position_push(s, skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skeleton = &bone_data->skeleton[i];
        ssize_t skeleton_offset = io_read_offset(s, header_length, is_x);

        io_position_push(s, skeleton_offset, SEEK_SET);
        ssize_t bones_offset;
        uint32_t position_count;
        ssize_t positions_offset;
        ssize_t unknown_value_offset;
        uint32_t object_bone_count;
        ssize_t object_bone_names_offset;
        uint32_t motion_bone_count;
        ssize_t motion_bone_names_offset;
        ssize_t parent_indices_offset;
        if (!is_x) {
            bones_offset = io_read_offset_f2(s, header_length);
            position_count = io_read_uint32_t_stream_reverse_endianness(s);
            positions_offset = io_read_offset_f2(s, header_length);
            unknown_value_offset = io_read_offset_f2(s, header_length);
            object_bone_count = io_read_uint32_t_stream_reverse_endianness(s);
            object_bone_names_offset = io_read_offset_f2(s, header_length);
            motion_bone_count = io_read_uint32_t_stream_reverse_endianness(s);
            motion_bone_names_offset = io_read_offset_f2(s, header_length);
            parent_indices_offset = io_read_offset_f2(s, header_length);
            io_read(s, 0x14);
        }
        else {
            bones_offset = io_read_offset_x(s);
            position_count = io_read_uint32_t_stream_reverse_endianness(s);
            positions_offset = io_read_offset_x(s);
            unknown_value_offset = io_read_offset_x(s);
            object_bone_count = io_read_uint32_t_stream_reverse_endianness(s);
            object_bone_names_offset = io_read_offset_x(s);
            motion_bone_count = io_read_uint32_t_stream_reverse_endianness(s);
            motion_bone_names_offset = io_read_offset_x(s);
            parent_indices_offset = io_read_offset_x(s);
            io_read(s, 0x28);
        }

        uint32_t bone_count = 0;
        io_position_push(s, bones_offset, SEEK_SET);
        if (!is_x)
            while (true) {
                if (io_read_uint8_t(s) == 0xFF)
                    break;

                io_read(s, 0x0B);
                bone_count++;
            }
        else
            while (true) {
                if (io_read_uint8_t(s) == 0xFF)
                    break;

                io_read(s, 0x0F);
                bone_count++;
            }
        io_position_pop(s);

        skeleton->bone.resize(bone_count);

        io_position_push(s, bones_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < bone_count; j++) {
                bone_database_bone* bone = &skeleton->bone[j];
                bone->type = (bone_database_bone_type)io_read_uint8_t(s);
                bone->has_parent = io_read_uint8_t(s) ? true : false;
                bone->parent = io_read_uint8_t(s);
                bone->pole_target = io_read_uint8_t(s);
                bone->mirror = io_read_uint8_t(s);
                bone->flags = io_read_uint8_t(s);
                io_read(s, 0x02);
                io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &bone->name);
            }
        else
            for (uint32_t j = 0; j < bone_count; j++) {
                bone_database_bone* bone = &skeleton->bone[j];
                bone->type = (bone_database_bone_type)io_read_uint8_t(s);
                bone->has_parent = io_read_uint8_t(s) ? true : false;
                bone->parent = io_read_uint8_t(s);
                bone->pole_target = io_read_uint8_t(s);
                bone->mirror = io_read_uint8_t(s);
                bone->flags = io_read_uint8_t(s);
                io_read(s, 0x02);
                io_read_string_null_terminated_offset(s, io_read_offset_x(s), &bone->name);
            }
        io_position_pop(s);

        skeleton->position.resize(position_count);

        io_position_push(s, positions_offset, SEEK_SET);
        for (uint32_t j = 0; j < position_count; j++) {
            vec3* position = &skeleton->position[j];
            position->x = io_read_float_t_stream_reverse_endianness(s);
            position->y = io_read_float_t_stream_reverse_endianness(s);
            position->z = io_read_float_t_stream_reverse_endianness(s);
        }
        io_position_pop(s);

        io_position_push(s, unknown_value_offset, SEEK_SET);
        skeleton->heel_height = io_read_float_t_stream_reverse_endianness(s);
        io_position_pop(s);

        skeleton->object_bone.resize(object_bone_count);

        io_position_push(s, object_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < object_bone_count; j++) {
                std::string* object_bone_name = &skeleton->object_bone[j];
                io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), object_bone_name);
            }
        else
            for (uint32_t j = 0; j < object_bone_count; j++) {
                std::string* object_bone_name = &skeleton->object_bone[j];
                io_read_string_null_terminated_offset(s, io_read_offset_x(s), object_bone_name);
            }
        io_position_pop(s);

        skeleton->motion_bone.resize(motion_bone_count);

        io_position_push(s, motion_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < motion_bone_count; j++) {
                std::string* motion_bone_name = &skeleton->motion_bone[j];
                io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), motion_bone_name);
            }
        else
            for (uint32_t j = 0; j < motion_bone_count; j++) {
                std::string* motion_bone_name = &skeleton->motion_bone[j];
                io_read_string_null_terminated_offset(s, io_read_offset_x(s), motion_bone_name);
            }
        io_position_pop(s);

        skeleton->parent_index.resize(motion_bone_count);

        io_position_push(s, parent_indices_offset, SEEK_SET);
        io_read(s, skeleton->parent_index.data(), motion_bone_count * sizeof(uint16_t));
        if (s->is_big_endian) {
            uint16_t* parent_indices = skeleton->parent_index.data();
            for (uint32_t j = 0; j < motion_bone_count; j++)
                parent_indices[j] = reverse_endianness_uint16_t(parent_indices[j]);
        }
        io_position_pop(s);
        io_position_pop(s);
    }
    io_position_pop(s);

    io_position_push(s, skeleton_name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skeleton = &bone_data->skeleton[i];
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &skeleton->name);
        }
    else
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skeleton = &bone_data->skeleton[i];
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &skeleton->name);
        }
    io_position_pop(s);

    bone_data->is_x = is_x;
    bone_data->modern = true;
    bone_data->ready = true;
}

static void bone_database_modern_write_inner(bone_database* bone_data, stream* s) {
    stream s_bone;
    io_open(&s_bone);
    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    bool is_x = bone_data->is_x;

    uint32_t skeleton_count = (uint32_t)bone_data->skeleton.size();

    if (!is_x) {
        uint32_t pos;
        ee = { 0, 1, 16, 1 };
        ee.sub.push_back({ 0, 4, ENRS_DWORD });
        e.vec.push_back(ee);
        pos = off = 16;

        skeleton_count *= 2;
        ee = { off, 1, (uint32_t)(skeleton_count * 4ULL), 1 };
        ee.sub.push_back({ 0, skeleton_count, ENRS_DWORD });
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
            ee.sub.push_back({ 0, 14, ENRS_DWORD });
            e.vec.push_back(ee);
            pos += off = 56;

            bone_count++;
            off += 8;
            pos += 8;
            ee = { off, 1, 12, bone_count };
            ee.sub.push_back({ 0, 1, ENRS_DWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(bone_count * 12ULL);
            if (pos + off % 0x10)
                off -= 8;
            pos += off = align_val(pos + off, 0x10) - pos;
            bone_count--;

            ee = { off, 1, 12, position_count };
            ee.sub.push_back({ 0, 3, ENRS_DWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(position_count * 12ULL);
            pos += off = align_val(off, 0x10);

            ee = { off, 1, 4, 1 };
            ee.sub.push_back({ 0, 1, ENRS_DWORD });
            e.vec.push_back(ee);
            pos += off = 4;

            object_bone_count += motion_bone_count;
            ee = { off, 1, (uint32_t)(object_bone_count * 4ULL), 1 };
            ee.sub.push_back({ 0, object_bone_count, ENRS_DWORD });
            e.vec.push_back(ee);
            pos += off = (uint32_t)(object_bone_count * 4ULL);
            object_bone_count -= motion_bone_count;

            ee = { off, 1, (uint32_t)(motion_bone_count * 2ULL), 1 };
            ee.sub.push_back({ 0, motion_bone_count, ENRS_WORD });
            e.vec.push_back(ee);
            off = (uint32_t)(motion_bone_count * 2ULL);
            pos += off = align_val(off, 0x04);
        }
    }
    else {
        ee = { 0, 2, 24, 1 };
        ee.sub.push_back({ 0, 2, ENRS_DWORD });
        ee.sub.push_back({ 0, 2, ENRS_QWORD });
        e.vec.push_back(ee);
        off = 24;
        off = align_val(off, 0x10);

        if (skeleton_count % 2) {
            ee = { off, 1, (uint32_t)(skeleton_count * 8ULL), 1 };
            ee.sub.push_back({ 0, skeleton_count, ENRS_QWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, (uint32_t)(skeleton_count * 8ULL), 1 };
            ee.sub.push_back({ 0, skeleton_count, ENRS_QWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);
        }
        else {
            skeleton_count *= 2;
            ee = { off, 1, (uint32_t)(skeleton_count * 8ULL), 1 };
            ee.sub.push_back({ 0, skeleton_count, ENRS_QWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);
            skeleton_count /= 2;
        }

        ee = { off, 7, 112, skeleton_count };
        ee.sub.push_back({ 0, 1, ENRS_QWORD });
        ee.sub.push_back({ 0, 1, ENRS_DWORD });
        ee.sub.push_back({ 4, 2, ENRS_QWORD });
        ee.sub.push_back({ 0, 1, ENRS_DWORD });
        ee.sub.push_back({ 4, 1, ENRS_QWORD });
        ee.sub.push_back({ 0, 1, ENRS_DWORD });
        ee.sub.push_back({ 4, 7, ENRS_QWORD });
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
            ee.sub.push_back({ 0, 1, ENRS_QWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(bone_count * 16ULL);
            bone_count--;

            ee = { off, 1, 12, position_count };
            ee.sub.push_back({ 0, 3, ENRS_DWORD });
            e.vec.push_back(ee);
            off = (uint32_t)(position_count * 12ULL);
            off = align_val(off, 0x10);

            ee = { off, 1, 4, 1 };
            ee.sub.push_back({ 0, 1, ENRS_DWORD });
            e.vec.push_back(ee);
            off = 4;
            off = align_val(off, 0x10);

            if (object_bone_count % 1) {
                ee = { off, 1, 12, position_count };
                ee.sub.push_back({ 0, 3, ENRS_DWORD });
                e.vec.push_back(ee);
                off = (uint32_t)(position_count * 12ULL);
                off = align_val(off, 0x10);
            }

            if (skeleton_count % 2) {
                ee = { off, 1, (uint32_t)(object_bone_count * 8ULL), 1 };
                ee.sub.push_back({ 0, object_bone_count, ENRS_QWORD });
                e.vec.push_back(ee);
                off = (uint32_t)(object_bone_count * 8ULL);
                off = align_val(off, 0x10);

                ee = { off, 1, (uint32_t)(motion_bone_count * 8ULL), 1 };
                ee.sub.push_back({ 0, motion_bone_count, ENRS_QWORD });
                e.vec.push_back(ee);
                off = (uint32_t)(motion_bone_count * 8ULL);
                off = align_val(off, 0x10);
            }
            else {
                object_bone_count += motion_bone_count;
                ee = { off, 1, (uint32_t)(object_bone_count * 8ULL), 1 };
                ee.sub.push_back({ 0, object_bone_count, ENRS_QWORD });
                e.vec.push_back(ee);
                off = (uint32_t)(object_bone_count * 8ULL);
                off = align_val(off, 0x10);
                object_bone_count -= motion_bone_count;
            }

            ee = { off, 1, (uint32_t)(motion_bone_count * 2ULL), 1 };
            ee.sub.push_back({ 0, motion_bone_count, ENRS_WORD });
            e.vec.push_back(ee);
            off = (uint32_t)(motion_bone_count * 2ULL);
            off = align_val(off, 0x10);
        }
    }

    io_write_int32_t(&s_bone, 0);
    io_write_int32_t(&s_bone, 0);
    io_write_offset_pof_add(&s_bone, 0, 0x40, is_x, &pof);
    io_write_offset_pof_add(&s_bone, 0, 0x40, is_x, &pof);
    io_align_write(&s_bone, 0x10);

    ssize_t skeleton_offsets_offset = io_get_position(&s_bone);
    if (!is_x)
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
    else {
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
        io_align_write(&s_bone, 0x10);
    }

    ssize_t skeleton_name_offsets_offset = io_get_position(&s_bone);
    if (!is_x)
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
    else {
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
        io_align_write(&s_bone, 0x10);
    }

    bone_database_skeleton_header* skh = force_malloc_s(bone_database_skeleton_header, skeleton_count);

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            uint32_t bone_count = (uint32_t)skel->bone.size();
            uint32_t position_count = (uint32_t)skel->position.size();
            uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
            uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

            skh[i].offset = io_get_position(&s_bone);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_write_uint32_t(&s_bone, 0);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_write_uint32_t(&s_bone, 0);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_write_uint32_t(&s_bone, 0);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_write(&s_bone, 0x14);

            skh[i].bones_offset = io_get_position(&s_bone);
            for (bone_database_bone& j : skel->bone) {
                io_write(&s_bone, 0x08);
                io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            }

            io_write(&s_bone, 0x08);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].positions_offset = io_get_position(&s_bone);
            io_write(&s_bone, sizeof(vec3) * position_count);
            io_align_write(&s_bone, 0x10);

            skh[i].unknown_value_offset = io_get_position(&s_bone);
            io_write_float_t(&s_bone, 0.0f);

            skh[i].object_bone_names_offset = io_get_position(&s_bone);
            for (std::string& j : skel->object_bone)
                io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);

            skh[i].motion_bone_names_offset = io_get_position(&s_bone);
            for (std::string& j : skel->motion_bone)
                io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);

            skh[i].parent_indices_offset = io_get_position(&s_bone);
            io_write(&s_bone, sizeof(uint16_t) * motion_bone_count);
            io_align_write(&s_bone, 0x04);
        }
    else {
        for (uint32_t i = 0; i < skeleton_count; i++) {
            skh[i].offset = io_get_position(&s_bone);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_write_uint32_t(&s_bone, 0);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_write_uint32_t(&s_bone, 0);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_write_uint32_t(&s_bone, 0);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_write(&s_bone, 0x28);
            io_align_write(&s_bone, 0x10);
        }

        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            uint32_t bone_count = (uint32_t)skel->bone.size();
            uint32_t position_count = (uint32_t)skel->position.size();
            uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
            uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();


            skh[i].bones_offset = io_get_position(&s_bone);
            for (bone_database_bone& j : skel->bone) {
                io_write(&s_bone, 0x08);
                io_write_offset_x_pof_add(&s_bone, 0, &pof);
            }

            io_write(&s_bone, 0x08);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].positions_offset = io_get_position(&s_bone);
            io_write(&s_bone, sizeof(vec3) * position_count);
            io_align_write(&s_bone, 0x10);

            skh[i].unknown_value_offset = io_get_position(&s_bone);
            io_write_float_t(&s_bone, 0.0f);
            io_align_write(&s_bone, 0x10);

            skh[i].object_bone_names_offset = io_get_position(&s_bone);
            for (std::string& j : skel->object_bone)
                io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].motion_bone_names_offset = io_get_position(&s_bone);
            for (std::string& j : skel->motion_bone)
                io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].parent_indices_offset = io_get_position(&s_bone);
            io_write(&s_bone, sizeof(uint16_t) * motion_bone_count);
            io_align_write(&s_bone, 0x10);
        }
    }

    std::vector<std::string> strings;
    std::vector<ssize_t> string_offsets;

    if (is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            if (bone_database_strings_push_back_check(strings, skel->name.c_str())) {
                string_offsets.push_back(io_get_position(&s_bone));
                io_write_utf8_string_null_terminated(&s_bone, skel->name.c_str());
            }
        }

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];

        for (bone_database_bone& j : skel->bone)
            if (bone_database_strings_push_back_check(strings, j.name.c_str())) {
                string_offsets.push_back(io_get_position(&s_bone));
                io_write_string_null_terminated(&s_bone, &j.name);
            }

        if (bone_database_strings_push_back_check(strings, "End")) {
            string_offsets.push_back(io_get_position(&s_bone));
            io_write_utf8_string_null_terminated(&s_bone, "End");
        }

        for (std::string& j : skel->object_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(io_get_position(&s_bone));
                io_write_string_null_terminated(&s_bone, &j);
            }

        for (std::string& j : skel->motion_bone)
            if (bone_database_strings_push_back_check(strings, j.c_str())) {
                string_offsets.push_back(io_get_position(&s_bone));
                io_write_string_null_terminated(&s_bone, &j);
            }
    }

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton[i];

            if (bone_database_strings_push_back_check(strings, skel->name.c_str())) {
                string_offsets.push_back(io_get_position(&s_bone));
                io_write_utf8_string_null_terminated(&s_bone, skel->name.c_str());
            }
        }
    io_align_write(&s_bone, 0x10);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton[i];

        uint32_t bone_count = (uint32_t)skel->bone.size();
        uint32_t position_count = (uint32_t)skel->position.size();
        uint32_t object_bone_count = (uint32_t)skel->object_bone.size();
        uint32_t motion_bone_count = (uint32_t)skel->motion_bone.size();

        io_position_push(&s_bone, skh[i].offset, SEEK_SET);
        if (!is_x) {
            io_write_offset_f2(&s_bone, skh[i].bones_offset, 0x40);
            io_write_uint32_t(&s_bone, position_count);
            io_write_offset_f2(&s_bone, skh[i].positions_offset, 0x40);
            io_write_offset_f2(&s_bone, skh[i].unknown_value_offset, 0x40);
            io_write_uint32_t(&s_bone, object_bone_count);
            io_write_offset_f2(&s_bone, skh[i].object_bone_names_offset, 0x40);
            io_write_uint32_t(&s_bone, motion_bone_count);
            io_write_offset_f2(&s_bone, skh[i].motion_bone_names_offset, 0x40);
            io_write_offset_f2(&s_bone, skh[i].parent_indices_offset, 0x40);
            io_write(&s_bone, 0x14);
        }
        else {
            io_write_offset_x(&s_bone, skh[i].bones_offset);
            io_write_uint32_t(&s_bone, position_count);
            io_write_offset_x(&s_bone, skh[i].positions_offset);
            io_write_offset_x(&s_bone, skh[i].unknown_value_offset);
            io_write_uint32_t(&s_bone, object_bone_count);
            io_write_offset_x(&s_bone, skh[i].object_bone_names_offset);
            io_write_uint32_t(&s_bone, motion_bone_count);
            io_write_offset_x(&s_bone, skh[i].motion_bone_names_offset);
            io_write_offset_x(&s_bone, skh[i].parent_indices_offset);
            io_write(&s_bone, 0x28);
        }
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].bones_offset, SEEK_SET);
        if (!is_x)
            for (bone_database_bone& j : skel->bone) {
                io_write_uint8_t(&s_bone, (uint8_t)j.type);
                io_write_uint8_t(&s_bone, j.has_parent ? 1 : 0);
                io_write_uint8_t(&s_bone, j.parent);
                io_write_uint8_t(&s_bone, j.pole_target);
                io_write_uint8_t(&s_bone, j.mirror);
                io_write_uint8_t(&s_bone, j.flags);
                io_write(&s_bone, 0x02);
                io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(strings,
                    string_offsets, j.name.c_str()), 0x40);
            }
        else
            for (bone_database_bone& j : skel->bone) {
                io_write_uint8_t(&s_bone, (uint8_t)j.type);
                io_write_uint8_t(&s_bone, j.has_parent ? 1 : 0);
                io_write_uint8_t(&s_bone, j.parent);
                io_write_uint8_t(&s_bone, j.pole_target);
                io_write_uint8_t(&s_bone, j.mirror);
                io_write_uint8_t(&s_bone, j.flags);
                io_write(&s_bone, 0x02);
                io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(strings,
                    string_offsets, j.name.c_str()));
            }

        if (!is_x) {
            io_write_uint32_t(&s_bone, 0xFF);
            io_write_uint32_t(&s_bone, 0xFF);
            io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(strings,
                string_offsets, "End"), 0x40);
        }
        else {

            io_write_uint32_t(&s_bone, 0xFF);
            io_write_uint32_t(&s_bone, 0xFF);
            io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(strings,
                string_offsets, "End"));
        }
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].positions_offset, SEEK_SET);
        io_write(&s_bone, skel->position.data(), sizeof(vec3)* position_count);
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].unknown_value_offset, SEEK_SET);
        io_write_float_t(&s_bone, skel->heel_height);
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].object_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (std::string& j : skel->object_bone)
                io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()), 0x40);
        else
            for (std::string& j : skel->object_bone)
                io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()));

        io_position_push(&s_bone, skh[i].motion_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (std::string& j : skel->motion_bone)
                io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()), 0x40);
        else
            for (std::string& j : skel->motion_bone)
                io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(strings,
                    string_offsets, j.c_str()));
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].parent_indices_offset, SEEK_SET);
        io_write(&s_bone, skel->parent_index.data(), sizeof(uint16_t) * motion_bone_count);
        io_position_pop(&s_bone);
    }

    io_position_push(&s_bone, 0x00, SEEK_SET);
    io_write_uint32_t(&s_bone, 0x09102720);
    io_write_uint32_t(&s_bone, skeleton_count);
    io_write_offset(&s_bone, skeleton_offsets_offset, 0x40, is_x);
    io_write_offset(&s_bone, skeleton_name_offsets_offset, 0x40, is_x);
    io_position_pop(&s_bone);

    io_position_push(&s_bone, skeleton_offsets_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++)
            io_write_offset_f2(&s_bone, skh[i].offset, 0x40);
    else
        for (uint32_t i = 0; i < skeleton_count; i++)
            io_write_offset_x(&s_bone, skh[i].offset);
    io_position_pop(&s_bone);

    io_position_push(&s_bone, skeleton_name_offsets_offset, SEEK_SET);
    if (!is_x)
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(strings,
                string_offsets, i.name.c_str()), 0x40);
    else
        for (bone_database_skeleton& i : bone_data->skeleton)
            io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(strings,
                string_offsets, i.name.c_str()));
    io_position_pop(&s_bone);

    free(skh);

    f2_struct st;
    io_align_write(&s_bone, 0x10);
    io_copy(&s_bone, &st.data);
    io_free(&s_bone);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('BONE');
    st.header.length = is_x ? 0x20 : 0x40;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    st.write(s, true, is_x);
}

inline static ssize_t bone_database_strings_get_string_offset(std::vector<std::string>& vec,
    std::vector<ssize_t>& vec_off, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool bone_database_strings_push_back_check(std::vector<std::string>& vec, const char* str) {
    size_t len = utf8_length(str);
    for (std::string& i : vec)
        if (!memcmp(str, i.c_str(), min(len, i.size()) + 1))
            return false;

    vec.push_back(std::string(str, len));
    return true;
}
