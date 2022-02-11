/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvpp.h"
#include "f2/struct.h"
#include "io/path.h"
#include "io/stream.h"
#include "str_utils.h"

vector_func(pvpp_a3da)
vector_func(pvpp_chara)
vector_func(pvpp_chara_item)
vector_func(pvpp_chara_effect)
vector_func(pvpp_chara_effect_a3da)
vector_func(pvpp_effect)
vector_func(pvpp_glitter)
vector_func(pvpp_motion)

static void pvpp_a3da_read(pvpp_a3da* a3d, stream* s);
static void pvpp_chara_read(pvpp_chara* chr, stream* s);
static void pvpp_chara_effect_read(pvpp_chara_effect* chr_eff, stream* s);
static void pvpp_chara_effect_a3da_read(pvpp_chara_effect_a3da* chr_pv_eff, stream* s);
static void pvpp_chara_item_read(pvpp_chara_item* chr_itm, stream* s);
static void pvpp_effect_read(pvpp_effect* eff, stream* s);
static void pvpp_glitter_read(pvpp_glitter* glt, stream* s);
static void pvpp_motion_read(pvpp_motion* mot, stream* s);
static void pvpp_read_inner(pvpp* pp, stream* s);

void pvpp_init(pvpp* pp) {
    memset(pp, 0, sizeof(pvpp));
}

void pvpp_read(pvpp* pp, char* path) {
    if (!pp || !path)
        return;

    char* path_pvpp = str_utils_add(path, ".pvpp");
    if (path_check_file_exists(path_pvpp)) {
        stream s;
        io_open(&s, path_pvpp, "rb");
        if (s.io.stream)
            pvpp_read_inner(pp, &s);
        io_free(&s);
    }
    free(path_pvpp);
}

void pvpp_wread(pvpp* pp, wchar_t* path) {
    if (!pp || !path)
        return;

    wchar_t* path_pvpp = str_utils_wadd(path, L".pvpp");
    if (path_wcheck_file_exists(path_pvpp)) {
        stream s;
        io_wopen(&s, path_pvpp, L"rb");
        if (s.io.stream)
            pvpp_read_inner(pp, &s);
        io_free(&s);
    }
    free(path_pvpp);
}

bool pvpp_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    pvpp* pp = (pvpp*)data;
    pvpp_read(pp, string_data(&s));

    string_free(&s);
    return pp->ready;
}

void pvpp_free(pvpp* pp) {
    vector_pvpp_chara_free(&pp->chara, pvpp_chara_free);
    vector_pvpp_effect_free(&pp->effect, pvpp_effect_free);
}

void pvpp_a3da_init(pvpp_a3da* a3d) {
    memset(a3d, 0, sizeof(pvpp_a3da));
}

void pvpp_a3da_free(pvpp_a3da* a3d) {
    string_free(&a3d->name);
}

void pvpp_chara_init(pvpp_chara* chr) {
    memset(chr, 0, sizeof(pvpp_chara));
}

void pvpp_chara_free(pvpp_chara* chr) {
    vector_pvpp_a3da_free(&chr->a3da, pvpp_a3da_free);
    vector_pvpp_chara_effect_free(&chr->chara_effect, pvpp_chara_effect_free);
    vector_pvpp_chara_item_free(&chr->item, pvpp_chara_item_free);
    vector_pvpp_glitter_free(&chr->glitter, pvpp_glitter_free);
    vector_pvpp_motion_free(&chr->motion, pvpp_motion_free);
}

void pvpp_chara_effect_init(pvpp_chara_effect* chr_eff) {
    memset(chr_eff, 0, sizeof(pvpp_chara_effect));
}

void pvpp_chara_effect_free(pvpp_chara_effect* chr_eff) {
    vector_pvpp_chara_effect_a3da_free(&chr_eff->effect_a3da, pvpp_chara_effect_a3da_free);
}

void pvpp_chara_effect_a3da_init(pvpp_chara_effect_a3da* chr_eff_a3da) {
    memset(chr_eff_a3da, 0, sizeof(pvpp_chara_effect_a3da));
}

void pvpp_chara_effect_a3da_free(pvpp_chara_effect_a3da* chr_eff_a3da) {
    pvpp_a3da_free(&chr_eff_a3da->a3da);
}

void pvpp_chara_item_init(pvpp_chara_item* chr_itm) {
    memset(chr_itm, 0, sizeof(pvpp_chara_item));
}

void pvpp_chara_item_free(pvpp_chara_item* chr_itm) {
    vector_pvpp_a3da_free(&chr_itm->a3da, pvpp_a3da_free);
    string_free(&chr_itm->bone);
}

void pvpp_effect_init(pvpp_effect* eff) {
    memset(eff, 0, sizeof(pvpp_effect));
}

