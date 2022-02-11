/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvsr.h"
#include "f2/struct.h"
#include "io/path.h"
#include "io/stream.h"
#include "str_utils.h"

vector_func(pvsr_a3da)
vector_func(pvsr_aet)
vector_func(pvsr_aet_entry)
vector_func(pvsr_effect)
vector_func(pvsr_glitter)
vector_func(pvsr_stage_effect)

static void pvsr_a3da_read(pvsr_a3da* a3d, stream* s);
static void pvsr_aet_read(pvsr_aet* aet, stream* s, int32_t x00);
static void pvsr_aet_entry_read(pvsr_aet_entry* aet_entry, stream* s);
static bool pvsr_aet_sub1_read(pvsr_aet_sub1* aet_sub1, stream* s, ssize_t offset);
static bool pvsr_aet_sub2_read(pvsr_aet_sub2* aet_sub2, stream* s, ssize_t offset);
static void pvsr_effect_read(pvsr_effect* eff, stream* s);
static void pvsr_glitter_read(pvsr_glitter* glt, stream* s);
static void pvsr_read_inner(pvsr* sr, stream* s);
static void pvsr_stage_effect_read(pvsr_stage_effect* stg_eff, stream* s);

void pvsr_init(pvsr* sr) {
    memset(sr, 0, sizeof(pvsr));
}

void pvsr_read(pvsr* sr, char* path) {
    if (!sr || !path)
        return;

    char* path_pvsr = str_utils_add(path, ".pvsr");
    if (path_check_file_exists(path_pvsr)) {
        stream s;
        io_open(&s, path_pvsr, "rb");
        if (s.io.stream)
            pvsr_read_inner(sr, &s);
        io_free(&s);
    }
    free(path_pvsr);
}

void pvsr_wread(pvsr* sr, wchar_t* path) {
    if (!sr || !path)
        return;

    wchar_t* path_pvsr = str_utils_wadd(path, L".pvsr");
    if (path_wcheck_file_exists(path_pvsr)) {
        stream s;
        io_wopen(&s, path_pvsr, L"rb");
        if (s.io.stream)
            pvsr_read_inner(sr, &s);
        io_free(&s);
    }
    free(path_pvsr);
}

bool pvsr_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    pvsr* sr = (pvsr*)data;
    pvsr_read(sr, string_data(&s));

    string_free(&s);
    return sr->ready;
}

void pvsr_free(pvsr* sr) {
    vector_pvsr_aet_free(&sr->aet, pvsr_aet_free);
    vector_pvsr_effect_free(&sr->effect, pvsr_effect_free);
    vector_string_free(&sr->emcs, string_free);
    for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++)
            if (sr->stage_change_effect_init[i][j])
                pvsr_stage_effect_free(&sr->stage_change_effect[i][j]);
    vector_pvsr_stage_effect_free(&sr->stage_effect, pvsr_stage_effect_free);
}

void pvsr_a3da_init(pvsr_a3da* a3d) {
    memset(a3d, 0, sizeof(pvsr_a3da));
}

void pvsr_a3da_free(pvsr_a3da* a3d) {
    string_free(&a3d->name);
}

void pvsr_aet_init(pvsr_aet* aet) {
    memset(aet, 0, sizeof(pvsr_aet));
}

void pvsr_aet_free(pvsr_aet* aet) {
    string_free(&aet->set_name);
    vector_pvsr_aet_entry_free(&aet->front, pvsr_aet_entry_free);
    vector_pvsr_aet_entry_free(&aet->front_low, pvsr_aet_entry_free);
    vector_pvsr_aet_entry_free(&aet->back, pvsr_aet_entry_free);
    pvsr_aet_sub1_free(&aet->sub1_data);
    pvsr_aet_sub2_free(&aet->sub2a_data);
    pvsr_aet_sub2_free(&aet->sub2b_data);
    pvsr_aet_sub2_free(&aet->sub2c_data);
    pvsr_aet_sub2_free(&aet->sub2d_data);
    vector_pvsr_aet_entry_free(&aet->unk03, pvsr_aet_entry_free);
    vector_pvsr_aet_entry_free(&aet->unk04, pvsr_aet_entry_free);
}

void pvsr_aet_entry_init(pvsr_aet_entry* aet_entry) {
    memset(aet_entry, 0, sizeof(pvsr_aet_entry));
}

void pvsr_aet_entry_free(pvsr_aet_entry* aet_entry) {
    string_free(&aet_entry->name);
}

