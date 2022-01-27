/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.h"
#include "../f2/struct.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../str_utils.h"

vector_func(stage_info)
vector_func(stage_info_modern)

static void stage_database_classic_read_inner(stage_database* stage_data, stream* s);
static void stage_database_classic_write_inner(stage_database* stage_data, stream* s);
static void stage_database_modern_read_inner(stage_database* stage_data, stream* s, uint32_t header_length);
static void stage_database_modern_write_inner(stage_database* stage_data, stream* s);

void stage_database_init(stage_database* stage_data) {
    memset(stage_data, 0, sizeof(stage_database));
}

void stage_database_read(stage_database* stage_data, char* path, bool modern) {
    if (!stage_data || !path)
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
                stage_database_classic_read_inner(stage_data, &s_bin);
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
            f2_struct_read(&st, path_stg);
            if (st.header.signature == reverse_endianness_uint32_t('STGC')); {
                stream s_stgc;
                io_mopen(&s_stgc, st.data, st.length);
                s_stgc.is_big_endian = st.header.use_big_endian;
                stage_database_modern_read_inner(stage_data, &s_stgc, st.header.length);
                io_free(&s_stgc);
            }
            f2_struct_free(&st);
        }
        free(path_stg);
    }
}

void stage_database_wread(stage_database* stage_data, wchar_t* path, bool modern) {
    if (!stage_data || !path)
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
                stage_database_classic_read_inner(stage_data, &s_bin);
                io_free(&s_bin);
                free(data);
            }
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_wadd(path, L".stg");
        if (path_wcheck_file_exists(path_stg)) {
            f2_struct st;
            f2_struct_wread(&st, path_stg);
            if (st.header.signature == reverse_endianness_uint32_t('STGC')); {
                stream s_stgc;
                io_mopen(&s_stgc, st.data, st.length);
                s_stgc.is_big_endian = st.header.use_big_endian;
                stage_database_modern_read_inner(stage_data, &s_stgc, st.header.length);
                io_free(&s_stgc);
            }
            f2_struct_free(&st);
        }
        free(path_stg);
    }
}

void stage_database_mread(stage_database* stage_data, void* data, size_t length, bool modern) {
    if (!stage_data || !data || !length)
        return;

    if (!modern) {
        stream s;
        io_mopen(&s, data, length);
        stage_database_classic_read_inner(stage_data, &s);
        io_free(&s);
    }
    else {
        f2_struct st;
        f2_struct_mread(&st, data, length);
        if (st.header.signature == reverse_endianness_uint32_t('STGC')); {
            stream s_stgc;
            io_mopen(&s_stgc, st.data, st.length);
            s_stgc.is_big_endian = st.header.use_big_endian;
            stage_database_modern_read_inner(stage_data, &s_stgc, st.header.length);
            io_free(&s_stgc);
        }
        f2_struct_free(&st);
    }
}

