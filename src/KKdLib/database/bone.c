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

vector_func(bone_database_bone)
vector_func(bone_database_skeleton)

static void bone_database_classic_read_inner(bone_database* bone_data, stream* s);
static void bone_database_classic_write_inner(bone_database* bone_data, stream* s);
static void bone_database_modern_read_inner(bone_database* bone_data, stream* s, uint32_t header_length);
static void bone_database_modern_write_inner(bone_database* bone_data, stream* s);
static ssize_t bone_database_strings_get_string_offset(vector_string* vec,
    vector_ssize_t* vec_off, char* str);
static bool bone_database_strings_push_back_check(vector_string* vec, char* str);

void bone_database_init(bone_database* bone_data) {
    memset(bone_data, 0, sizeof(bone_database));
}

void bone_database_read(bone_database* bone_data, char* path, bool modern) {
    if (!bone_data || !path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc(s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_mopen(&s_bin, data, s.length);
                bone_database_classic_read_inner(bone_data, &s_bin);
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
            f2_struct_read(&st, path_bon);
            if (st.header.signature == reverse_endianness_uint32_t('BONE')); {
                stream s_bone;
                io_mopen(&s_bone, st.data, st.length);
                s_bone.is_big_endian = st.header.use_big_endian;
                bone_database_modern_read_inner(bone_data, &s_bone, st.header.length);
                io_free(&s_bone);
            }
            f2_struct_free(&st);
        }
        free(path_bon);
    }
}