void pvsr_aet_sub1_init(pvsr_aet_sub1* aet_sub1) {
    memset(aet_sub1, 0, sizeof(pvsr_aet_sub1));
}

void pvsr_aet_sub1_free(pvsr_aet_sub1* aet_sub1) {
    string_free(&aet_sub1->name);
}

void pvsr_aet_sub2_init(pvsr_aet_sub2* aet_sub2) {
    memset(aet_sub2, 0, sizeof(pvsr_aet_sub2));
}

void pvsr_aet_sub2_free(pvsr_aet_sub2* aet_sub2) {

}

void pvsr_effect_init(pvsr_effect* eff) {
    memset(eff, 0, sizeof(pvsr_effect));
}

void pvsr_effect_free(pvsr_effect* eff) {
    string_free(&eff->name);
}

void pvsr_glitter_init(pvsr_glitter* glt) {
    memset(glt, 0, sizeof(pvsr_glitter));
}

void pvsr_glitter_free(pvsr_glitter* glt) {
    string_free(&glt->name);
}

void pvsr_stage_effect_init(pvsr_stage_effect* stg_eff) {
    memset(stg_eff, 0, sizeof(pvsr_stage_effect));
}

void pvsr_stage_effect_free(pvsr_stage_effect* stg_eff) {
    vector_pvsr_a3da_free(&stg_eff->a3da, pvsr_a3da_free);
    vector_pvsr_glitter_free(&stg_eff->glitter, pvsr_glitter_free);
}

static void pvsr_a3da_read(pvsr_a3da* a3d, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &a3d->name);
    a3d->hash = io_read_uint32_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
}

static void pvsr_aet_read(pvsr_aet* aet, stream* s, int32_t x00) {
    ssize_t set_name_offset = io_read_offset_x(s);
    ssize_t front_offset = io_read_offset_x(s);
    ssize_t front_low_offset = io_read_offset_x(s);
    ssize_t back_offset = io_read_offset_x(s);
    ssize_t sub1_offset = io_read_offset_x(s);
    ssize_t sub2a_offset = io_read_offset_x(s);
    ssize_t sub2b_offset = io_read_offset_x(s);
    ssize_t sub2c_offset = io_read_offset_x(s);
    ssize_t sub2d_offset = io_read_offset_x(s);
    aet->u48 = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u4a = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u4c = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u4e = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u50 = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u52 = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u54 = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u56 = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u58 = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u5a = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u5c = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u5e = io_read_uint16_t_stream_reverse_endianness(s);
    aet->u60 = io_read_uint16_t_stream_reverse_endianness(s);
    uint8_t front_count = io_read_uint8_t(s);
    uint8_t front_low_count = io_read_uint8_t(s);
    uint8_t back_count = io_read_uint8_t(s);
    io_align_read(s, 0x08);
    ssize_t o68 = 0;
    ssize_t o70 = 0;
    uint8_t u78 = 0;
    uint8_t u79 = 0;
    if (x00 & 0x100) {
        o68 = io_read_offset_x(s);
        o70 = io_read_offset_x(s);
        u78 = io_read_uint8_t(s);
        u79 = io_read_uint8_t(s);
        io_align_read(s, 0x08);
    }

    io_read_string_null_terminated_offset(s, set_name_offset, &aet->set_name);

    vector_pvsr_aet_entry_reserve(&aet->front, front_count);
    aet->front.end += front_count;

    io_position_push(s, front_offset, SEEK_SET);
    for (int32_t i = 0; i < front_count; i++)
        pvsr_aet_entry_read(&aet->front.begin[i], s);
    io_position_pop(s);

    vector_pvsr_aet_entry_reserve(&aet->front_low, front_low_count);
    aet->front_low.end += front_low_count;

    io_position_push(s, front_low_offset, SEEK_SET);
    for (int32_t i = 0; i < front_low_count; i++)
        pvsr_aet_entry_read(&aet->front_low.begin[i], s);
    io_position_pop(s);

    vector_pvsr_aet_entry_reserve(&aet->back, back_count);
    aet->back.end += back_count;

    io_position_push(s, back_offset, SEEK_SET);
    for (int32_t i = 0; i < back_count; i++)
        pvsr_aet_entry_read(&aet->back.begin[i], s);
    io_position_pop(s);

    aet->sub1_data_init = pvsr_aet_sub1_read(&aet->sub1_data, s, sub1_offset);
    aet->sub2a_data_init = pvsr_aet_sub2_read(&aet->sub2a_data, s, sub2a_offset);
    aet->sub2b_data_init = pvsr_aet_sub2_read(&aet->sub2b_data, s, sub2b_offset);
    aet->sub2c_data_init = pvsr_aet_sub2_read(&aet->sub2c_data, s, sub2c_offset);
    aet->sub2d_data_init = pvsr_aet_sub2_read(&aet->sub2d_data, s, sub2d_offset);

    if (x00 & 0x100) {
        vector_pvsr_aet_entry_reserve(&aet->unk03, u78);
        aet->unk03.end += u78;

        io_position_push(s, o68, SEEK_SET);
        for (int32_t i = 0; i < u78; i++)
            pvsr_aet_entry_read(&aet->unk03.begin[i], s);
        io_position_pop(s);

        vector_pvsr_aet_entry_reserve(&aet->unk04, u79);
        aet->unk04.end += u79;

        io_position_push(s, o70, SEEK_SET);
        for (int32_t i = 0; i < u79; i++)
            pvsr_aet_entry_read(&aet->unk04.begin[i], s);
        io_position_pop(s);
    }
}

