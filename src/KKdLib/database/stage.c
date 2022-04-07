/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../hash.h"
#include "../str_utils.h"

static void stage_database_classic_read_inner(stage_database* stage_data, stream* s);
static void stage_database_classic_write_inner(stage_database* stage_data, stream* s);
static void stage_database_modern_read_inner(stage_database* stage_data, stream* s, uint32_t header_length);
static void stage_database_modern_write_inner(stage_database* stage_data, stream* s);

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
            io_open(&s, path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                stage_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
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
                io_open(&s_stgc, &st.data);
                s_stgc.is_big_endian = st.header.use_big_endian;
                stage_database_modern_read_inner(this, &s_stgc, st.header.length);
                io_free(&s_stgc);
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
            io_open(&s, path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                io_read(&s, data, s.length);
                stream s_bin;
                io_open(&s_bin, data, s.length);
                stage_database_classic_read_inner(this, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
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
                io_open(&s_stgc, &st.data);
                s_stgc.is_big_endian = st.header.use_big_endian;
                stage_database_modern_read_inner(this, &s_stgc, st.header.length);
                io_free(&s_stgc);
            }
        }
        free(path_stg);
    }
}

void stage_database::read(const void* data, size_t length, bool modern) {
    if (!data || !length)
        return;

    if (!modern) {
        stream s;
        io_open(&s, data, length);
        stage_database_classic_read_inner(this, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        st.read(data, length);
        if (st.header.signature == reverse_endianness_uint32_t('STGC')) {
            stream s_stgc;
            io_open(&s_stgc, &st.data);
            s_stgc.is_big_endian = st.header.use_big_endian;
            stage_database_modern_read_inner(this, &s_stgc, st.header.length);
            io_free(&s_stgc);
        }
    }
}

void stage_database::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            stage_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_stg = str_utils_add(path, ".stg");
        stream s;
        io_open(&s, path_stg, "wb");
        if (s.io.stream)
            stage_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_stg);
    }
}

void stage_database::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        stream s;
        io_open(&s, path_bin, L"wb");
        if (s.io.stream)
            stage_database_classic_write_inner(this, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_add(path, L".stg");
        stream s;
        io_open(&s, path_stg, L"wb");
        if (s.io.stream)
            stage_database_modern_write_inner(this, &s);
        io_free(&s);
        free(path_stg);
    }
}

void stage_database::write(void** data, size_t* length) {
    if (!!data || !ready)
        return;

    stream s;
    io_open(&s);
    if (!modern)
        stage_database_classic_write_inner(this, &s);
    else
        stage_database_modern_write_inner(this, &s);
    io_align_write(&s, 0x10);
    io_copy(&s, data, length);
    io_free(&s);
}

bool stage_database::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    stage_database* stage_data = (stage_database*)data;
    stage_data->read(string_data(&s), stage_data->modern);

    string_free(&s);
    return stage_data->ready;
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

