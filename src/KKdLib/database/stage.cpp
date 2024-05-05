/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.hpp"
#include "../f2/struct.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../hash.hpp"
#include "../str_utils.hpp"

static void stage_database_file_classic_read_inner(stage_database_file* stage_data, stream& s);
static void stage_database_file_classic_write_inner(stage_database_file* stage_data, stream& s);
static void stage_database_file_modern_read_inner(stage_database_file* stage_data, stream& s, uint32_t header_length);
static void stage_database_file_modern_write_inner(stage_database_file* stage_data, stream& s);
static int64_t stage_database_file_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str);
static bool stage_database_file_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str);

const stage_effects stage_effects_default;
const stage_effects_modern stage_effects_modern_default;

stage_effects::stage_effects() : field_0(), field_20() {
    field_0[0] = -1;
    field_20[0] = -1;
}

stage_effects_modern::stage_effects_modern() {
    field_0[0] = -1;
    field_20[0] = -1;
}

stage_data_file::stage_data_file() : id(), object_set_id(), lens_flare_texture(), lens_shaft_texture(),
lens_ghost_texture(), unknown(), render_texture(), movie_texture(), reflect_type(), refract_enable(),
reflect(), reflect_data(), refract(), refract_data(), flags(), ring_rectangle_x(), ring_rectangle_y(),
ring_rectangle_width(), ring_rectangle_height(), ring_height(), ring_out_height(), effects_init() {
    lens_shaft_inv_scale = 1.0f;
}

stage_data_file::~stage_data_file() {

}

stage_data_modern_file::stage_data_modern_file() : unknown(), render_texture(),
render_texture_flag(), movie_texture(), movie_texture_flag(), field_04(), field_04_flag(), field_05(),
field_05_flag(), field_06(), field_06_flag(), field_07(), field_07_flag(), field_08(), field_09(),
field_10(), field_11(), field_12(), ring_rectangle_x(), ring_rectangle_y(), ring_rectangle_width(),
ring_rectangle_height(), ring_height(), ring_out_height(), field_13(), effects_init() {
    hash = hash_murmurhash_empty;
    lens_shaft_inv_scale = 1.0f;
}

stage_data_modern_file::~stage_data_modern_file() {

}

stage_data::stage_data() : id(), object_set_id(), lens_flare_texture(), lens_shaft_texture(),
lens_ghost_texture(), unknown(), render_texture(), movie_texture(), reflect_type(), refract_enable(),
reflect(), reflect_data(), refract(), refract_data(), flags(), ring_rectangle_x(), ring_rectangle_y(),
ring_rectangle_width(), ring_rectangle_height(), ring_height(), ring_out_height(), effects_init() {
    name_hash = hash_murmurhash_empty;
    lens_shaft_inv_scale = 1.0f;
}

stage_data::~stage_data() {

}

stage_data_modern::stage_data_modern() : unknown(), render_texture(),
render_texture_flag(), movie_texture(), movie_texture_flag(), field_04(), field_04_flag(), field_05(),
field_05_flag(), field_06(), field_06_flag(), field_07(), field_07_flag(), field_08(), field_09(),
field_10(), field_11(), field_12(), ring_rectangle_x(), ring_rectangle_y(), ring_rectangle_width(),
ring_rectangle_height(), ring_height(), ring_out_height(), field_13(), effects_init() {
    hash = hash_murmurhash_empty;
    auth_3d_name_hash = hash_murmurhash_empty;
    lens_shaft_inv_scale = 1.0f;
}

stage_data_modern::~stage_data_modern() {

}

stage_database_file::stage_database_file() : ready(), modern(), big_endian(), is_x(), format() {

}

stage_database_file::~stage_database_file() {

}

void stage_database_file::read(const char* path, bool modern) {
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
                stage_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        char* path_stg = str_utils_add(path, ".stg");
        if (path_check_file_exists(path_stg)) {
            f2_struct st;
            st.read(path_stg);
            if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
                memory_stream s_stgc;
                s_stgc.open(st.data);
                s_stgc.big_endian = st.header.use_big_endian;
                stage_database_file_modern_read_inner(this, s_stgc, st.header.length);
            }
        }
        free_def(path_stg);
    }
}