static void pvsr_aet_entry_read(pvsr_aet_entry* aet_entry, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &aet_entry->name);
    aet_entry->hash = io_read_uint32_t_stream_reverse_endianness(s);
    aet_entry->bright_scale = io_read_float_t_stream_reverse_endianness(s);
}

static bool pvsr_aet_sub1_read(pvsr_aet_sub1* aet_sub1, stream* s, ssize_t offset) {
    if (offset <= 0)
        return false;

    io_position_push(s, offset, SEEK_SET);
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &aet_sub1->name);
    aet_sub1->hash = io_read_uint32_t_stream_reverse_endianness(s);
    aet_sub1->unk2 = io_read_uint16_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
    io_position_pop(s);
    return true;
}

static bool pvsr_aet_sub2_read(pvsr_aet_sub2* aet_sub2, stream* s, ssize_t offset) {
    if (offset <= 0)
        return false;

    io_position_push(s, offset, SEEK_SET);
    aet_sub2->u00 = io_read_uint16_t_stream_reverse_endianness(s);
    aet_sub2->u02 = io_read_uint16_t_stream_reverse_endianness(s);
    aet_sub2->u04 = io_read_uint16_t_stream_reverse_endianness(s);
    aet_sub2->u06 = io_read_uint16_t_stream_reverse_endianness(s);
    aet_sub2->u08 = io_read_uint16_t_stream_reverse_endianness(s);
    aet_sub2->u0a = io_read_uint16_t_stream_reverse_endianness(s);
    aet_sub2->u0c = io_read_uint16_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
    io_position_pop(s);
    return true;
}

static void pvsr_effect_read(pvsr_effect* eff, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &eff->name);
    eff->emission = io_read_float_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
}

static void pvsr_glitter_read(pvsr_glitter* glt, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &glt->name);
    glt->unk1 = io_read_uint8_t(s);
    io_align_read(s, 0x08);
}