void pvpp_effect_free(pvpp_effect* eff) {
    vector_pvpp_a3da_free(&eff->a3da, pvpp_a3da_free);
    vector_pvpp_glitter_free(&eff->glitter, pvpp_glitter_free);
}

void pvpp_glitter_init(pvpp_glitter* glt) {
    memset(glt, 0, sizeof(pvpp_glitter));
}

void pvpp_glitter_free(pvpp_glitter* glt) {
    string_free(&glt->name);
    string_free(&glt->unk1);
}

void pvpp_motion_init(pvpp_motion* mot) {
    memset(mot, 0, sizeof(pvpp_motion));
}

void pvpp_motion_free(pvpp_motion* mot) {
    string_free(&mot->name);
}

static void pvpp_a3da_read(pvpp_a3da* a3d, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &a3d->name);
    a3d->hash = io_read_uint32_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
}

static void pvpp_chara_read(pvpp_chara* chr, stream* s) {
    uint8_t u00 = io_read_uint8_t(s);
    uint8_t chr_eff_count = io_read_uint8_t(s);
    uint8_t motion_count = io_read_uint8_t(s);
    uint8_t a3da_count = io_read_uint8_t(s);
    uint8_t item_count = io_read_uint8_t(s);
    uint8_t u05 = io_read_uint8_t(s);
    uint8_t glitter_count = io_read_uint8_t(s);
    uint8_t u07 = io_read_uint8_t(s);
    size_t chr_eff_offset = io_read_offset_x(s);
    size_t motion_offset = io_read_offset_x(s);
    size_t a3da_offset = io_read_offset_x(s);
    size_t item_offset = io_read_offset_x(s);
    size_t glitter_offset = io_read_offset_x(s);
    size_t o30 = io_read_offset_x(s);

    vector_pvpp_chara_effect_reserve(&chr->chara_effect, chr_eff_count);
    chr->chara_effect.end += chr_eff_count;

    io_position_push(s, chr_eff_offset, SEEK_SET);
    for (int32_t i = 0; i < chr_eff_count; i++)
        pvpp_chara_effect_read(&chr->chara_effect.begin[i], s);
    io_position_pop(s);

    vector_pvpp_motion_reserve(&chr->motion, motion_count);
    chr->motion.end += motion_count;

    io_position_push(s, motion_offset, SEEK_SET);
    for (int32_t i = 0; i < motion_count; i++) {
        io_position_push(s, io_read_offset_x(s), SEEK_SET);
        pvpp_motion_read(&chr->motion.begin[i], s);
        io_position_pop(s);
    }
    io_position_pop(s);

    vector_pvpp_a3da_reserve(&chr->a3da, a3da_count);
    chr->a3da.end += a3da_count;

    io_position_push(s, a3da_offset, SEEK_SET);
    for (int32_t i = 0; i < a3da_count; i++)
        pvpp_a3da_read(&chr->a3da.begin[i], s);
    io_position_pop(s);

    vector_pvpp_chara_item_reserve(&chr->item, item_count);
    chr->item.end += item_count;

    io_position_push(s, item_offset, SEEK_SET);
    for (int32_t i = 0; i < item_count; i++)
        pvpp_chara_item_read(&chr->item.begin[i], s);
    io_position_pop(s);

    vector_pvpp_glitter_reserve(&chr->glitter, glitter_count);
    chr->glitter.end += glitter_count;

    io_position_push(s, glitter_offset, SEEK_SET);
    for (int32_t i = 0; i < glitter_count; i++)
        pvpp_glitter_read(&chr->glitter.begin[i], s);
    io_position_pop(s);
}

static void pvpp_chara_effect_read(pvpp_chara_effect* chr_eff, stream* s) {
    chr_eff->base_chara = (pvpp_chara_type)io_read_uint8_t(s);
    uint8_t pv_effects_count = io_read_uint8_t(s);
    size_t pv_effects_offstet = io_read_offset_x(s);

    vector_pvpp_chara_effect_a3da_reserve(&chr_eff->effect_a3da, pv_effects_count);
    chr_eff->effect_a3da.end += pv_effects_count;

    io_position_push(s, pv_effects_offstet, SEEK_SET);
    for (int32_t i = 0; i < pv_effects_count; i++)
        pvpp_chara_effect_a3da_read(&chr_eff->effect_a3da.begin[i], s);
    io_position_pop(s);
}

static void pvpp_chara_effect_a3da_read(pvpp_chara_effect_a3da* chr_eff_a3da, stream* s) {
    chr_eff_a3da->u00 = io_read_uint8_t(s);
    chr_eff_a3da->u01 = io_read_uint8_t(s);
    chr_eff_a3da->u02 = io_read_uint8_t(s);
    chr_eff_a3da->u03 = io_read_uint8_t(s);
    chr_eff_a3da->u04 = io_read_uint8_t(s);
    chr_eff_a3da->u05 = io_read_uint8_t(s);
    chr_eff_a3da->u06 = io_read_uint8_t(s);
    chr_eff_a3da->u07 = io_read_uint8_t(s);
    size_t a3da_offset = io_read_offset_x(s);
    size_t o10 = io_read_offset_x(s);

    io_position_push(s, a3da_offset, SEEK_SET);
    pvpp_a3da_read(&chr_eff_a3da->a3da, s);
    io_position_pop(s);
}

