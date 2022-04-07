/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvsr.h"
#include "f2/struct.h"
#include "io/path.h"
#include "io/stream.h"
#include "hash.h"
#include "str_utils.h"

static void pvsr_a3da_read(pvsr_a3da* a3d, stream* s);
static void pvsr_aet_read(pvsr_aet* aet_entry, stream* s);
static void pvsr_effect_read(pvsr_effect* eff, stream* s);
static void pvsr_glitter_read(pvsr_glitter* glt, stream* s);
static void pvsr_read_inner(pvsr* sr, stream* s);
static void pvsr_stage_effect_read(pvsr_stage_effect* stg_eff, stream* s);
static void pvsr_stage_effect_env_read(pvsr_stage_effect_env* aet, stream* s, int32_t x00);
static bool pvsr_stage_effect_env_sub1_read(pvsr_stage_effect_env_sub1* aet_sub1, stream* s, ssize_t offset);
static bool pvsr_stage_effect_env_sub2_read(pvsr_stage_effect_env_sub2* aet_sub2, stream* s, ssize_t offset);

pvsr::pvsr() : ready(), stage_change_effect_init() {

}

pvsr::~pvsr() {

}

void pvsr::read(const char* path) {
    if (!path)
        return;

    char* path_pvsr = str_utils_add(path, ".pvsr");
    if (path_check_file_exists(path_pvsr)) {
        stream s;
        io_open(&s, path_pvsr, "rb");
        if (s.io.stream)
            pvsr_read_inner(this, &s);
        io_free(&s);
    }
    free(path_pvsr);
}

void pvsr::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_pvsr = str_utils_add(path, L".pvsr");
    if (path_check_file_exists(path_pvsr)) {
        stream s;
        io_open(&s, path_pvsr, L"rb");
        if (s.io.stream)
            pvsr_read_inner(this, &s);
        io_free(&s);
    }
    free(path_pvsr);
}

bool pvsr::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    pvsr* sr = (pvsr*)data;
    sr->read(string_data(&s));

    string_free(&s);
    return sr->ready;
}

pvsr_a3da::pvsr_a3da() : hash(hash_murmurhash_empty) {

}

pvsr_a3da::~pvsr_a3da() {

}

pvsr_aet::pvsr_aet() : bright_scale(1.0f), hash(hash_murmurhash_empty) {

}

pvsr_aet::~pvsr_aet() {

}

pvsr_effect::pvsr_effect() : emission(1.0f) {

}

pvsr_effect::~pvsr_effect() {

}

pvsr_glitter::pvsr_glitter() : unk1() {

}

pvsr_glitter::~pvsr_glitter() {

}

pvsr_stage_effect::pvsr_stage_effect() : bar_count() {

}

pvsr_stage_effect::~pvsr_stage_effect() {

}

pvsr_stage_effect_env_sub1::pvsr_stage_effect_env_sub1() : hash(hash_murmurhash_empty), unk2() {

}

pvsr_stage_effect_env_sub1::~pvsr_stage_effect_env_sub1() {

}

pvsr_stage_effect_env::pvsr_stage_effect_env() : u48(), u4a(), u4c(), u4e(), u50(), u52(), u54(), u56(), u58(), u5a(),
u5c(), u5e(), u60(), sub1_data(), sub1_data_init(), sub2a_data(), sub2a_data_init(),
sub2b_data(), sub2b_data_init(), sub2c_data(), sub2c_data_init(), sub2d_data(), sub2d_data_init() {

}

pvsr_stage_effect_env::~pvsr_stage_effect_env() {

}

static void pvsr_a3da_read(pvsr_a3da* a3d, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &a3d->name);
    a3d->hash = io_read_uint32_t_stream_reverse_endianness(s);
    io_align_read(s, 0x08);
}