void stage_database_file::read(const wchar_t* path, bool modern) {
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
                stage_database_file_classic_read_inner(this, s_bin);
                free_def(data);
            }
        }
        free_def(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_add(path, L".stg");
        if (path_check_file_exists(path_stg)) {
            f2_struct st;
            st.read(path_stg);
            if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
                memory_stream s_stgc;
                s_stgc.open(st.data);
                s_stgc.big_endian = st.header.use_big_endian;
                stage_database_file_modern_read_inner(this, s_stgc, st.header.length);
            }
        }
        free_def(path_stg);
    }
}

void stage_database_file::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        memory_stream s;
        s.open(data, size);
        stage_database_file_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
            memory_stream s_stgc;
            s_stgc.open(st.data);
            s_stgc.big_endian = st.header.use_big_endian;
            stage_database_file_modern_read_inner(this, s_stgc, st.header.length);
        }
    }
}

void stage_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        file_stream s;
        s.open(path_bin, "wb");
        if (s.check_not_null())
            stage_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        char* path_stg = str_utils_add(path, ".stg");
        file_stream s;
        s.open(path_stg, "wb");
        if (s.check_not_null())
            stage_database_file_modern_write_inner(this, s);
        free_def(path_stg);
    }
}

void stage_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        file_stream s;
        s.open(path_bin, L"wb");
        if (s.check_not_null())
            stage_database_file_classic_write_inner(this, s);
        free_def(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_add(path, L".stg");
        file_stream s;
        s.open(path_stg, L"wb");
        if (s.check_not_null())
            stage_database_file_modern_write_inner(this, s);
        free_def(path_stg);
    }
}

void stage_database_file::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        stage_database_file_classic_write_inner(this, s);
    else
        stage_database_file_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool stage_database_file::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    stage_database_file* stage_data = (stage_database_file*)data;
    stage_data->read(path.c_str(), stage_data->modern);

    return stage_data->ready;
}

stage_database::stage_database() {

}

stage_database::~stage_database() {

}