static void pvpp_chara_item_read(pvpp_chara_item* chr_itm, stream* s) {
    uint8_t a3das_count = io_read_uint8_t(s);
    size_t a3das_offset = io_read_offset_x(s);
    size_t bone_offset = io_read_offset_x(s);
    chr_itm->u18.x = io_read_float_t_stream_reverse_endianness(s);
    chr_itm->u18.y = io_read_float_t_stream_reverse_endianness(s);
    chr_itm->u18.z = io_read_float_t_stream_reverse_endianness(s);
    chr_itm->u18.w = io_read_float_t_stream_reverse_endianness(s);

    vector_pvpp_a3da_reserve(&chr_itm->a3da, a3das_count);
    chr_itm->a3da.end += a3das_count;

    io_position_push(s, a3das_offset, SEEK_SET);
    for (int32_t k = 0; k < a3das_count; k++)
        pvpp_a3da_read(&chr_itm->a3da.begin[k], s);
    io_position_pop(s);

    io_read_string_null_terminated_offset(s, bone_offset, &chr_itm->bone);
}

static void pvpp_effect_read(pvpp_effect* eff, stream* s) {
    uint8_t a3da_count = io_read_uint8_t(s);
    uint8_t effect_count = io_read_uint8_t(s);
    eff->chara_index = (pvpp_chara_type)io_read_uint8_t(s);
    ssize_t a3da_offset = io_read_offset_x(s);
    ssize_t effect_offset = io_read_offset_x(s);

    vector_pvpp_a3da_reserve(&eff->a3da, a3da_count);
    eff->a3da.end += a3da_count;

    io_position_push(s, a3da_offset, SEEK_SET);
    for (int32_t i = 0; i < a3da_count; i++)
        pvpp_a3da_read(&eff->a3da.begin[i], s);
    io_position_pop(s);

    vector_pvpp_glitter_reserve(&eff->glitter, effect_count);
    eff->glitter.end += effect_count;

    io_position_push(s, effect_offset, SEEK_SET);
    for (int32_t i = 0; i < effect_count; i++)
        pvpp_glitter_read(&eff->glitter.begin[i], s);
    io_position_pop(s);
}

static void pvpp_glitter_read(pvpp_glitter* glt, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &glt->name);
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &glt->unk1);
    glt->unk2 = io_read_uint8_t(s) ? true : false;
    io_align_read(s, 0x08);
}

static void pvpp_motion_read(pvpp_motion* mot, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &mot->name);
    mot->hash = io_read_uint32_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
}

static void pvpp_read_inner(pvpp* pp, stream* s) {
    f2_struct st;
    f2_struct_sread(&st, s);
    if (st.header.signature == reverse_endianness_uint32_t('PVPP') && st.data) {
        stream s_pvpp;
        io_mopen(&s_pvpp, st.data, st.length);
        s_pvpp.is_big_endian = st.header.use_big_endian;

        io_read_uint32_t_stream_reverse_endianness(&s_pvpp);
        uint8_t chara_count = io_read_uint8_t(&s_pvpp);
        ssize_t effect_offset = io_read_offset_x(&s_pvpp);
        ssize_t chara_offset = io_read_offset_x(&s_pvpp);
        io_read_offset_x(&s_pvpp);

        vector_pvpp_chara_reserve(&pp->chara, chara_count);
        pp->chara.end += chara_count;

        io_position_push(&s_pvpp, chara_offset, SEEK_SET);
        for (int32_t i = 0; i < chara_count; i++)
            pvpp_chara_read(&pp->chara.begin[i], &s_pvpp);
        io_position_pop(&s_pvpp);

        io_position_push(&s_pvpp, effect_offset, SEEK_SET);
        io_read_uint8_t(&s_pvpp);
        uint8_t efffect_count = io_read_uint8_t(&s_pvpp);
        effect_offset = io_read_offset_x(&s_pvpp);
        io_position_pop(&s_pvpp);

        vector_pvpp_effect_reserve(&pp->effect, efffect_count);
        pp->effect.end += efffect_count;

        io_position_push(&s_pvpp, effect_offset, SEEK_SET);
        for (int32_t i = 0; i < efffect_count; i++)
            pvpp_effect_read(&pp->effect.begin[i], &s_pvpp);
        io_position_pop(&s_pvpp);

        io_free(&s_pvpp);

        pp->ready = true;
    }
    f2_struct_free(&st);
}