void bone_database_wread(bone_database* bone_data, wchar_t* path, bool modern) {
    if (!bone_data || !path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        if (path_wcheck_file_exists(path_bin)) {
            stream s;
            io_wopen(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc(s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_mopen(&s_bin, data, s.length);
                bone_database_classic_read_inner(bone_data, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_wadd(path, L".bon");
        if (path_wcheck_file_exists(path_bon)) {
            f2_struct st;
            f2_struct_wread(&st, path_bon);
            if (st.header.signature == reverse_endianness_uint32_t('BONE')); {
                stream s_bone;
                io_mopen(&s_bone, st.data, st.length);
                s_bone.is_big_endian = st.header.use_big_endian;
                bone_database_modern_read_inner(bone_data, &s_bone, st.header.length);
                io_free(&s_bone);
            }
            f2_struct_free(&st);
        }
        free(path_bon);
    }
}

void bone_database_mread(bone_database* bone_data, void* data, size_t length, bool modern) {
    if (!bone_data || !data || !length)
        return;

    if (!modern) {
        stream s;
        io_mopen(&s, data, length);
        bone_database_classic_read_inner(bone_data, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        f2_struct_mread(&st, data, length);
        if (st.header.signature == reverse_endianness_uint32_t('BONE')); {
            stream s_bone;
            io_mopen(&s_bone, st.data, st.length);
            s_bone.is_big_endian = st.header.use_big_endian;
            bone_database_modern_read_inner(bone_data, &s_bone, st.header.length);
            io_free(&s_bone);
        }
        f2_struct_free(&st);
    }
}

void bone_database_write(bone_database* bone_data, char* path) {
    if (!bone_data || !path || !bone_data->ready)
        return;

    if (!bone_data->modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            bone_database_classic_write_inner(bone_data, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_bon = str_utils_add(path, ".bon");
        stream s;
        io_open(&s, path_bon, "wb");
        if (s.io.stream)
            bone_database_modern_write_inner(bone_data, &s);
        io_free(&s);
        free(path_bon);
    }
}

void bone_database_wwrite(bone_database* bone_data, wchar_t* path) {
    if (!bone_data || !path || !bone_data->ready)
        return;

    if (!bone_data->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_wopen(&s, path_bin, L"wb");
        if (s.io.stream)
            bone_database_classic_write_inner(bone_data, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_bon = str_utils_wadd(path, L".bon");
        stream s;
        io_wopen(&s, path_bon, L"wb");
        if (s.io.stream)
            bone_database_modern_write_inner(bone_data, &s);
        io_free(&s);
        free(path_bon);
    }
}

void bone_database_mwrite(bone_database* bone_data, void** data, size_t* length) {
    if (!bone_data || !data || !bone_data->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    if (!bone_data->modern)
        bone_database_classic_write_inner(bone_data, &s);
    else
        bone_database_modern_write_inner(bone_data, &s);
    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool bone_database_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    bone_database* bone_data = data;
    bone_database_read(bone_data, string_data(&s), bone_data->modern);

    string_free(&s);
    return bone_data->ready;
}

void bone_database_bones_calculate_count(vector_bone_database_bone* bones, size_t* object_bone_count,
    size_t* motion_bone_count, size_t* total_bone_count, size_t* ik_bone_count, size_t* chain_pos) {
    size_t _object_bone_count = 0;
    size_t _motion_bone_count = 0;
    size_t _total_bone_count = 0;
    size_t _ik_bone_count = 0;
    size_t _chain_pos = 0;
    for (bone_database_bone* i = bones->begin; i != bones->end; i++)
        switch (i->type) {
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

bool bone_database_get_skeleton(bone_database* bone_data,
    char* name, bone_database_skeleton** skeleton) {
    if (!skeleton)
        return false;

    *skeleton = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *skeleton = i;
            return true;
        }
    return false;
}

int32_t bone_database_get_skeleton_bone_index(bone_database* bone_data, char* name, char* bone_name) {
    if (!bone_data || !name || !bone_name)
        return -1;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            for (bone_database_bone* j = i->bone.begin; j != i->bone.end; j++)
                if (!str_utils_compare(string_data(&j->name), bone_name))
                    return (int32_t)(j - i->bone.begin);
            return -1;
        }
    return -1;
}

bool bone_database_get_skeleton_bones(bone_database* bone_data,
    char* name, vector_bone_database_bone** bone) {
    if (!bone)
        return false;

    *bone = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *bone = &i->bone;
            return true;
        }
    return false;
}

bool bone_database_get_skeleton_positions(bone_database* bone_data,
    char* name, vector_vec3** positions) {
    if (!positions)
        return false;

    *positions = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *positions = &i->position;
            return true;
        }
    return false;
}

int32_t bone_database_get_skeleton_object_bone_index(bone_database* bone_data,
    char* name, char* bone_name) {
    if (!bone_data || !name || !bone_name)
        return -1;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            vector_string* object_bones = &i->object_bone;
            for (string* j = object_bones->begin; j != object_bones->end; j++)
                if (!str_utils_compare(string_data(j), bone_name))
                    return (int32_t)(j - object_bones->begin);
            return -1;
        }
    return -1;
}

bool bone_database_get_skeleton_object_bones(bone_database* bone_data,
    char* name, vector_string** object_bones) {
    if (!object_bones)
        return false;

    *object_bones = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *object_bones = &i->object_bone;
            return true;
        }
    return false;
}

int32_t bone_database_get_skeleton_motion_bone_index(bone_database* bone_data,
    char* name, char* bone_name) {
    if (!bone_data || !name || !bone_name)
        return -1;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            vector_string* motion_bones = &i->motion_bone;
            for (string* j = motion_bones->begin; j != motion_bones->end; j++)
                if (!str_utils_compare(string_data(j), bone_name))
                    return (int32_t)(j - motion_bones->begin);
            return -1;
        }
    return -1;
}

bool bone_database_get_skeleton_motion_bones(bone_database* bone_data,
    char* name, vector_string** motion_bones) {
    if (!motion_bones)
        return false;

    *motion_bones = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *motion_bones = &i->motion_bone;
            return true;
        }
    return false;
}

bool bone_database_get_skeleton_parent_indices(bone_database* bone_data,
    char* name, vector_uint16_t** parent_indices) {
    if (!parent_indices)
        return false;

    *parent_indices = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *parent_indices = &i->parent_index;
            return true;
        }
    return false;
}

bool bone_database_get_skeleton_heel_height(bone_database* bone_data,
    char* name, float_t** unknown_value) {
    if (!unknown_value)
        return false;

    *unknown_value = 0;
    if (!bone_data || !name)
        return false;

    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        if (!str_utils_compare(string_data(&i->name), name)) {
            *unknown_value = &i->heel_height;
            return true;
        }
    return false;
}

void bone_database_free(bone_database* bone_data) {
    vector_bone_database_skeleton_free(&bone_data->skeleton, bone_database_skeleton_free);
}

char* bone_database_skeleton_type_to_string(bone_database_skeleton_type type) {
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

void bone_database_bone_free(bone_database_bone* bone) {
    string_free(&bone->name);
}

void bone_database_skeleton_free(bone_database_skeleton* skel) {
    vector_bone_database_bone_free(&skel->bone, bone_database_bone_free);
    vector_vec3_free(&skel->position, 0);;
    vector_string_free(&skel->object_bone, string_free);
    vector_string_free(&skel->motion_bone, string_free);
    vector_uint16_t_free(&skel->parent_index, 0);
    string_free(&skel->name);
}

static void bone_database_classic_read_inner(bone_database* bone_data, stream* s) {
    uint32_t signature = io_read_uint32_t(s);
    if (signature != 0x09102720)
        return;

    uint32_t skeleton_count = io_read_uint32_t(s);
    uint32_t skeleton_offsets_offset = io_read_uint32_t(s);
    uint32_t skeleton_name_offsets_offset = io_read_uint32_t(s);
    io_read(s, 0, 0x14);

    vector_bone_database_skeleton_reserve(&bone_data->skeleton, skeleton_count);
    bone_data->skeleton.end += skeleton_count;

    io_position_push(s, skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton.begin[i];
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
        io_read(s, 0, 0x14);

        uint32_t bone_count = 0;
        io_position_push(s, bones_offset, SEEK_SET);
        while (true) {
            if (io_read_uint8_t(s) == 0xFF)
                break;

            io_read(s, 0, 0x0B);
            bone_count++;
        }
        io_position_pop(s);

        vector_bone_database_bone_reserve(&skel->bone, bone_count);
        skel->bone.end += bone_count;

        io_position_push(s, bones_offset, SEEK_SET);
        for (uint32_t j = 0; j < bone_count; j++) {
            bone_database_bone* bone = &skel->bone.begin[j];
            bone->type = io_read_uint8_t(s);
            bone->has_parent = io_read_uint8_t(s) ? true : false;
            bone->parent = io_read_uint8_t(s);
            bone->pole_target = io_read_uint8_t(s);
            bone->mirror = io_read_uint8_t(s);
            bone->flags = io_read_uint8_t(s);
            io_read(s, 0, 0x02);
            io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &bone->name);
        }
        io_position_pop(s);

        vector_vec3_reserve(&skel->position, position_count);
        skel->position.end += position_count;

        io_position_push(s, positions_offset, SEEK_SET);
        for (uint32_t j = 0; j < position_count; j++) {
            vec3* position = &skel->position.begin[j];
            position->x = io_read_float_t(s);
            position->y = io_read_float_t(s);
            position->z = io_read_float_t(s);
        }
        io_position_pop(s);

        io_position_push(s, unknown_value_offset, SEEK_SET);
        skel->heel_height = io_read_float_t(s);
        io_position_pop(s);

        vector_string_reserve(&skel->object_bone, object_bone_count);
        skel->object_bone.end += object_bone_count;

        io_position_push(s, object_bone_names_offset, SEEK_SET);
        for (uint32_t j = 0; j < object_bone_count; j++) {
            string* object_bone_name = &skel->object_bone.begin[j];
            io_read_string_null_terminated_offset(s, io_read_uint32_t(s), object_bone_name);
        }
        io_position_pop(s);

        vector_string_reserve(&skel->motion_bone, motion_bone_count);
        skel->motion_bone.end += motion_bone_count;

        io_position_push(s, motion_bone_names_offset, SEEK_SET);
        for (uint32_t j = 0; j < motion_bone_count; j++) {
            string* motion_bone_name = &skel->motion_bone.begin[j];
            io_read_string_null_terminated_offset(s, io_read_uint32_t(s), motion_bone_name);
        }
        io_position_pop(s);

        vector_uint16_t_reserve(&skel->parent_index, motion_bone_count);
        skel->parent_index.end += motion_bone_count;

        io_position_push(s, parent_indices_offset, SEEK_SET);
        io_read(s, skel->parent_index.begin, motion_bone_count * sizeof(uint16_t));
        io_position_pop(s);
        io_position_pop(s);
    }
    io_position_pop(s);

    io_position_push(s, skeleton_name_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skeleton = &bone_data->skeleton.begin[i];
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

    uint32_t skeleton_count = (uint32_t)(bone_data->skeleton.end - bone_data->skeleton.begin);
    ssize_t skeleton_offsets_offset = io_get_position(s);
    io_write(s, 0, skeleton_count * 0x04ULL);
    
    ssize_t skeleton_name_offset = io_get_position(s);
    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
        io_write_string_null_terminated(s, &i->name);

    ssize_t skeleton_name_offsets_offset = io_get_position(s);
    for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++) {
        io_write_uint32_t(s, (uint32_t)skeleton_name_offset);
        skeleton_name_offset += i->name.length + 1;
    }

    vector_string strings = vector_empty(string);
    vector_ssize_t string_offsets = vector_empty(ssize_t);

    ssize_t* skeleton_offsets = force_malloc_s(ssize_t, skeleton_count);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

        uint32_t bone_count = (uint32_t)(skel->bone.end - skel->bone.begin);
        uint32_t position_count = (uint32_t)(skel->position.end - skel->position.begin);
        uint32_t object_bone_count = (uint32_t)(skel->object_bone.end - skel->object_bone.begin);
        uint32_t motion_bone_count = (uint32_t)(skel->motion_bone.end - skel->motion_bone.begin);

        vector_string_reserve(&strings, bone_count);
        vector_ssize_t_reserve(&string_offsets, bone_count);

        for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++)
            if (bone_database_strings_push_back_check(&strings, string_data(&j->name))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
                io_write_string_null_terminated(s, &j->name);
            }

        if (bone_database_strings_push_back_check(&strings, "End")) {
            *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
            io_write_utf8_string_null_terminated(s, "End");
        }
        io_align_write(s, 0x04);

        ssize_t bones_offset = io_get_position(s);
        for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++) {
            io_write_uint8_t(s, (uint8_t)j->type);
            io_write_uint8_t(s, j->has_parent ? 1 : 0);
            io_write_uint8_t(s, j->parent);
            io_write_uint8_t(s, j->pole_target);
            io_write_uint8_t(s, j->mirror);
            io_write_uint8_t(s, j->flags);
            io_write(s, 0, 0x02);
            io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&j->name)));
        }

        io_write_uint32_t(s, 0xFF);
        io_write_uint32_t(s, 0xFF);
        io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(&strings,
            &string_offsets, "End"));

        ssize_t positions_offset = io_get_position(s);
        io_write(s, skel->position.begin, sizeof(vec3) * position_count);

        ssize_t unknown_value_offset = io_get_position(s);
        io_write_float_t(s, skel->heel_height);

        for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
            if (bone_database_strings_push_back_check(&strings, string_data(j))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
                io_write_string_null_terminated(s, j);
            }
        io_align_write(s, 0x04);

        ssize_t object_bone_names_offset = io_get_position(s);
        for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
            io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(j)));

        for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
            if (bone_database_strings_push_back_check(&strings, string_data(j))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(s);
                io_write_string_null_terminated(s, j);
            }
        io_align_write(s, 0x04);

        ssize_t motion_bone_names_offset = io_get_position(s);
        for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
            io_write_uint32_t(s, (uint32_t)bone_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(j)));

        ssize_t parent_indices_offset = io_get_position(s);
        io_write(s, skel->parent_index.begin, sizeof(uint16_t) * motion_bone_count);
        io_align_write(s, 0x04);

        skeleton_offsets[skel - bone_data->skeleton.begin] = io_get_position(s);
        io_write_uint32_t(s, (uint32_t)bones_offset);
        io_write_uint32_t(s, position_count);
        io_write_uint32_t(s, (uint32_t)positions_offset);
        io_write_uint32_t(s, (uint32_t)unknown_value_offset);
        io_write_uint32_t(s, object_bone_count);
        io_write_uint32_t(s, (uint32_t)object_bone_names_offset);
        io_write_uint32_t(s, motion_bone_count);
        io_write_uint32_t(s, (uint32_t)motion_bone_names_offset);
        io_write_uint32_t(s, (uint32_t)parent_indices_offset);
        io_write(s, 0, 0x14);

        vector_string_clear(&strings, string_free);
        vector_ssize_t_clear(&string_offsets, 0);
    }

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, 0x09102720);
    io_write_uint32_t(s, skeleton_count);
    io_write_uint32_t(s, (uint32_t)skeleton_offsets_offset);
    io_write_uint32_t(s, (uint32_t)skeleton_name_offsets_offset);
    io_write(s, 0, 0x14);
    io_position_pop(s);

    io_position_push(s, skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++)
        io_write_uint32_t(s, (uint32_t)skeleton_offsets[i]);
    io_position_pop(s);

    vector_string_free(&strings, string_free);
    vector_ssize_t_free(&string_offsets, 0);
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

    vector_bone_database_skeleton_reserve(&bone_data->skeleton, skeleton_count);
    bone_data->skeleton.end += skeleton_count;

    io_position_push(s, skeleton_offsets_offset, SEEK_SET);
    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skeleton = &bone_data->skeleton.begin[i];
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
            io_read(s, 0, 0x14);
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
            io_read(s, 0, 0x28);
        }

        uint32_t bone_count = 0;
        io_position_push(s, bones_offset, SEEK_SET);
        if (!is_x)
            while (true) {
                if (io_read_uint8_t(s) == 0xFF)
                    break;

                io_read(s, 0, 0x0B);
                bone_count++;
            }
        else
            while (true) {
                if (io_read_uint8_t(s) == 0xFF)
                    break;

                io_read(s, 0, 0x0F);
                bone_count++;
            }
        io_position_pop(s);

        vector_bone_database_bone_reserve(&skeleton->bone, bone_count);
        skeleton->bone.end += bone_count;

        io_position_push(s, bones_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < bone_count; j++) {
                bone_database_bone* bone = &skeleton->bone.begin[j];
                bone->type = io_read_uint8_t(s);
                bone->has_parent = io_read_uint8_t(s) ? true : false;
                bone->parent = io_read_uint8_t(s);
                bone->pole_target = io_read_uint8_t(s);
                bone->mirror = io_read_uint8_t(s);
                bone->flags = io_read_uint8_t(s);
                io_read(s, 0, 0x02);
                io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &bone->name);
            }
        else
            for (uint32_t j = 0; j < bone_count; j++) {
                bone_database_bone* bone = &skeleton->bone.begin[j];
                bone->type = io_read_uint8_t(s);
                bone->has_parent = io_read_uint8_t(s) ? true : false;
                bone->parent = io_read_uint8_t(s);
                bone->pole_target = io_read_uint8_t(s);
                bone->mirror = io_read_uint8_t(s);
                bone->flags = io_read_uint8_t(s);
                io_read(s, 0, 0x02);
                io_read_string_null_terminated_offset(s, io_read_offset_x(s), &bone->name);
            }
        io_position_pop(s);

        vector_vec3_reserve(&skeleton->position, position_count);
        skeleton->position.end += position_count;

        io_position_push(s, positions_offset, SEEK_SET);
        for (uint32_t j = 0; j < position_count; j++) {
            vec3* position = &skeleton->position.begin[j];
            position->x = io_read_float_t_stream_reverse_endianness(s);
            position->y = io_read_float_t_stream_reverse_endianness(s);
            position->z = io_read_float_t_stream_reverse_endianness(s);
        }
        io_position_pop(s);

        io_position_push(s, unknown_value_offset, SEEK_SET);
        skeleton->heel_height = io_read_float_t_stream_reverse_endianness(s);
        io_position_pop(s);

        vector_string_reserve(&skeleton->object_bone, object_bone_count);
        skeleton->object_bone.end += object_bone_count;

        io_position_push(s, object_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < object_bone_count; j++) {
                string* object_bone_name = &skeleton->object_bone.begin[j];
                io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), object_bone_name);
            }
        else
            for (uint32_t j = 0; j < object_bone_count; j++) {
                string* object_bone_name = &skeleton->object_bone.begin[j];
                io_read_string_null_terminated_offset(s, io_read_offset_x(s), object_bone_name);
            }
        io_position_pop(s);

        vector_string_reserve(&skeleton->motion_bone, motion_bone_count);
        skeleton->motion_bone.end += motion_bone_count;

        io_position_push(s, motion_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t j = 0; j < motion_bone_count; j++) {
                string* motion_bone_name = &skeleton->motion_bone.begin[j];
                io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), motion_bone_name);
            }
        else
            for (uint32_t j = 0; j < motion_bone_count; j++) {
                string* motion_bone_name = &skeleton->motion_bone.begin[j];
                io_read_string_null_terminated_offset(s, io_read_offset_x(s), motion_bone_name);
            }
        io_position_pop(s);

        vector_uint16_t_reserve(&skeleton->parent_index, motion_bone_count);
        skeleton->parent_index.end += motion_bone_count;
        
        io_position_push(s, parent_indices_offset, SEEK_SET);
        io_read(s, skeleton->parent_index.begin, motion_bone_count * sizeof(uint16_t));
        if (s->is_big_endian) {
            uint16_t* parent_indices = skeleton->parent_index.begin;
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
            bone_database_skeleton* skeleton = &bone_data->skeleton.begin[i];
            io_read_string_null_terminated_offset(s, io_read_offset_f2(s, header_length), &skeleton->name);
        }
    else
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skeleton = &bone_data->skeleton.begin[i];
            io_read_string_null_terminated_offset(s, io_read_offset_x(s), &skeleton->name);
        }
    io_position_pop(s);

    bone_data->is_x = is_x;
    bone_data->modern = true;
    bone_data->ready = true;
}