void stage_database::add(stage_database_file* stage_data_file) {
    if (!stage_data_file || !stage_data_file->ready)
        return;

    if (stage_data.capacity() - stage_data.size() < stage_data_file->stage_data.size())
        stage_data.reserve(stage_data_file->stage_data.size());

    for (::stage_data_file i : stage_data_file->stage_data) {
        uint32_t name_hash = hash_string_murmurhash(i.name);

        ::stage_data* data = 0;
        for (::stage_data& j : stage_data)
            if (name_hash == j.name_hash) {
                data = &j;
                break;
            }

        if (!data) {
            stage_data.push_back({});
            data = &stage_data.back();
        }

        data->id = i.id;
        data->name.assign(i.name);
        data->name_hash = hash_string_murmurhash(data->name);
        data->auth_3d_name.assign(i.auth_3d_name);
        data->object_set_id = i.object_set_id;
        data->object_ground = i.object_ground;
        data->object_ring = i.object_ring;
        data->object_sky = i.object_sky;
        data->object_shadow = i.object_shadow;
        data->object_reflect = i.object_reflect;
        data->object_refract = i.object_refract;
        data->lens_flare_texture = i.lens_flare_texture;
        data->lens_shaft_texture = i.lens_shaft_texture;
        data->lens_ghost_texture = i.lens_ghost_texture;
        data->lens_shaft_inv_scale = i.lens_shaft_inv_scale;
        data->unknown = i.unknown;
        data->render_texture = i.render_texture;
        data->movie_texture = i.movie_texture;
        data->collision_file_path = i.collision_file_path;
        data->reflect_type = i.reflect_type;
        data->refract_enable = i.refract_enable;
        data->reflect = i.reflect;
        data->reflect_data = i.reflect_data;
        data->refract = i.refract;
        data->refract_data = i.refract_data;
        data->flags = i.flags;
        data->ring_rectangle_x = i.ring_rectangle_x;
        data->ring_rectangle_y = i.ring_rectangle_y;
        data->ring_rectangle_width = i.ring_rectangle_width;
        data->ring_rectangle_height = i.ring_rectangle_height;
        data->ring_height = i.ring_height;
        data->ring_out_height = i.ring_out_height;
        data->effects = i.effects;
        data->effects_init = i.effects_init;
        data->auth_3d_ids.assign(i.auth_3d_ids.begin(), i.auth_3d_ids.end());
    }

    for (stage_data_modern_file& i : stage_data_file->stage_modern) {
        uint32_t hash = i.hash;

        stage_data_modern* data = 0;
        for (stage_data_modern& j : stage_modern)
            if (hash == j.hash) {
                data = &j;
                break;
            }

        if (!data) {
            stage_modern.push_back({});
            data = &stage_modern.back();
        }

        data->hash = i.hash;
        data->name.assign(i.name);
        data->auth_3d_name.assign(i.auth_3d_name);
        data->auth_3d_name_hash = hash_string_murmurhash(data->auth_3d_name);
        data->object_ground = i.object_ground;
        data->object_sky = i.object_sky;
        data->object_shadow = i.object_shadow;
        data->object_reflect = i.object_reflect;
        data->object_refract = i.object_refract;
        data->lens_shaft_inv_scale = i.lens_shaft_inv_scale;
        data->unknown = i.unknown;
        data->render_texture = i.render_texture;
        data->render_texture_flag = i.render_texture_flag;
        data->movie_texture = i.movie_texture;
        data->movie_texture_flag = i.movie_texture_flag;
        data->field_04 = i.field_04;
        data->field_04_flag = i.field_04_flag;
        data->field_05 = i.field_05;
        data->field_05_flag = i.field_05_flag;
        data->field_06 = i.field_06;
        data->field_06_flag = i.field_06_flag;
        data->field_07 = i.field_07;
        data->field_07_flag = i.field_07_flag;
        data->field_08 = i.field_08;
        data->field_09 = i.field_09;
        data->field_10 = i.field_10;
        data->field_11 = i.field_11;
        data->field_12 = i.field_12;
        data->ring_rectangle_x = i.ring_rectangle_x;
        data->ring_rectangle_y = i.ring_rectangle_y;
        data->ring_rectangle_width = i.ring_rectangle_width;
        data->ring_rectangle_height = i.ring_rectangle_height;
        data->ring_height = i.ring_height;
        data->ring_out_height = i.ring_out_height;
        data->field_13 = i.field_13;
        data->effects = i.effects;
        data->effects_init = i.effects_init;
        data->auth_3d_ids.assign(i.auth_3d_ids.begin(), i.auth_3d_ids.end());
    }

    update();
}

void stage_database::clear() {
    stage_data.clear();
    stage_data.shrink_to_fit();
    stage_modern.clear();
    stage_modern.shrink_to_fit();
    stage_data_names.clear();
    stage_data_names.shrink_to_fit();
    stage_modern_names.clear();
    stage_modern_names.shrink_to_fit();
}

void stage_database::update() {
    stage_data_names.clear();
    stage_modern_names.clear();

    for (::stage_data& i : stage_data)
        stage_data_names.push_back(i.name_hash, &i);

    for (stage_data_modern& i : stage_modern)
        stage_modern_names.push_back(i.hash, &i);

    stage_data_names.sort_unique();
    stage_modern_names.sort_unique();
}

const ::stage_data* stage_database::get_stage_data(int32_t stage_index) const {
    if (stage_index >= 0 && stage_index < stage_data.size())
        return &stage_data[stage_index];
    return 0;
}

const ::stage_data_modern* stage_database::get_stage_data_modern(uint32_t stage_hash) const {
    auto elem = stage_modern_names.find(stage_hash);
    if (elem != stage_modern_names.end())
        return elem->second;
    return 0;
}

int32_t stage_database::get_stage_index(const char* name) const {
    auto elem = stage_data_names.find(hash_utf8_murmurhash(name));
    if (elem != stage_data_names.end())
        return (int32_t)(elem->second - stage_data.data());
    return -1;
}