void stage_database_write(stage_database* stage_data, char* path) {
    if (!stage_data || !path || !stage_data->ready)
        return;

    if (!stage_data->modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            stage_database_classic_write_inner(stage_data, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_stg = str_utils_add(path, ".stg");
        stream s;
        io_open(&s, path_stg, "wb");
        if (s.io.stream)
            stage_database_modern_write_inner(stage_data, &s);
        io_free(&s);
        free(path_stg);
    }
}

void stage_database_wwrite(stage_database* stage_data, wchar_t* path) {
    if (!stage_data || !path || !stage_data->ready)
        return;

    if (!stage_data->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_wopen(&s, path_bin, L"wb");
        if (s.io.stream)
            stage_database_classic_write_inner(stage_data, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_stg = str_utils_wadd(path, L".stg");
        stream s;
        io_wopen(&s, path_stg, L"wb");
        if (s.io.stream)
            stage_database_modern_write_inner(stage_data, &s);
        io_free(&s);
        free(path_stg);
    }
}

void stage_database_mwrite(stage_database* stage_data, void** data, size_t* length) {
    if (!stage_data || !data || !stage_data->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    if (!stage_data->modern)
        stage_database_classic_write_inner(stage_data, &s);
    else
        stage_database_modern_write_inner(stage_data, &s);
    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool stage_database_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    stage_database* stage_data = data;
    stage_database_read(stage_data, string_data(&s), stage_data->modern);

    string_free(&s);
    return stage_data->ready;
}

void stage_database_merge_mdata(stage_database* stage_data,
    stage_database* base_stage_data, stage_database* mdata_stage_data) {
    if (!stage_data || !base_stage_data || !mdata_stage_data
        || !base_stage_data->ready || !mdata_stage_data->ready)
        return;

    vector_stage_info* stage = &stage_data->stage;
    vector_stage_info* base_stage = &base_stage_data->stage;
    vector_stage_info* mdata_stage = &mdata_stage_data->stage;

    int32_t count = (int32_t)vector_length(*base_stage);
    vector_stage_info_reserve(stage, count);
    stage->end += count;

    for (int32_t i = 0; i < count; i++) {
        stage_info* b_info = &base_stage->begin[i];
        stage_info* info = &stage->begin[i];

        *info = *b_info;
        string_copy(&b_info->name, &info->name);
        string_copy(&b_info->auth_3d_name, &info->auth_3d_name);
        string_copy(&b_info->collision_file_path, &info->collision_file_path);
        if (b_info->auth_3d_count) {
            info->auth_3d_count = b_info->auth_3d_count;
            info->auth_3d_ids = force_malloc_s(uint32_t, b_info->auth_3d_count);
            memcpy(info->auth_3d_ids, b_info->auth_3d_ids, sizeof(uint32_t) * b_info->auth_3d_count);
        }
        else {
            info->auth_3d_count = 0;
            info->auth_3d_ids = 0;
        }
    }

    int32_t mdata_count = (int32_t)vector_length(*mdata_stage);
    mdata_count -= count;
    if (mdata_count > 0)
        vector_stage_info_reserve(stage, mdata_count);
    mdata_count += count;
    for (int32_t i = 0; i < mdata_count; i++) {
        stage_info* m_info = &mdata_stage->begin[i];

        char* name_str = string_data(&m_info->name);
        size_t name_len = m_info->name.length;

        stage_info* info = 0;
        for (info = stage->begin; info != stage->end; info++)
            if (!memcmp(name_str, string_data(&info->name), min(name_len, info->name.length) + 1))
                break;

        if (info == stage->end)
            info = vector_stage_info_reserve_back(stage);
        else
            stage_info_free(info);

        *info = *m_info;
        string_copy(&m_info->name, &info->name);
        string_copy(&m_info->auth_3d_name, &info->auth_3d_name);
        string_copy(&m_info->collision_file_path, &info->collision_file_path);
        if (m_info->auth_3d_count) {
            info->auth_3d_count = m_info->auth_3d_count;
            info->auth_3d_ids = force_malloc_s(uint32_t, m_info->auth_3d_count);
            memcpy(info->auth_3d_ids, m_info->auth_3d_ids, sizeof(uint32_t) * m_info->auth_3d_count);
        }
        else {
            info->auth_3d_count = 0;
            info->auth_3d_ids = 0;
        }
    }

    stage_data->ready = true;
}

void stage_database_split_mdata(stage_database* stage_data,
    stage_database* base_stage_data, stage_database* mdata_stage_data) {
    if (!stage_data || !base_stage_data || !mdata_stage_data
        || !stage_data->ready || !base_stage_data->ready)
        return;

}

void stage_database_free(stage_database* stage_data) {
    vector_stage_info_free(&stage_data->stage, stage_info_free);
}

void stage_info_free(stage_info* info) {
    string_free(&info->name);
    string_free(&info->auth_3d_name);
    string_free(&info->collision_file_path);
    free(info->auth_3d_ids);
}

void stage_info_modern_free(stage_info_modern* info) {
    string_free(&info->name);
    string_free(&info->auth_3d_name);
    free(info->auth_3d_ids);
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
        return;
    }

    stage_data->stage = vector_empty(stage_info);
    vector_stage_info_reserve(&stage_data->stage, count);
    stage_data->stage.end += count;

    io_position_push(s, stages_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_info* stage = &stage_data->stage.begin[i];

        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &stage->name);
        io_read_string_null_terminated_offset(s, io_read_uint32_t(s), &stage->auth_3d_name);
        stage->object_set_id = io_read_uint32_t(s);
        stage->object_ground.id = io_read_uint16_t(s);
        stage->object_ground.set_id = io_read_uint16_t(s);
        stage->object_unknown.id = io_read_uint16_t(s);
        stage->object_unknown.set_id = io_read_uint16_t(s);
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
        stage->reflect_type = io_read_uint32_t(s);
        stage->refract_enable = io_read_uint32_t(s) ? true : false;

        int32_t reflect_offset = io_read_uint32_t(s);
        if (reflect_offset) {
            io_position_push(s, reflect_offset, SEEK_SET);
            stage->reflect_data = true;
            stage->reflect.mode = io_read_uint32_t(s);
            stage->reflect.blur_num = io_read_uint32_t(s);
            stage->reflect.blur_filter = io_read_uint32_t(s);
            io_position_pop(s);
        }
        else
            stage->reflect_data = false;

        int32_t refract_offset = io_read_uint32_t(s);
        if (refract_offset) {
            io_position_push(s, refract_offset, SEEK_SET);
            stage->refract_data = true;
            stage->refract.mode = io_read_uint32_t(s);
            io_position_pop(s);
        }
        else
            stage->refract_data = false;

        if (stage_data->format > STAGE_DATA_F)
            stage->flags = io_read_uint32_t(s);

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

        if (stage->object_unknown.id == 0xFFFF)
            stage->object_unknown.id = -1;
        if (stage->object_unknown.set_id == 0xFFFF)
            stage->object_unknown.set_id = -1;

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
        stage_info* stage = &stage_data->stage.begin[i];
        stage_effects* effects = &stage->effects;

        for (int32_t j = 0; j < 8; j++)
            effects->field_0[j] = io_read_uint32_t(s);

        for (int32_t j = 0; j < 16; j++)
            effects->parent_bone_node[j] = io_read_uint32_t(s);
    }
    io_position_pop(s);

    io_position_push(s, auth3d_id_counts_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_info* stage = &stage_data->stage.begin[i];

        stage->auth_3d_count = io_read_uint32_t(s);
    }
    io_position_pop(s);

    io_position_push(s, auth3d_ids_offsets_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++) {
        stage_info* stage = &stage_data->stage.begin[i];

        uint32_t id = io_read_uint32_t(s);
        uint32_t offset = io_read_uint32_t(s);
        if (!offset) {
            stage->auth_3d_count = 0;
            continue;
        }

        io_position_push(s, offset, SEEK_SET);
        for (uint32_t j = 0; j < stage->auth_3d_count; j++) {
            if (io_read_uint32_t(s) == -1) {
                stage->auth_3d_count = j;
                break;
            }
        }
        io_position_pop(s);
        
        if (stage->auth_3d_count) {
            io_position_push(s, offset, SEEK_SET);
            stage->auth_3d_ids = force_malloc_s(uint32_t, stage->auth_3d_count);
            for (uint32_t j = 0; j < stage->auth_3d_count; j++)
                stage->auth_3d_ids[j] = io_read_uint32_t(s);
            io_position_pop(s);
        }

    }
    io_position_pop(s);

    stage_data->ready = true;
    stage_data->modern = false;
}

static void stage_database_classic_write_inner(stage_database* stage_data, stream* s) {
    /*writer.AddStringToStringTable( Name );
    writer.AddStringToStringTable( Auth3dName );
    writer.Write( ObjectSetID );
    Objects.WriteClassic( writer );
    writer.Write( LensFlareTexture );
    writer.Write( LensShaftTexture );
    writer.Write( LensGhostTexture );
    writer.Write( LensShaftInvScale );
    writer.Write( Field00 );
    writer.Write( RenderTextureId );

    if ( format > BinaryFormat.DT )
        writer.Write( MovieTextureId );

    writer.AddStringToStringTable( CollisionFilePath );
    writer.Write( ReflectType );
    writer.Write( RefractEnable ? 1u : 0u );
    
    writer.ScheduleWriteOffsetIf( Reflect != null && Reflect.BlurNum != 0 , 4, AlignmentMode.Left,
        () => Reflect.Write( writer ) );
    
    writer.ScheduleWriteOffsetIf( Refract != null , 4, AlignmentMode.Left,
        () => Refract.Write( writer ) );

    if ( format == BinaryFormat.FT )
        writer.Write( ( int ) Flags );

    writer.Write( RingRectangleX );
    writer.Write( RingRectangleY );
    writer.Write( RingRectangleWidth) ;
    writer.Write( RingRectangleHeight );
    writer.Write( RingRingHeight );
    writer.Write( RingOutHeight );*/
}

static void stage_database_modern_read_inner(stage_database* stage_data, stream* s, uint32_t header_length) {

}

static void stage_database_modern_write_inner(stage_database* stage_data, stream* s) {

}