static void stage_database_classic_read_inner(stage_database* stage_data, stream* s) {
    int32_t count = io_read_int32_t(s);
    int32_t stages_offset = io_read_int32_t(s);
    int32_t stage_effects_offset = io_read_int32_t(s);
    int32_t auth3d_id_counts_offset = io_read_int32_t(s);
    int32_t auth3d_ids_offsets_offset = io_read_int32_t(s);

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

    io_position_push(s, stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];

        stage->id = i;
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &stage->name);
        stage->name_hash = hash_string_murmurhash(&stage->name);
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &stage->auth_3d_name);
        stage->object_set_id = (uint16_t)io_read_uint32_t(s); // Crutch to remove higher bits
        stage->object_ground.id = io_read_uint16_t(s);
        stage->object_ground.set_id = io_read_uint16_t(s);
        stage->object_ring.id = io_read_uint16_t(s);
        stage->object_ring.set_id = io_read_uint16_t(s);
        stage->object_sky.id = io_read_uint16_t(s);
        stage->object_sky.set_id = io_read_uint16_t(s);
        stage->object_shadow.id = io_read_uint16_t(s);
        stage->object_shadow.set_id = io_read_uint16_t(s);
        stage->object_reflect.id = io_read_uint16_t(s);
        stage->object_reflect.set_id = io_read_uint16_t(s);
        stage->object_refract.id = io_read_uint16_t(s);
        stage->object_refract.set_id = io_read_uint16_t(s);
        stage->lens_flare_texture = io_read_uint32_t(s);
        stage->lens_shaft_texture = io_read_uint32_t(s);
        stage->lens_ghost_texture = io_read_uint32_t(s);
        stage->lens_shaft_inv_scale = io_read_float_t(s);
        stage->unknown = io_read_uint32_t(s);
        stage->render_texture = io_read_uint32_t(s);

        if (stage_data->format > STAGE_DATA_AC)
            stage->movie_texture = io_read_uint32_t(s);
        else
            stage->movie_texture = -1;

        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &stage->collision_file_path);
        stage->reflect_type = (stage_data_reflect_type)io_read_uint32_t(s);
        stage->refract_enable = io_read_uint32_t(s) ? true : false;

        int32_t reflect_offset = io_read_uint32_t(s);
        if (reflect_offset) {
            io_position_push(s, reflect_offset, SEEK_SET);
            stage->reflect = true;
            stage->reflect_data.mode = (stage_data_reflect_resolution_mode)io_read_uint32_t(s);
            stage->reflect_data.blur_num = io_read_uint32_t(s);
            stage->reflect_data.blur_filter = (stage_data_blur_filter_mode)io_read_uint32_t(s);
            io_position_pop(s);
        }
        else
            stage->reflect = false;

        int32_t refract_offset = io_read_uint32_t(s);
        if (refract_offset) {
            io_position_push(s, refract_offset, SEEK_SET);
            stage->refract = true;
            stage->refract_data.mode = (stage_data_refract_resolution_mode)io_read_uint32_t(s);
            io_position_pop(s);
        }
        else
            stage->refract = false;

        if (stage_data->format > STAGE_DATA_F)
            stage->flags = (stage_data_flags)io_read_uint32_t(s);

        stage->ring_rectangle_x = io_read_float_t(s);
        stage->ring_rectangle_y = io_read_float_t(s);
        stage->ring_rectangle_width = io_read_float_t(s);
        stage->ring_rectangle_height = io_read_float_t(s);
        stage->ring_height = io_read_float_t(s);
        stage->ring_out_height = io_read_float_t(s);

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
    io_position_pop(s);

    io_position_push(s, stage_effects_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];
        stage_effects* effects = &stage->effects;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = io_read_uint32_t(s);

        for (int32_t j = 0; j < 16; j++)
            effects->field_20[j] = io_read_uint32_t(s);
    }
    io_position_pop(s);

    io_position_push(s, auth3d_id_counts_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];

        stage->auth_3d_ids.resize(io_read_int32_t(s));
    }
    io_position_pop(s);

    io_position_push(s, auth3d_ids_offsets_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        ::stage_data* stage = &stage_data->stage_data[i];

        uint32_t id = io_read_uint32_t(s);
        uint32_t offset = io_read_uint32_t(s);
        if (!offset) {
            stage->auth_3d_ids.clear();
            stage->auth_3d_ids.shrink_to_fit();
            continue;
        }

        io_position_push(s, offset, SEEK_SET);
        int32_t auth_3d_count = (int32_t)stage->auth_3d_ids.size();
        for (int32_t j = 0; j < auth_3d_count; j++)
            if (io_read_int32_t(s) == -1) {
                stage->auth_3d_ids.resize(j);
                stage->auth_3d_ids.shrink_to_fit();
                break;
            }
        io_position_pop(s);
        
        if (stage->auth_3d_ids.size()) {
            io_position_push(s, offset, SEEK_SET);
            for (int32_t& j : stage->auth_3d_ids)
                j = io_read_int32_t(s);
            io_position_pop(s);
        }

    }
    io_position_pop(s);

    stage_data->ready = true;
    stage_data->modern = false;
    stage_data->is_x = false;
}

static void stage_database_classic_write_inner(stage_database* stage_data, stream* s) {

}