static void pvsr_read_inner(pvsr* sr, stream* s) {
    f2_struct st;
    f2_struct_sread(&st, s);
    if (st.header.signature == reverse_endianness_uint32_t('PVSR') && st.data) {
        stream s_pvsr;
        io_mopen(&s_pvsr, st.data, st.length);
        s_pvsr.is_big_endian = st.header.use_big_endian;

        int32_t x00 = io_read_int32_t_stream_reverse_endianness(&s_pvsr);
        uint8_t x04 = io_read_uint8_t(&s_pvsr);
        uint8_t x05 = io_read_uint8_t(&s_pvsr);
        uint8_t x06 = io_read_uint8_t(&s_pvsr);
        uint8_t x07 = io_read_uint8_t(&s_pvsr);
        uint8_t stage_effect_count = io_read_uint8_t(&s_pvsr);
        uint8_t aets_count = io_read_uint8_t(&s_pvsr);
        uint8_t x0a = io_read_uint8_t(&s_pvsr);
        uint8_t x0b = io_read_uint8_t(&s_pvsr);
        uint8_t x0c = io_read_uint8_t(&s_pvsr);
        uint8_t x0d = io_read_uint8_t(&s_pvsr);
        uint8_t x0e = io_read_uint8_t(&s_pvsr);
        uint8_t x0f = io_read_uint8_t(&s_pvsr);
        ssize_t x10 = io_read_offset_x(&s_pvsr);
        ssize_t stage_effect_offset = io_read_offset_x(&s_pvsr);
        ssize_t stage_change_effect_offset = io_read_offset_x(&s_pvsr);
        ssize_t aet_offset = io_read_offset_x(&s_pvsr);

        io_position_push(&s_pvsr, x10, SEEK_SET);
        uint8_t unknown_count = io_read_uint8_t(&s_pvsr);
        uint8_t effect_count = io_read_uint8_t(&s_pvsr);
        uint8_t emcs_count = io_read_uint8_t(&s_pvsr);
        ssize_t unknown_offset = io_read_offset_x(&s_pvsr);
        ssize_t effect_offset = io_read_offset_x(&s_pvsr);
        ssize_t emcs_offset = io_read_offset_x(&s_pvsr);
        io_position_pop(&s_pvsr);

        vector_pvsr_effect_reserve(&sr->effect, effect_count);
        sr->effect.end += effect_count;

        io_position_push(&s_pvsr, effect_offset, SEEK_SET);
        for (int32_t i = 0; i < effect_count; i++)
            pvsr_effect_read(&sr->effect.begin[i], &s_pvsr);
        io_position_pop(&s_pvsr);

        vector_string_reserve(&sr->emcs, emcs_count);
        sr->emcs.end += emcs_count;

        io_position_push(&s_pvsr, emcs_offset, SEEK_SET);
        for (int32_t i = 0; i < emcs_count; i++)
            io_read_string_null_terminated_offset(&s_pvsr,
                io_read_offset_x(&s_pvsr), &sr->emcs.begin[i]);
        io_position_pop(&s_pvsr);

        vector_pvsr_stage_effect_reserve(&sr->stage_effect, stage_effect_count);
        sr->stage_effect.end += stage_effect_count;

        io_position_push(&s_pvsr, stage_effect_offset, SEEK_SET);
        for (int32_t i = 0; i < stage_effect_count; i++)
            pvsr_stage_effect_read(&sr->stage_effect.begin[i], &s_pvsr);
        io_position_pop(&s_pvsr);

        for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++) {
                pvsr_stage_effect_init(&sr->stage_change_effect[i][j]);
                sr->stage_change_effect_init[i][j] = false;
            }

        io_position_push(&s_pvsr, stage_change_effect_offset, SEEK_SET);
        for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++) {
                ssize_t offset = io_read_offset_x(&s_pvsr);
                if (offset <= 0)
                    continue;

                io_position_push(&s_pvsr, offset, SEEK_SET);
                pvsr_stage_effect_read(&sr->stage_change_effect[i][j], &s_pvsr);
                sr->stage_change_effect_init[i][j] = true;
                io_position_pop(&s_pvsr);
            }
        io_position_pop(&s_pvsr);

        vector_pvsr_aet_reserve(&sr->aet, aets_count);
        sr->aet.end += aets_count;

        io_position_push(&s_pvsr, aet_offset, SEEK_SET);
        for (int32_t i = 0; i < aets_count; i++)
            pvsr_aet_read(&sr->aet.begin[i], &s_pvsr, x00);
        io_position_pop(&s_pvsr);

        io_free(&s_pvsr);

        sr->ready = true;
    }
    f2_struct_free(&st);
}

static void pvsr_stage_effect_read(pvsr_stage_effect* stg_eff, stream* s) {
    uint8_t u00 = io_read_uint8_t(s);
    uint8_t u01 = io_read_uint8_t(s);
    uint8_t u02 = io_read_uint8_t(s);
    uint8_t u03 = io_read_uint8_t(s);
    uint8_t u04 = io_read_uint8_t(s);
    ssize_t a3da_offset = io_read_offset_x(s);
    ssize_t glitter_offset = io_read_offset_x(s);
    io_read_offset_x(s);

    uint8_t a3da_count;
    uint8_t glitter_count;
    if (u04 != 0xFF) {
        a3da_count = u00;
        glitter_count = u01;
    }
    else {
        a3da_count = u01;
        glitter_count = u02;
    }

    vector_pvsr_a3da_reserve(&stg_eff->a3da, a3da_count);
    stg_eff->a3da.end += a3da_count;

    io_position_push(s, a3da_offset, SEEK_SET);
    for (int32_t i = 0; i < a3da_count; i++)
        pvsr_a3da_read(&stg_eff->a3da.begin[i], s);
    io_position_pop(s);

    vector_pvsr_glitter_reserve(&stg_eff->glitter, glitter_count);
    stg_eff->glitter.end += glitter_count;

    io_position_push(s, glitter_offset, SEEK_SET);
    for (int32_t i = 0; i < glitter_count; i++)
        pvsr_glitter_read(&stg_eff->glitter.begin[i], s);
    io_position_pop(s);
}