static void bone_database_modern_write_inner(bone_database* bone_data, stream* s) {
    stream s_bone;
    io_mopen(&s_bone, 0, 0);
    uint32_t off;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;
    vector_size_t pof = vector_empty(size_t);

    bool is_x = bone_data->is_x;

    uint32_t skeleton_count = (uint32_t)(bone_data->skeleton.end - bone_data->skeleton.begin);

    if (!is_x) {
        uint32_t pos;
        ee = (enrs_entry){ 0, 1, 16, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        pos = off = 16;

        skeleton_count *= 2;
        ee = (enrs_entry){ off, 1, (uint32_t)(skeleton_count * 4ULL), 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, skeleton_count, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        pos += off = (uint32_t)(skeleton_count * 4ULL);
        skeleton_count /= 2;

        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

            uint32_t bone_count = (uint32_t)(skel->bone.end - skel->bone.begin);
            uint32_t position_count = (uint32_t)(skel->position.end - skel->position.begin);
            uint32_t object_bone_count = (uint32_t)(skel->object_bone.end - skel->object_bone.begin);
            uint32_t motion_bone_count = (uint32_t)(skel->motion_bone.end - skel->motion_bone.begin);

            ee = (enrs_entry){ off, 1, 56, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 14, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            pos += off = 56;

            bone_count++;
            off += 8;
            pos += 8;
            ee = (enrs_entry){ off, 1, 12, bone_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(bone_count * 12ULL);
            if (pos + off % 0x10)
                off -= 8;
            pos += off = align_val(pos + off, 0x10) - pos;
            bone_count--;

            ee = (enrs_entry){ off, 1, 12, position_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(position_count * 12ULL);
            pos += off = align_val(off, 0x10);

            ee = (enrs_entry){ off, 1, 4, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            pos += off = 4;

            object_bone_count += motion_bone_count;
            ee = (enrs_entry){ off, 1, (uint32_t)(object_bone_count * 4ULL), 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, object_bone_count, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            pos += off = (uint32_t)(object_bone_count * 4ULL);
            object_bone_count -= motion_bone_count;

            ee = (enrs_entry){ off, 1, (uint32_t)(motion_bone_count * 2ULL), 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, motion_bone_count, ENRS_WORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(motion_bone_count * 2ULL);
            pos += off = align_val(off, 0x04);
        }
    }
    else {
        ee = (enrs_entry){ 0, 2, 24, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_QWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 24;
        off = align_val(off, 0x10);

        if (skeleton_count % 2) {
            ee = (enrs_entry){ off, 1, (uint32_t)(skeleton_count * 8ULL), 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, skeleton_count, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);

            ee = (enrs_entry){ off, 1, (uint32_t)(skeleton_count * 8ULL), 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, skeleton_count, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);
        }
        else {
            skeleton_count *= 2;
            ee = (enrs_entry){ off, 1, (uint32_t)(skeleton_count * 8ULL), 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, skeleton_count, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(skeleton_count * 8ULL);
            off = align_val(off, 0x10);
            skeleton_count /= 2;
        }

        ee = (enrs_entry){ off, 7, 112, skeleton_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 7, ENRS_QWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(skeleton_count * 112ULL);
        off = align_val(off, 0x10);

        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

            uint32_t bone_count = (uint32_t)(skel->bone.end - skel->bone.begin);
            uint32_t position_count = (uint32_t)(skel->position.end - skel->position.begin);
            uint32_t object_bone_count = (uint32_t)(skel->object_bone.end - skel->object_bone.begin);
            uint32_t motion_bone_count = (uint32_t)(skel->motion_bone.end - skel->motion_bone.begin);

            bone_count++;
            off += 8;
            ee = (enrs_entry){ off, 1, 16, bone_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(bone_count * 16ULL);
            bone_count--;

            ee = (enrs_entry){ off, 1, 12, position_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(position_count * 12ULL);
            off = align_val(off, 0x10);

            ee = (enrs_entry){ off, 1, 4, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = 4;
            off = align_val(off, 0x10);

            if (object_bone_count % 1) {
                ee = (enrs_entry){ off, 1, 12, position_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(position_count * 12ULL);
                off = align_val(off, 0x10);
            }

            if (skeleton_count % 2) {
                ee = (enrs_entry){ off, 1, (uint32_t)(object_bone_count * 8ULL), 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, object_bone_count, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(object_bone_count * 8ULL);
                off = align_val(off, 0x10);

                ee = (enrs_entry){ off, 1, (uint32_t)(motion_bone_count * 8ULL), 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, motion_bone_count, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(motion_bone_count * 8ULL);
                off = align_val(off, 0x10);
            }
            else {
                object_bone_count += motion_bone_count;
                ee = (enrs_entry){ off, 1, (uint32_t)(object_bone_count * 8ULL), 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, object_bone_count, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(object_bone_count * 8ULL);
                off = align_val(off, 0x10);
                object_bone_count -= motion_bone_count;
            }

            ee = (enrs_entry){ off, 1, (uint32_t)(motion_bone_count * 2ULL), 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, motion_bone_count, ENRS_WORD });
            vector_enrs_entry_push_back(&e, &ee);
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
        for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
    else {
        for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
        io_align_write(&s_bone, 0x10);
    }

    ssize_t skeleton_name_offsets_offset = io_get_position(&s_bone);
    if (!is_x)
        for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
    else {
        for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
        io_align_write(&s_bone, 0x10);
    }

    bone_database_skeleton_header* skh = force_malloc_s(bone_database_skeleton_header, skeleton_count);

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

            uint32_t bone_count = (uint32_t)(skel->bone.end - skel->bone.begin);
            uint32_t position_count = (uint32_t)(skel->position.end - skel->position.begin);
            uint32_t object_bone_count = (uint32_t)(skel->object_bone.end - skel->object_bone.begin);
            uint32_t motion_bone_count = (uint32_t)(skel->motion_bone.end - skel->motion_bone.begin);

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
            io_write(&s_bone, 0, 0x14);

            skh[i].bones_offset = io_get_position(&s_bone);
            for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++) {
                io_write(&s_bone, 0, 0x08);
                io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            }

            io_write(&s_bone, 0, 0x08);
            io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].positions_offset = io_get_position(&s_bone);
            io_write(&s_bone, 0, sizeof(vec3) * position_count);
            io_align_write(&s_bone, 0x10);

            skh[i].unknown_value_offset = io_get_position(&s_bone);
            io_write_float_t(&s_bone, 0.0f);

            skh[i].object_bone_names_offset = io_get_position(&s_bone);
            for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
                io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);

            skh[i].motion_bone_names_offset = io_get_position(&s_bone);
            for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
                io_write_offset_f2_pof_add(&s_bone, 0, 0x40, &pof);

            skh[i].parent_indices_offset = io_get_position(&s_bone);
            io_write(&s_bone, 0, sizeof(uint16_t) * motion_bone_count);
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
            io_write(&s_bone, 0, 0x28);
            io_align_write(&s_bone, 0x10);
        }

        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

            uint32_t bone_count = (uint32_t)(skel->bone.end - skel->bone.begin);
            uint32_t position_count = (uint32_t)(skel->position.end - skel->position.begin);
            uint32_t object_bone_count = (uint32_t)(skel->object_bone.end - skel->object_bone.begin);
            uint32_t motion_bone_count = (uint32_t)(skel->motion_bone.end - skel->motion_bone.begin);


            skh[i].bones_offset = io_get_position(&s_bone);
            for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++) {
                io_write(&s_bone, 0, 0x08);
                io_write_offset_x_pof_add(&s_bone, 0, &pof);
            }

            io_write(&s_bone, 0, 0x08);
            io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].positions_offset = io_get_position(&s_bone);
            io_write(&s_bone, 0, sizeof(vec3) * position_count);
            io_align_write(&s_bone, 0x10);

            skh[i].unknown_value_offset = io_get_position(&s_bone);
            io_write_float_t(&s_bone, 0.0f);
            io_align_write(&s_bone, 0x10);

            skh[i].object_bone_names_offset = io_get_position(&s_bone);
            for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
                io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].motion_bone_names_offset = io_get_position(&s_bone);
            for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
                io_write_offset_x_pof_add(&s_bone, 0, &pof);
            io_align_write(&s_bone, 0x10);

            skh[i].parent_indices_offset = io_get_position(&s_bone);
            io_write(&s_bone, 0, sizeof(uint16_t) * motion_bone_count);
            io_align_write(&s_bone, 0x10);
        }
    }

    vector_string strings = vector_empty(string);
    vector_ssize_t string_offsets = vector_empty(ssize_t);

    if (is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

            if (bone_database_strings_push_back_check(&strings, string_data(&skel->name))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_bone);
                io_write_utf8_string_null_terminated(&s_bone, string_data(&skel->name));
            }
        }

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

        for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++)
            if (bone_database_strings_push_back_check(&strings, string_data(&j->name))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_bone);
                io_write_string_null_terminated(&s_bone, &j->name);
            }

        if (bone_database_strings_push_back_check(&strings, "End")) {
            *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_bone);
            io_write_utf8_string_null_terminated(&s_bone, "End");
        }

        for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
            if (bone_database_strings_push_back_check(&strings, string_data(j))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_bone);
                io_write_string_null_terminated(&s_bone, j);
            }

        for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
            if (bone_database_strings_push_back_check(&strings, string_data(j))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_bone);
                io_write_string_null_terminated(&s_bone, j);
            }
    }

    if (!is_x)
        for (uint32_t i = 0; i < skeleton_count; i++) {
            bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

            if (bone_database_strings_push_back_check(&strings, string_data(&skel->name))) {
                *vector_ssize_t_reserve_back(&string_offsets) = io_get_position(&s_bone);
                io_write_utf8_string_null_terminated(&s_bone, string_data(&skel->name));
            }
        }
    io_align_write(&s_bone, 0x10);

    for (uint32_t i = 0; i < skeleton_count; i++) {
        bone_database_skeleton* skel = &bone_data->skeleton.begin[i];

        uint32_t bone_count = (uint32_t)(skel->bone.end - skel->bone.begin);
        uint32_t position_count = (uint32_t)(skel->position.end - skel->position.begin);
        uint32_t object_bone_count = (uint32_t)(skel->object_bone.end - skel->object_bone.begin);
        uint32_t motion_bone_count = (uint32_t)(skel->motion_bone.end - skel->motion_bone.begin);

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
            io_write(&s_bone, 0, 0x14);
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
            io_write(&s_bone, 0, 0x28);
        }
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].bones_offset, SEEK_SET);
        if (!is_x)
            for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++) {
                io_write_uint8_t(&s_bone, (uint8_t)j->type);
                io_write_uint8_t(&s_bone, j->has_parent ? 1 : 0);
                io_write_uint8_t(&s_bone, j->parent);
                io_write_uint8_t(&s_bone, j->pole_target);
                io_write_uint8_t(&s_bone, j->mirror);
                io_write_uint8_t(&s_bone, j->flags);
                io_write(&s_bone, 0, 0x02);
                io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(&j->name)), 0x40);
            }
        else
            for (bone_database_bone* j = skel->bone.begin; j != skel->bone.end; j++) {
                io_write_uint8_t(&s_bone, (uint8_t)j->type);
                io_write_uint8_t(&s_bone, j->has_parent ? 1 : 0);
                io_write_uint8_t(&s_bone, j->parent);
                io_write_uint8_t(&s_bone, j->pole_target);
                io_write_uint8_t(&s_bone, j->mirror);
                io_write_uint8_t(&s_bone, j->flags);
                io_write(&s_bone, 0, 0x02);
                io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(&j->name)));
            }

        if (!is_x) {
            io_write_uint32_t(&s_bone, 0xFF);
            io_write_uint32_t(&s_bone, 0xFF);
            io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(&strings,
                &string_offsets, "End"), 0x40);
        }
        else {

            io_write_uint32_t(&s_bone, 0xFF);
            io_write_uint32_t(&s_bone, 0xFF);
            io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(&strings,
                &string_offsets, "End"));
        }
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].positions_offset, SEEK_SET);
        io_write(&s_bone, skel->position.begin, sizeof(vec3)* position_count);
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].unknown_value_offset, SEEK_SET);
        io_write_float_t(&s_bone, skel->heel_height);
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].object_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
                io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(j)), 0x40);
        else
            for (string* j = skel->object_bone.begin; j != skel->object_bone.end; j++)
                io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(j)));

        io_position_push(&s_bone, skh[i].motion_bone_names_offset, SEEK_SET);
        if (!is_x)
            for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
                io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(j)), 0x40);
        else
            for (string* j = skel->motion_bone.begin; j != skel->motion_bone.end; j++)
                io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(&strings,
                    &string_offsets, string_data(j)));
        io_position_pop(&s_bone);

        io_position_push(&s_bone, skh[i].parent_indices_offset, SEEK_SET);
        io_write(&s_bone, skel->parent_index.begin, sizeof(uint16_t) * motion_bone_count);
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
        for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
            io_write_offset_f2(&s_bone, bone_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->name)), 0x40);
    else
        for (bone_database_skeleton* i = bone_data->skeleton.begin; i != bone_data->skeleton.end; i++)
            io_write_offset_x(&s_bone, bone_database_strings_get_string_offset(&strings,
                &string_offsets, string_data(&i->name)));
    io_position_pop(&s_bone);

    vector_string_free(&strings, string_free);
    vector_ssize_t_free(&string_offsets, 0);
    free(skh);

    vector_string_free(&strings, string_free);
    vector_ssize_t_free(&string_offsets, 0);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    io_align_write(&s_bone, 0x10);
    io_mcopy(&s_bone, &st.data, &st.length);
    io_free(&s_bone);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('BONE');
    st.header.length = is_x ? 0x20 : 0x40;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    f2_struct_swrite(&st, s, true, is_x);
    f2_struct_free(&st);
}

inline static ssize_t bone_database_strings_get_string_offset(vector_string* vec,
    vector_ssize_t* vec_off, char* str) {
    size_t len = utf8_length(str);
    for (string* i = vec->begin; i != vec->end; i++)
        if (!memcmp(str, string_data(i), min(len, i->length) + 1))
            return vec_off->begin[i - vec->begin];
    return 0;
}

inline static bool bone_database_strings_push_back_check(vector_string* vec, char* str) {
    size_t len = utf8_length(str);
    for (string* i = vec->begin; i != vec->end; i++)
        if (!memcmp(str, string_data(i), min(len, i->length) + 1))
            return false;

    string* s = vector_string_reserve_back(vec);
    string_init_length(s, str, len);
    return true;
}