const char* stage_database::get_stage_name(int32_t stage_index) const {
    if (stage_index >= 0 && stage_index < stage_data.size())
        return stage_data[stage_index].name.c_str();
    return 0;
}

const char* stage_database::get_stage_name_modern(uint32_t stage_hash) const {
    auto elem = stage_modern_names.find(stage_hash);
    if (elem != stage_modern_names.end())
        return elem->second->name.c_str();
    return 0;
}

static void stage_database_file_classic_read_inner(stage_database_file* stage_data, stream& s) {
    int32_t count = s.read_int32_t();
    int32_t stages_offset = s.read_int32_t();
    int32_t stage_effects_offset = s.read_int32_t();
    int32_t auth3d_id_counts_offset = s.read_int32_t();
    int32_t auth_3d_ids_offsets_offset = s.read_int32_t();

    int32_t size = (stage_effects_offset - stages_offset) / count;
    if (size >= 0x68 && size < 0x6C)
        stage_data->format = STAGE_DATA_AC;
    else if (size >= 0x6C && size < 0x70)
        stage_data->format = STAGE_DATA_F;
    else if (size >= 0x70 && size < 0x74)
        stage_data->format = STAGE_DATA_FT;
    else {
        stage_data->ready = false;
        stage_data->modern = false;
        stage_data->big_endian = false;
        stage_data->is_x = false;
        stage_data->format = STAGE_DATA_UNK;
        return;
    }

    stage_data->stage_data.resize(count);

    stage_data_file* _stage_data = stage_data->stage_data.data();

    s.position_push(stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_file* stage = &_stage_data[i];

        stage->id = i;
        stage->name = s.read_string_null_terminated_offset(s.read_uint32_t());
        stage->auth_3d_name = s.read_string_null_terminated_offset(s.read_uint32_t());
        stage->object_set_id = (uint16_t)s.read_uint32_t(); // Crutch to remove higher bits
        stage->object_ground.id = s.read_uint16_t();
        stage->object_ground.set_id = s.read_uint16_t();
        stage->object_ring.id = s.read_uint16_t();
        stage->object_ring.set_id = s.read_uint16_t();
        stage->object_sky.id = s.read_uint16_t();
        stage->object_sky.set_id = s.read_uint16_t();
        stage->object_shadow.id = s.read_uint16_t();
        stage->object_shadow.set_id = s.read_uint16_t();
        stage->object_reflect.id = s.read_uint16_t();
        stage->object_reflect.set_id = s.read_uint16_t();
        stage->object_refract.id = s.read_uint16_t();
        stage->object_refract.set_id = s.read_uint16_t();
        stage->lens_flare_texture = s.read_uint32_t();
        stage->lens_shaft_texture = s.read_uint32_t();
        stage->lens_ghost_texture = s.read_uint32_t();
        stage->lens_shaft_inv_scale = s.read_float_t();
        stage->unknown = s.read_uint32_t();
        stage->render_texture = s.read_uint32_t();

        if (stage_data->format > STAGE_DATA_AC)
            stage->movie_texture = s.read_uint32_t();
        else
            stage->movie_texture = -1;

        stage->collision_file_path = s.read_string_null_terminated_offset(s.read_uint32_t());
        stage->reflect_type = (stage_data_reflect_type)s.read_uint32_t();
        stage->refract_enable = s.read_uint32_t() ? true : false;

        int32_t reflect_offset = s.read_uint32_t();
        if (reflect_offset) {
            s.position_push(reflect_offset, SEEK_SET);
            stage->reflect = true;
            stage->reflect_data.mode = (stage_data_reflect_resolution_mode)s.read_uint32_t();
            stage->reflect_data.blur_num = s.read_uint32_t();
            stage->reflect_data.blur_filter = (stage_data_blur_filter_mode)s.read_uint32_t();
            s.position_pop();
        }
        else
            stage->reflect = false;

        int32_t refract_offset = s.read_uint32_t();
        if (refract_offset) {
            s.position_push(refract_offset, SEEK_SET);
            stage->refract = true;
            stage->refract_data.mode = (stage_data_refract_resolution_mode)s.read_uint32_t();
            s.position_pop();
        }
        else
            stage->refract = false;

        if (stage_data->format > STAGE_DATA_F)
            stage->flags = (stage_data_flags)s.read_uint32_t();

        stage->ring_rectangle_x = s.read_float_t();
        stage->ring_rectangle_y = s.read_float_t();
        stage->ring_rectangle_width = s.read_float_t();
        stage->ring_rectangle_height = s.read_float_t();
        stage->ring_height = s.read_float_t();
        stage->ring_out_height = s.read_float_t();

        if (stage->object_ground.id == 0xFFFF)
            stage->object_ground.id = -1;
        if (stage->object_ground.set_id == 0xFFFF)
            stage->object_ground.set_id = -1;

        if (stage->object_ring.id == 0xFFFF)
            stage->object_ring.id = -1;
        if (stage->object_ring.set_id == 0xFFFF)
            stage->object_ring.set_id = -1;

        if (stage->object_sky.id == 0xFFFF)
            stage->object_sky.id = -1;
        if (stage->object_sky.set_id == 0xFFFF)
            stage->object_sky.set_id = -1;

        if (stage->object_shadow.id == 0xFFFF)
            stage->object_shadow.id = -1;
        if (stage->object_shadow.set_id == 0xFFFF)
            stage->object_shadow.set_id = -1;

        if (stage->object_reflect.id == 0xFFFF)
            stage->object_reflect.id = -1;
        if (stage->object_reflect.set_id == 0xFFFF)
            stage->object_reflect.set_id = -1;

        if (stage->object_refract.id == 0xFFFF)
            stage->object_refract.id = -1;
        if (stage->object_refract.set_id == 0xFFFF)
            stage->object_refract.set_id = -1;
    }
    s.position_pop();

    s.position_push(stage_effects_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_file* stage = &_stage_data[i];
        stage_effects* effects = &stage->effects;
        stage->effects_init = true;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = s.read_uint32_t();

        for (int32_t j = 0; j < 16; j++)
            effects->field_20[j] = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(auth3d_id_counts_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_file* stage = &_stage_data[i];

        stage->auth_3d_ids.resize(s.read_int32_t());
    }
    s.position_pop();

    s.position_push(auth_3d_ids_offsets_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_file* stage = &_stage_data[i];

        uint32_t id = s.read_uint32_t();
        uint32_t offset = s.read_uint32_t();
        if (!offset) {
            stage->auth_3d_ids.clear();
            stage->auth_3d_ids.shrink_to_fit();
            continue;
        }

        s.position_push(offset, SEEK_SET);
        int32_t auth_3d_count = (int32_t)stage->auth_3d_ids.size();
        for (int32_t j = 0; j < auth_3d_count; j++)
            if (s.read_int32_t() == -1) {
                stage->auth_3d_ids.resize(j);
                stage->auth_3d_ids.shrink_to_fit();
                break;
            }
        s.position_pop();

        if (stage->auth_3d_ids.size()) {
            s.position_push(offset, SEEK_SET);
            for (int32_t& j : stage->auth_3d_ids)
                j = s.read_int32_t();
            s.position_pop();
        }

    }
    s.position_pop();

    stage_data->ready = true;
    stage_data->modern = false;
    stage_data->big_endian = false;
    stage_data->is_x = false;
}

static void stage_database_file_classic_write_inner(stage_database_file* stage_data, stream& s) {
    s.write(0x20);

    int32_t count = (int32_t)stage_data->stage_data.size();

    int64_t stages_offset = s.get_position();
    switch (stage_data->format) {
    case STAGE_DATA_AC:
        s.write(count * 0x68LL);
        break;
    case STAGE_DATA_F:
        s.write(count * 0x6CLL);
        break;
    case STAGE_DATA_FT:
        s.write(count * 0x70LL);
        break;
    default:
        return;
    }
    s.align_write(0x20);

    int64_t stage_effects_offset = s.get_position();
    s.write(count * 0x60LL);
    s.align_write(0x20);

    int64_t auth3d_id_counts_offset = s.get_position();
    s.write(count * 0x04LL);
    s.align_write(0x20);

    int64_t auth_3d_ids_offsets_offset = s.get_position();
    s.write(count * 0x08LL + 0x04);
    s.align_write(0x20);

    std::vector<string_hash> strings;
    std::vector<int64_t> string_offsets;

    strings.reserve(count * 3LL);
    string_offsets.reserve(count * 3LL);

    if (stage_database_file_strings_push_back_check(strings, "")) {
        string_offsets.push_back(s.get_position());
        s.write_string_null_terminated("");
    }

    for (const stage_data_file& i : stage_data->stage_data) {
        if (stage_database_file_strings_push_back_check(strings, i.name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.name);
        }

        if (stage_database_file_strings_push_back_check(strings, i.auth_3d_name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.auth_3d_name);
        }

        if (stage_database_file_strings_push_back_check(strings, i.auth_3d_name)) {
            string_offsets.push_back(s.get_position());
            s.write_string_null_terminated(i.auth_3d_name);
        }
    }
    s.align_write(0x04);

    std::vector<int64_t> reflect_offsets;
    reflect_offsets.reserve(count);

    for (const stage_data_file& i : stage_data->stage_data)
        if (i.reflect) {
            reflect_offsets.push_back(s.get_position());
            s.write_uint32_t(i.reflect_data.mode);
            s.write_uint32_t(i.reflect_data.blur_num);
            s.write_uint32_t(i.reflect_data.blur_filter);
        }
        else
            reflect_offsets.push_back(0);

    std::vector<int64_t> refract_offsets;
    refract_offsets.reserve(count);

    for (const stage_data_file& i : stage_data->stage_data)
        if (i.refract) {
            refract_offsets.push_back(s.get_position());
            s.write_uint32_t(i.refract_data.mode);
        }
        else
            refract_offsets.push_back(0);

    std::vector<int64_t> auth_3d_ids_offsets;
    auth_3d_ids_offsets.reserve(count);

    for (const stage_data_file& i : stage_data->stage_data) {
        auth_3d_ids_offsets.push_back(s.get_position());
        for (const int32_t& j : i.auth_3d_ids)
            s.write_uint32_t(j);
        s.write_uint32_t(-1);
    }
    s.write_uint32_t(0x358637BD);

    if (stage_data->format == STAGE_DATA_F)
        s.align_write(0x10);

    stage_data_file* _stage_data = stage_data->stage_data.data();
    int64_t* _reflect_offsets = reflect_offsets.data();
    int64_t* _refract_offsets = refract_offsets.data();

    s.position_push(stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        const stage_data_file* stage = &_stage_data[i];

        s.write_uint32_t((uint32_t)stage_database_file_strings_get_string_offset(strings,
            string_offsets, stage->name));
        s.write_uint32_t((uint32_t)stage_database_file_strings_get_string_offset(strings,
            string_offsets, stage->auth_3d_name));
        s.write_uint32_t(stage->object_set_id);
        s.write_uint16_t((uint16_t)stage->object_ground.id);
        s.write_uint16_t((uint16_t)stage->object_ground.set_id);
        s.write_uint16_t((uint16_t)stage->object_ring.id);
        s.write_uint16_t((uint16_t)stage->object_ring.set_id);
        s.write_uint16_t((uint16_t)stage->object_sky.id);
        s.write_uint16_t((uint16_t)stage->object_sky.set_id);
        s.write_uint16_t((uint16_t)stage->object_shadow.id);
        s.write_uint16_t((uint16_t)stage->object_shadow.set_id);
        s.write_uint16_t((uint16_t)stage->object_reflect.id);
        s.write_uint16_t((uint16_t)stage->object_reflect.set_id);
        s.write_uint16_t((uint16_t)stage->object_refract.id);
        s.write_uint16_t((uint16_t)stage->object_refract.set_id);
        s.write_uint32_t(stage->lens_flare_texture);
        s.write_uint32_t(stage->lens_shaft_texture);
        s.write_uint32_t(stage->lens_ghost_texture);
        s.write_float_t(stage->lens_shaft_inv_scale);
        s.write_uint32_t(stage->unknown);
        s.write_uint32_t(stage->render_texture);

        if (stage_data->format > STAGE_DATA_AC)
            s.write_uint32_t(stage->movie_texture);

        s.write_uint32_t((uint32_t)stage_database_file_strings_get_string_offset(strings,
            string_offsets, stage->collision_file_path));
        s.write_uint32_t(stage->reflect_type);
        s.write_uint32_t(stage->refract_enable ? 0x01 : 0x00);

        s.write_uint32_t((uint32_t)_reflect_offsets[i]);
        s.write_uint32_t((uint32_t)_refract_offsets[i]);

        if (stage_data->format > STAGE_DATA_F)
            s.write_uint32_t(stage->flags);

        s.write_float_t(stage->ring_rectangle_x);
        s.write_float_t(stage->ring_rectangle_y);
        s.write_float_t(stage->ring_rectangle_width);
        s.write_float_t(stage->ring_rectangle_height);
        s.write_float_t(stage->ring_height);
        s.write_float_t(stage->ring_out_height);
    }
    s.position_pop();

    s.position_push(stage_effects_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        const stage_data_file* stage = &_stage_data[i];
        const stage_effects* effects = &stage->effects;

        for (int32_t j = 0; j < 8; j++)
            s.write_int32_t(effects->field_0[j]);

        for (int32_t j = 0; j < 16; j++)
            s.write_int32_t(effects->field_20[j]);
    }
    s.position_pop();

    s.position_push(auth3d_id_counts_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        const stage_data_file* stage = &_stage_data[i];

        s.write_int32_t((int32_t)stage->auth_3d_ids.size());
    }
    s.position_pop();

    int64_t* _auth_3d_ids_offsets = auth_3d_ids_offsets.data();

    s.position_push(auth_3d_ids_offsets_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        s.write_uint32_t(i);
        s.write_uint32_t((uint32_t)_auth_3d_ids_offsets[i]);
    }
    s.position_pop();

    s.position_push(0x00, SEEK_SET);
    s.write_int32_t(count);
    s.write_int32_t((int32_t)stages_offset);
    s.write_int32_t((int32_t)stage_effects_offset);
    s.write_int32_t((int32_t)auth3d_id_counts_offset);
    s.write_int32_t((int32_t)auth_3d_ids_offsets_offset);
    s.position_pop();
}

static void stage_database_file_modern_read_inner(stage_database_file* stage_data, stream& s, uint32_t header_length) {
    bool big_endian = s.big_endian;
    bool is_x = true;

    s.set_position(0x04, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    int32_t count = s.read_int32_t();
    int64_t stages_offset = s.read_offset(header_length, is_x);

    stage_data->stage_modern.resize(count);

    stage_data_modern_file* _stage_modern = stage_data->stage_modern.data();

    s.position_push(stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_modern_file* stage = &_stage_modern[i];
        stage->hash = (uint32_t)s.read_uint64_t_reverse_endianness();
        stage->name = s.read_string_null_terminated_offset(s.read_offset(header_length, is_x));
        stage->auth_3d_name = s.read_string_null_terminated_offset(s.read_offset(header_length, is_x));
        stage->object_ground.set_id = s.read_uint32_t_reverse_endianness();
        stage->object_ground.id = s.read_uint32_t_reverse_endianness();
        stage->object_sky.id = s.read_uint32_t_reverse_endianness();
        stage->object_sky.set_id = s.read_uint32_t_reverse_endianness();
        stage->object_shadow.id = s.read_uint32_t_reverse_endianness();
        stage->object_shadow.set_id = s.read_uint32_t_reverse_endianness();
        stage->object_reflect.id = s.read_uint32_t_reverse_endianness();
        stage->object_reflect.set_id = s.read_uint32_t_reverse_endianness();
        stage->object_refract.id = s.read_uint32_t_reverse_endianness();
        stage->object_refract.set_id = s.read_uint32_t_reverse_endianness();
        stage->lens_shaft_inv_scale = s.read_float_t_reverse_endianness();
        stage->unknown = s.read_uint32_t_reverse_endianness();
        stage->render_texture = s.read_uint32_t_reverse_endianness();
        stage->render_texture_flag = s.read_uint32_t_reverse_endianness();
        stage->movie_texture = s.read_uint32_t_reverse_endianness();
        stage->movie_texture_flag = s.read_uint32_t_reverse_endianness();
        stage->field_04 = s.read_uint32_t_reverse_endianness();
        stage->field_04_flag = s.read_uint32_t_reverse_endianness();
        stage->field_05 = s.read_uint32_t_reverse_endianness();
        stage->field_05_flag = s.read_uint32_t_reverse_endianness();
        stage->field_06 = s.read_uint32_t_reverse_endianness();
        stage->field_06_flag = s.read_uint32_t_reverse_endianness();
        stage->field_07 = s.read_uint32_t_reverse_endianness();
        stage->field_07_flag = s.read_uint32_t_reverse_endianness();
        stage->field_08 = s.read_uint32_t_reverse_endianness();
        stage->field_09 = s.read_uint32_t_reverse_endianness();
        stage->field_10 = s.read_uint32_t_reverse_endianness();

        if (is_x)
            s.read_uint32_t_reverse_endianness();

        stage->field_11 = s.read_offset(header_length, is_x);
        stage->field_12 = s.read_offset(header_length, is_x);

        stage->ring_rectangle_x = s.read_float_t_reverse_endianness();
        stage->ring_rectangle_y = s.read_float_t_reverse_endianness();
        stage->ring_rectangle_width = s.read_float_t_reverse_endianness();
        stage->ring_rectangle_height = s.read_float_t_reverse_endianness();
        stage->ring_height = s.read_float_t_reverse_endianness();
        stage->ring_out_height = s.read_float_t_reverse_endianness();

        if (is_x)
            stage->field_13 = s.read_uint32_t_reverse_endianness();

        stage_effects_modern* effects = &stage->effects;
        stage->effects_init = true;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = s.read_uint32_t_reverse_endianness();

        for (int32_t j = 0; j < 16; j++)
            effects->field_20[j] = s.read_uint32_t_reverse_endianness();

        if (is_x)
            s.read_uint32_t_reverse_endianness();

        int32_t auth_3d_ids_count = s.read_int32_t_reverse_endianness();
        int64_t auth_3d_ids_offset = s.read_offset(header_length, is_x);

        if (is_x)
            s.read_uint32_t_reverse_endianness();

        if (!auth_3d_ids_offset) {
            stage->auth_3d_ids.clear();
            stage->auth_3d_ids.shrink_to_fit();
            continue;
        }

        s.position_push(auth_3d_ids_offset, SEEK_SET);
        for (int32_t j = 0; j < auth_3d_ids_count; j++)
            if (s.read_uint32_t() == hash_murmurhash_empty) {
                stage->auth_3d_ids.resize(j);
                stage->auth_3d_ids.shrink_to_fit();
                break;
            }
        s.position_pop();

        if (stage->auth_3d_ids.size()) {
            s.position_push(auth_3d_ids_offset, SEEK_SET);
            for (uint32_t& j : stage->auth_3d_ids)
                j = s.read_int32_t();
            s.position_pop();
        }
    }
    s.position_pop();

    stage_data->ready = true;
    stage_data->modern = true;
    stage_data->big_endian = big_endian;
    stage_data->is_x = is_x;
    stage_data->format = STAGE_DATA_UNK;
}

static void stage_database_file_modern_write_inner(stage_database_file* stage_data, stream& s) {

}

inline static int64_t stage_database_file_strings_get_string_offset(const std::vector<string_hash>& vec,
    const std::vector<int64_t>& vec_off, const std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (const string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return vec_off[&i - vec.data()];
    return 0;
}

inline static bool stage_database_file_strings_push_back_check(std::vector<string_hash>& vec, const std::string& str) {
    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(str);
    uint64_t hash_murmurhash = hash_string_murmurhash(str);
    for (string_hash& i : vec)
        if (hash_fnv1a64m == i.hash_fnv1a64m && hash_murmurhash == i.hash_murmurhash)
            return false;

    vec.push_back(str);
    return true;
}
