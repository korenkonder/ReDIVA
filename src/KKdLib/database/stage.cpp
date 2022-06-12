/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.hpp"
#include "../f2/struct.hpp"
#include "../io/path.hpp"
#include "../io/stream.hpp"
#include "../hash.hpp"
#include "../str_utils.hpp"

static void stage_database_classic_read_inner(stage_database* stage_data, stream& s);
static void stage_database_classic_write_inner(stage_database* stage_data, stream& s);
static void stage_database_modern_read_inner(stage_database* stage_data, stream& s, uint32_t header_length);
static void stage_database_modern_write_inner(stage_database* stage_data, stream& s);

stage_data::stage_data() : id(), object_set_id(), lens_flare_texture(), lens_shaft_texture(),
lens_ghost_texture(), unknown(), render_texture(), movie_texture(), reflect_type(), refract_enable(),
reflect(), reflect_data(), refract(), refract_data(), flags(), ring_rectangle_x(), ring_rectangle_y(),
ring_rectangle_width(), ring_rectangle_height(), ring_height(), ring_out_height(), effects() {
    name_hash = hash_murmurhash_empty;
    lens_shaft_inv_scale = 1.0f;
}

stage_data::~stage_data() {

}

stage_data_modern::stage_data_modern() : unknown(), render_texture(),
render_texture_flag(), movie_texture(), movie_texture_flag(), field_04(), field_04_flag(), field_05(),
field_05_flag(), field_06(), field_06_flag(), field_07(), field_07_flag(), field_08(), field_09(),
field_10(), field_11(), field_12(), ring_rectangle_x(), ring_rectangle_y(), ring_rectangle_width(),
ring_rectangle_height(), ring_height(), ring_out_height(), field_13(), effects() {
    hash = hash_murmurhash_empty;
    auth_3d_name_hash = hash_murmurhash_empty;
    lens_shaft_inv_scale = 1.0f;
}

stage_data_modern::~stage_data_modern() {

}

stage_database::stage_database() : ready(), modern(), is_x(), format() {

}

stage_database::~stage_database() {

}

void stage_database::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            s.open(path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                stream s_bin;
                s_bin.open(data, s.length);
                stage_database_classic_read_inner(this, s_bin);
                free(data);
            }
        }
        free(path_bin);
    }
    else {
        char* path_stg = str_utils_add(path, ".stg");
        if (path_check_file_exists(path_stg)) {
            f2_struct st;
            st.read(path_stg);
            if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
                stream s_stgc;
                s_stgc.open(st.data);
                s_stgc.is_big_endian = st.header.use_big_endian;
                stage_database_modern_read_inner(this, s_stgc, st.header.length);
            }
        }
        free(path_stg);
    }
}

void stage_database::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            s.open(path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                stream s_bin;
                s_bin.open(data, s.length);
                stage_database_classic_read_inner(this, s_bin);
                free(data);
            }
        }
        free(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_add(path, L".stg");
        if (path_check_file_exists(path_stg)) {
            f2_struct st;
            st.read(path_stg);
            if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
                stream s_stgc;
                s_stgc.open(st.data);
                s_stgc.is_big_endian = st.header.use_big_endian;
                stage_database_modern_read_inner(this, s_stgc, st.header.length);
            }
        }
        free(path_stg);
    }
}

void stage_database::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        stream s;
        s.open(data, size);
        stage_database_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
            stream s_stgc;
            s_stgc.open(st.data);
            s_stgc.is_big_endian = st.header.use_big_endian;
            stage_database_modern_read_inner(this, s_stgc, st.header.length);
        }
    }
}

void stage_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        s.open(path_bin, "wb");
        if (s.io.stream)
            stage_database_classic_write_inner(this, s);
        free(path_bin);
    }
    else {
        char* path_stg = str_utils_add(path, ".stg");
        stream s;
        s.open(path_stg, "wb");
        if (s.io.stream)
            stage_database_modern_write_inner(this, s);
        free(path_stg);
    }
}

void stage_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        stream s;
        s.open(path_bin, L"wb");
        if (s.io.stream)
            stage_database_classic_write_inner(this, s);
        free(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_add(path, L".stg");
        stream s;
        s.open(path_stg, L"wb");
        if (s.io.stream)
            stage_database_modern_write_inner(this, s);
        free(path_stg);
    }
}

void stage_database::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
    s.open();
    if (!modern)
        stage_database_classic_write_inner(this, s);
    else
        stage_database_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool stage_database::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    stage_database* stage_data = (stage_database*)data;
    stage_data->read(s.c_str(), stage_data->modern);

    return stage_data->ready;
}

int32_t stage_database::get_stage_index(const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (::stage_data& i : stage_data)
        if (name_hash == i.name_hash)
            return (int32_t)(&i - stage_data.data());
    return -1;
}