static void pvsr_aet_read(pvsr_aet* aet_entry, stream* s) {
    io_read_string_null_terminated_offset(s, io_read_offset_x(s), &aet_entry->name);
    aet_entry->hash = io_read_uint32_t_stream_reverse_endianness(s);
    aet_entry->bright_scale = io_read_float_t_stream_reverse_endianness(s);
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
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('PVSR') || !st.data.data())
        return;

    stream s_pvsr;
    io_open(&s_pvsr, &st.data);
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

    sr->effect.resize(effect_count);

    io_position_push(&s_pvsr, effect_offset, SEEK_SET);
    for (pvsr_effect& i : sr->effect)
        pvsr_effect_read(&i, &s_pvsr);
    io_position_pop(&s_pvsr);

    sr->emcs.resize(emcs_count);

    io_position_push(&s_pvsr, emcs_offset, SEEK_SET);
    for (std::string& i : sr->emcs)
        io_read_string_null_terminated_offset(&s_pvsr,
            io_read_offset_x(&s_pvsr), &i);
    io_position_pop(&s_pvsr);

    sr->stage_effect.resize(stage_effect_count);

    io_position_push(&s_pvsr, stage_effect_offset, SEEK_SET);
    for (pvsr_stage_effect& i : sr->stage_effect)
        pvsr_stage_effect_read(&i, &s_pvsr);
    io_position_pop(&s_pvsr);

    for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++)
            sr->stage_change_effect_init[i][j] = false;

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

    sr->stage_effect_env.resize(aets_count);

    io_position_push(&s_pvsr, aet_offset, SEEK_SET);
    for (pvsr_stage_effect_env& i : sr->stage_effect_env)
        pvsr_stage_effect_env_read(&i, &s_pvsr, x00);
    io_position_pop(&s_pvsr);

    io_free(&s_pvsr);

    sr->ready = true;
}

static void pvsr_stage_effect_read(pvsr_stage_effect* stg_eff, stream* s) {
    uint8_t u00 = io_read_uint8_t(s);
    uint8_t u01 = io_read_uint8_t(s);
    uint8_t u02 = io_read_uint8_t(s);
    uint8_t u03 = io_read_uint8_t(s);
    stg_eff->bar_count = io_read_uint8_t(s);
    ssize_t a3da_offset = io_read_offset_x(s);
    ssize_t glitter_offset = io_read_offset_x(s);
    io_read_offset_x(s);

    uint8_t a3da_count;
    uint8_t glitter_count;
    if (stg_eff->bar_count != 0xFF) {
        a3da_count = u00;
        glitter_count = u01;
    }
    else {
        a3da_count = u01;
        glitter_count = u02;
    }

    stg_eff->a3da.resize(a3da_count);

    io_position_push(s, a3da_offset, SEEK_SET);
    for (pvsr_a3da& i : stg_eff->a3da)
        pvsr_a3da_read(&i, s);
    io_position_pop(s);

    stg_eff->glitter.resize(glitter_count);

    io_position_push(s, glitter_offset, SEEK_SET);
    for (pvsr_glitter& i : stg_eff->glitter)
        pvsr_glitter_read(&i, s);
    io_position_pop(s);
}

static void pvsr_stage_effect_env_read(pvsr_stage_effect_env* aet, stream* s, int32_t x00) {
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

    aet->aet_front.resize(front_count);

    io_position_push(s, front_offset, SEEK_SET);
    for (pvsr_aet& i : aet->aet_front)
        pvsr_aet_read(&i, s);
    io_position_pop(s);

    aet->aet_front_low.resize(front_low_count);

    io_position_push(s, front_low_offset, SEEK_SET);
    for (pvsr_aet& i : aet->aet_front_low)
        pvsr_aet_read(&i, s);
    io_position_pop(s);

    aet->aet_back.resize(back_count);

    io_position_push(s, back_offset, SEEK_SET);
    for (pvsr_aet& i : aet->aet_back)
        pvsr_aet_read(&i, s);
    io_position_pop(s);

    aet->sub1_data_init = pvsr_stage_effect_env_sub1_read(&aet->sub1_data, s, sub1_offset);
    aet->sub2a_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2a_data, s, sub2a_offset);
    aet->sub2b_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2b_data, s, sub2b_offset);
    aet->sub2c_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2c_data, s, sub2c_offset);
    aet->sub2d_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2d_data, s, sub2d_offset);

    if (x00 & 0x100) {
        aet->unk03.resize(u78);

        io_position_push(s, o68, SEEK_SET);
        for (pvsr_aet& i : aet->unk03)
            pvsr_aet_read(&i, s);
        io_position_pop(s);

        aet->unk04.resize(u79);

        io_position_push(s, o70, SEEK_SET);
        for (pvsr_aet& i : aet->unk04)
            pvsr_aet_read(&i, s);
        io_position_pop(s);
    }
}

static bool pvsr_stage_effect_env_sub1_read(pvsr_stage_effect_env_sub1* aet_sub1, stream* s, ssize_t offset) {
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

static bool pvsr_stage_effect_env_sub2_read(pvsr_stage_effect_env_sub2* aet_sub2, stream* s, ssize_t offset) {
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