static void stage_database_modern_read_inner(stage_database* stage_data, stream* s, uint32_t header_length) {
    bool is_x = true;

    io_set_position(s, 0x04, SEEK_SET);
    is_x &= io_read_uint32_t_stream_reverse_endianness(s) == 0;

    io_set_position(s, 0x00, SEEK_SET);

    int32_t count = io_read_int32_t(s);
    ssize_t stages_offset = io_read_offset(s, header_length, is_x);

    stage_data->stage_modern.resize(count);

    io_position_push(s, stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_data_modern* stage = &stage_data->stage_modern[i];
        stage->hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(s);
        io_read_string_null_terminated_offset(s,
            io_read_offset(s, header_length, is_x), &stage->name);
        io_read_string_null_terminated_offset(s,
            io_read_offset(s, header_length, is_x), &stage->auth_3d_name);
        stage->auth_3d_name_hash = hash_string_murmurhash(&stage->auth_3d_name);
        stage->object_ground.set_id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_ground.id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_sky.id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_sky.set_id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_shadow.id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_shadow.set_id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_reflect.id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_reflect.set_id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_refract.id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->object_refract.set_id = io_read_uint32_t_stream_reverse_endianness(s);
        stage->lens_shaft_inv_scale = io_read_float_t_stream_reverse_endianness(s);
        stage->unknown = io_read_uint32_t_stream_reverse_endianness(s);
        stage->render_texture = io_read_uint32_t_stream_reverse_endianness(s);
        stage->render_texture_flag = io_read_uint32_t_stream_reverse_endianness(s);
        stage->movie_texture = io_read_uint32_t_stream_reverse_endianness(s);
        stage->movie_texture_flag = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_04 = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_04_flag = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_05 = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_05_flag = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_06 = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_06_flag = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_07 = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_07_flag = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_08 = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_09 = io_read_uint32_t_stream_reverse_endianness(s);
        stage->field_10 = io_read_uint32_t_stream_reverse_endianness(s);

        if (is_x)
            io_read_uint32_t_stream_reverse_endianness(s);

        stage->field_11 = io_read_offset(s, header_length, is_x);
        stage->field_12 = io_read_offset(s, header_length, is_x);

        stage->ring_rectangle_x = io_read_float_t_stream_reverse_endianness(s);
        stage->ring_rectangle_y = io_read_float_t_stream_reverse_endianness(s);
        stage->ring_rectangle_width = io_read_float_t_stream_reverse_endianness(s);
        stage->ring_rectangle_height = io_read_float_t_stream_reverse_endianness(s);
        stage->ring_height = io_read_float_t_stream_reverse_endianness(s);
        stage->ring_out_height = io_read_float_t_stream_reverse_endianness(s);

        if (is_x)
            stage->field_13 = io_read_uint32_t_stream_reverse_endianness(s);

        stage_effects* effects = &stage->effects;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = io_read_uint32_t_stream_reverse_endianness(s);

        for (int32_t j = 0; j < 16; j++)
            effects->field_20[j] = io_read_uint32_t_stream_reverse_endianness(s);

        if (is_x)
            io_read_uint32_t_stream_reverse_endianness(s);

        int32_t auth3d_ids_count = io_read_int32_t_stream_reverse_endianness(s);
        ssize_t auth3d_ids_offset = io_read_offset(s, header_length, is_x);

        if (is_x)
            io_read_uint32_t_stream_reverse_endianness(s);

        if (!auth3d_ids_offset) {
            stage->auth_3d_ids.clear();
            stage->auth_3d_ids.shrink_to_fit();
            continue;
        }

        io_position_push(s, auth3d_ids_offset, SEEK_SET);
        for (int32_t j = 0; j < auth3d_ids_count; j++)
            if (io_read_uint32_t(s) == hash_murmurhash_empty) {
                stage->auth_3d_ids.resize(j);
                stage->auth_3d_ids.shrink_to_fit();
                break;
            }
        io_position_pop(s);

        if (stage->auth_3d_ids.size()) {
            io_position_push(s, auth3d_ids_offset, SEEK_SET);
            for (uint32_t& j : stage->auth_3d_ids)
                j = io_read_int32_t(s);
            io_position_pop(s);
        }
    }
    io_position_pop(s);

    stage_data->ready = true;
    stage_data->modern = true;
    stage_data->is_x = is_x;
    stage_data->format = STAGE_DATA_UNK;
}

static void stage_database_modern_write_inner(stage_database* stage_data, stream* s) {

}