void stage_database::merge_mdata(stage_database* base_stage_data, stage_database* mdata_stage_data) {
    if (!base_stage_data || !mdata_stage_data || !base_stage_data->ready || !mdata_stage_data->ready)
        return;

    if (this != base_stage_data) {
        stage_data = base_stage_data->stage_data;
        stage_modern = base_stage_data->stage_modern;
    }

    for (::stage_data i : mdata_stage_data->stage_data) {
        uint32_t name_hash = i.name_hash;

        ::stage_data* info = 0;
        for (::stage_data& j : stage_data)
            if (name_hash == j.name_hash) {
                info = &j;
                break;
            }

        if (info)
            *info = i;
        else
            stage_data.push_back(i);
    }

    for (stage_data_modern& i : mdata_stage_data->stage_modern) {
        uint32_t name_hash = hash_string_murmurhash(&i.name);

        stage_data_modern* info = 0;
        for (stage_data_modern& j : stage_modern)
            if (name_hash == hash_string_murmurhash(&j.name)) {
                info = &j;
                break;
            }

        if (info)
            *info = i;
        else
            stage_modern.push_back(i);
    }

    ready = true;
    modern = base_stage_data->modern;
    is_x = base_stage_data->is_x;
    format = base_stage_data->format;
}

void stage_database::split_mdata(stage_database* base_stage_data, stage_database* mdata_stage_data) {
    if (!base_stage_data || !mdata_stage_data || !ready || !base_stage_data->ready)
        return;

}

static void stage_database_classic_read_inner(stage_database* stage_data, stream& s) {
    int32_t count = s.read_int32_t();
    int32_t stages_offset = s.read_int32_t();
    int32_t stage_effects_offset = s.read_int32_t();
    int32_t auth3d_id_counts_offset = s.read_int32_t();
    int32_t auth3d_ids_offsets_offset = s.read_int32_t();

    int32_t size = (stage_effects_offset - stages_offset) / count;
    stage_data->format = size == 104 ? STAGE_DATA_AC
        : (size == 108 ? STAGE_DATA_F
        : (size >= 112 ? STAGE_DATA_FT : STAGE_DATA_UNK));

    if (stage_data->format == STAGE_DATA_UNK) {
        stage_data->ready = false;
        stage_data->modern = false;
        stage_data->is_x = false;
        return;
    }

    stage_data->stage_data.resize(count);

    s.position_push(stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];

        stage->id = i;
        stage->name = s.read_string_null_terminated_offset(s.read_uint32_t());
        stage->name_hash = hash_string_murmurhash(&stage->name);
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
        ::stage_data* stage = &stage_data->stage_data[i];
        stage_effects* effects = &stage->effects;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = s.read_uint32_t();

        for (int32_t j = 0; j < 16; j++)
            effects->field_20[j] = s.read_uint32_t();
    }
    s.position_pop();

    s.position_push(auth3d_id_counts_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];

        stage->auth_3d_ids.resize(s.read_int32_t());
    }
    s.position_pop();

    s.position_push(auth3d_ids_offsets_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];

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
    stage_data->is_x = false;
}

static void stage_database_classic_write_inner(stage_database* stage_data, stream& s) {

}

static void stage_database_modern_read_inner(stage_database* stage_data, stream& s, uint32_t header_length) {
    bool is_x = true;

    s.set_position(0x04, SEEK_SET);
    is_x &= s.read_uint32_t_reverse_endianness() == 0;

    s.set_position(0x00, SEEK_SET);

    int32_t count = s.read_int32_t();
    int64_t stages_offset = s.read_offset(header_length, is_x);

    stage_data->stage_modern.resize(count);

    s.position_push(stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_modern* stage = &stage_data->stage_modern[i];
        stage->hash = (uint32_t)s.read_uint64_t_reverse_endianness();
        stage->name = s.read_string_null_terminated_offset(s.read_offset(header_length, is_x));
        stage->auth_3d_name = s.read_string_null_terminated_offset(s.read_offset(header_length, is_x));
        stage->auth_3d_name_hash = hash_string_murmurhash(&stage->auth_3d_name);
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

        stage_effects* effects = &stage->effects;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = s.read_uint32_t_reverse_endianness();

        for (int32_t j = 0; j < 16; j++)
            effects->field_20[j] = s.read_uint32_t_reverse_endianness();

        if (is_x)
            s.read_uint32_t_reverse_endianness();

        int32_t auth3d_ids_count = s.read_int32_t_reverse_endianness();
        int64_t auth3d_ids_offset = s.read_offset(header_length, is_x);

        if (is_x)
            s.read_uint32_t_reverse_endianness();

        if (!auth3d_ids_offset) {
            stage->auth_3d_ids.clear();
            stage->auth_3d_ids.shrink_to_fit();
            continue;
        }

        s.position_push(auth3d_ids_offset, SEEK_SET);
        for (int32_t j = 0; j < auth3d_ids_count; j++)
            if (s.read_uint32_t() == hash_murmurhash_empty) {
                stage->auth_3d_ids.resize(j);
                stage->auth_3d_ids.shrink_to_fit();
                break;
            }
        s.position_pop();

        if (stage->auth_3d_ids.size()) {
            s.position_push(auth3d_ids_offset, SEEK_SET);
            for (uint32_t& j : stage->auth_3d_ids)
                j = s.read_int32_t();
            s.position_pop();
        }
    }
    s.position_pop();

    stage_data->ready = true;
    stage_data->modern = true;
    stage_data->is_x = is_x;
    stage_data->format = STAGE_DATA_UNK;
}

static void stage_database_modern_write_inner(stage_database* stage_data, stream& s) {

}

