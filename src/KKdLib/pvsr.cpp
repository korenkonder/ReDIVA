/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvsr.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/stream.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

static void pvsr_a3da_read(pvsr_a3da* a3d, stream& s);
static void pvsr_aet_read(pvsr_aet* aet_entry, stream& s);
static void pvsr_effect_read(pvsr_effect* eff, stream& s);
static void pvsr_glitter_read(pvsr_glitter* glt, stream& s);
static void pvsr_read_inner(pvsr* sr, stream& s);
static void pvsr_stage_effect_read(pvsr_stage_effect* stg_eff, stream& s);
static void pvsr_stage_effect_env_read(pvsr_stage_effect_env* aet, stream& s, int32_t x00);
static bool pvsr_stage_effect_env_sub1_read(pvsr_stage_effect_env_sub1* aet_sub1, stream& s, int64_t offset);
static bool pvsr_stage_effect_env_sub2_read(pvsr_stage_effect_env_sub2* aet_sub2, stream& s, int64_t offset);

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
        s.open(path_pvsr, "rb");
        if (s.io.stream)
            pvsr_read_inner(this, s);
    }
    free(path_pvsr);
}

void pvsr::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_pvsr = str_utils_add(path, L".pvsr");
    if (path_check_file_exists(path_pvsr)) {
        stream s;
        s.open(path_pvsr, L"rb");
        if (s.io.stream)
            pvsr_read_inner(this, s);
    }
    free(path_pvsr);
}

bool pvsr::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    pvsr* sr = (pvsr*)data;
    sr->read(s.c_str());

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

static void pvsr_a3da_read(pvsr_a3da* a3d, stream& s) {
    a3d->name = s.read_string_null_terminated_offset(s.read_offset_x());
    a3d->hash = s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvsr_aet_read(pvsr_aet* aet_entry, stream& s) {
    aet_entry->name = s.read_string_null_terminated_offset(s.read_offset_x());
    aet_entry->hash = s.read_uint32_t_reverse_endianness();
    aet_entry->bright_scale = s.read_float_t_reverse_endianness();
}

static void pvsr_effect_read(pvsr_effect* eff, stream& s) {
    eff->name = s.read_string_null_terminated_offset(s.read_offset_x());
    eff->emission = s.read_float_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvsr_glitter_read(pvsr_glitter* glt, stream& s) {
    glt->name = s.read_string_null_terminated_offset(s.read_offset_x());
    glt->unk1 = s.read_uint8_t();
    s.align_read(0x08);
}

static void pvsr_read_inner(pvsr* sr, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('PVSR') || !st.data.data())
        return;

    stream s_pvsr;
    s_pvsr.open(st.data);
    s_pvsr.is_big_endian = st.header.use_big_endian;

    int32_t x00 = s_pvsr.read_int32_t_reverse_endianness();
    uint8_t x04 = s_pvsr.read_uint8_t();
    uint8_t x05 = s_pvsr.read_uint8_t();
    uint8_t x06 = s_pvsr.read_uint8_t();
    uint8_t x07 = s_pvsr.read_uint8_t();
    int8_t stage_effect_count = s_pvsr.read_int8_t();
    int8_t stage_effect_envs_count = s_pvsr.read_int8_t();
    uint8_t x0a = s_pvsr.read_uint8_t();
    uint8_t x0b = s_pvsr.read_uint8_t();
    uint8_t x0c = s_pvsr.read_uint8_t();
    uint8_t x0d = s_pvsr.read_uint8_t();
    uint8_t x0e = s_pvsr.read_uint8_t();
    uint8_t x0f = s_pvsr.read_uint8_t();
    int64_t x10 = s_pvsr.read_offset_x();
    int64_t stage_effect_offset = s_pvsr.read_offset_x();
    int64_t stage_change_effect_offset = s_pvsr.read_offset_x();
    int64_t stage_effect_env_offset = s_pvsr.read_offset_x();

    s_pvsr.position_push(x10, SEEK_SET);
    int8_t unknown_count = s_pvsr.read_int8_t();
    int8_t effect_count = s_pvsr.read_int8_t();
    int8_t emcs_count = s_pvsr.read_int8_t();
    int64_t unknown_offset = s_pvsr.read_offset_x();
    int64_t effect_offset = s_pvsr.read_offset_x();
    int64_t emcs_offset = s_pvsr.read_offset_x();
    s_pvsr.position_pop();

    sr->effect.resize(effect_count);

    s_pvsr.position_push(effect_offset, SEEK_SET);
    for (pvsr_effect& i : sr->effect)
        pvsr_effect_read(&i, s_pvsr);
    s_pvsr.position_pop();

    sr->emcs.resize(emcs_count);

    s_pvsr.position_push(emcs_offset, SEEK_SET);
    for (std::string& i : sr->emcs)
        i = s_pvsr.read_string_null_terminated_offset(s_pvsr.read_offset_x());
    s_pvsr.position_pop();

    sr->stage_effect.resize(stage_effect_count);

    s_pvsr.position_push(stage_effect_offset, SEEK_SET);
    for (pvsr_stage_effect& i : sr->stage_effect)
        pvsr_stage_effect_read(&i, s_pvsr);
    s_pvsr.position_pop();

    for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++)
            sr->stage_change_effect_init[i][j] = false;

    s_pvsr.position_push(stage_change_effect_offset, SEEK_SET);
    for (int32_t i = 0; i < PVSR_STAGE_CHANGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_CHANGE_EFFECT_COUNT; j++) {
            int64_t offset = s_pvsr.read_offset_x();
            if (offset <= 0)
                continue;

            s_pvsr.position_push(offset, SEEK_SET);
            pvsr_stage_effect_read(&sr->stage_change_effect[i][j], s_pvsr);
            sr->stage_change_effect_init[i][j] = true;
            s_pvsr.position_pop();
        }
    s_pvsr.position_pop();

    sr->stage_effect_env.resize(stage_effect_envs_count);

    s_pvsr.position_push(stage_effect_env_offset, SEEK_SET);
    for (pvsr_stage_effect_env& i : sr->stage_effect_env)
        pvsr_stage_effect_env_read(&i, s_pvsr, x00);
    s_pvsr.position_pop();

    sr->ready = true;
}

static void pvsr_stage_effect_read(pvsr_stage_effect* stg_eff, stream& s) {
    uint8_t u00 = s.read_uint8_t();
    uint8_t u01 = s.read_uint8_t();
    uint8_t u02 = s.read_uint8_t();
    uint8_t u03 = s.read_uint8_t();
    stg_eff->bar_count = s.read_uint8_t();
    int64_t a3da_offset = s.read_offset_x();
    int64_t glitter_offset = s.read_offset_x();
    s.read_offset_x();

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

    s.position_push(a3da_offset, SEEK_SET);
    for (pvsr_a3da& i : stg_eff->a3da)
        pvsr_a3da_read(&i, s);
    s.position_pop();

    stg_eff->glitter.resize(glitter_count);

    s.position_push(glitter_offset, SEEK_SET);
    for (pvsr_glitter& i : stg_eff->glitter)
        pvsr_glitter_read(&i, s);
    s.position_pop();
}

static void pvsr_stage_effect_env_read(pvsr_stage_effect_env* aet, stream& s, int32_t x00) {
    int64_t set_name_offset = s.read_offset_x();
    int64_t front_offset = s.read_offset_x();
    int64_t front_low_offset = s.read_offset_x();
    int64_t back_offset = s.read_offset_x();
    int64_t sub1_offset = s.read_offset_x();
    int64_t sub2a_offset = s.read_offset_x();
    int64_t sub2b_offset = s.read_offset_x();
    int64_t sub2c_offset = s.read_offset_x();
    int64_t sub2d_offset = s.read_offset_x();
    aet->u48 = s.read_uint16_t_reverse_endianness();
    aet->u4a = s.read_uint16_t_reverse_endianness();
    aet->u4c = s.read_uint16_t_reverse_endianness();
    aet->u4e = s.read_uint16_t_reverse_endianness();
    aet->u50 = s.read_uint16_t_reverse_endianness();
    aet->u52 = s.read_uint16_t_reverse_endianness();
    aet->u54 = s.read_uint16_t_reverse_endianness();
    aet->u56 = s.read_uint16_t_reverse_endianness();
    aet->u58 = s.read_uint16_t_reverse_endianness();
    aet->u5a = s.read_uint16_t_reverse_endianness();
    aet->u5c = s.read_uint16_t_reverse_endianness();
    aet->u5e = s.read_uint16_t_reverse_endianness();
    aet->u60 = s.read_uint16_t_reverse_endianness();
    int8_t front_count = s.read_int8_t();
    int8_t front_low_count = s.read_int8_t();
    int8_t back_count = s.read_int8_t();
    s.align_read(0x08);
    int64_t o68 = 0;
    int64_t o70 = 0;
    int8_t u78 = 0;
    int8_t u79 = 0;
    if (x00 & 0x100) {
        o68 = s.read_offset_x();
        o70 = s.read_offset_x();
        u78 = s.read_int8_t();
        u79 = s.read_int8_t();
        s.align_read(0x08);
    }

    aet->set_name = s.read_string_null_terminated_offset(set_name_offset);

    aet->aet_front.resize(front_count);

    s.position_push(front_offset, SEEK_SET);
    for (pvsr_aet& i : aet->aet_front)
        pvsr_aet_read(&i, s);
    s.position_pop();

    aet->aet_front_low.resize(front_low_count);

    s.position_push(front_low_offset, SEEK_SET);
    for (pvsr_aet& i : aet->aet_front_low)
        pvsr_aet_read(&i, s);
    s.position_pop();

    aet->aet_back.resize(back_count);

    s.position_push(back_offset, SEEK_SET);
    for (pvsr_aet& i : aet->aet_back)
        pvsr_aet_read(&i, s);
    s.position_pop();

    aet->sub1_data_init = pvsr_stage_effect_env_sub1_read(&aet->sub1_data, s, sub1_offset);
    aet->sub2a_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2a_data, s, sub2a_offset);
    aet->sub2b_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2b_data, s, sub2b_offset);
    aet->sub2c_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2c_data, s, sub2c_offset);
    aet->sub2d_data_init = pvsr_stage_effect_env_sub2_read(&aet->sub2d_data, s, sub2d_offset);

    if (x00 & 0x100) {
        aet->unk03.resize(u78);

        s.position_push(o68, SEEK_SET);
        for (pvsr_aet& i : aet->unk03)
            pvsr_aet_read(&i, s);
        s.position_pop();

        aet->unk04.resize(u79);

        s.position_push(o70, SEEK_SET);
        for (pvsr_aet& i : aet->unk04)
            pvsr_aet_read(&i, s);
        s.position_pop();
    }
}

static bool pvsr_stage_effect_env_sub1_read(pvsr_stage_effect_env_sub1* aet_sub1, stream& s, int64_t offset) {
    if (offset <= 0)
        return false;

    s.position_push(offset, SEEK_SET);
    aet_sub1->name = s.read_string_null_terminated_offset(s.read_offset_x());
    aet_sub1->hash = s.read_uint32_t_reverse_endianness();
    aet_sub1->unk2 = s.read_uint16_t_reverse_endianness();
    s.align_read(0x08);
    s.position_pop();
    return true;
}

static bool pvsr_stage_effect_env_sub2_read(pvsr_stage_effect_env_sub2* aet_sub2, stream& s, int64_t offset) {
    if (offset <= 0)
        return false;

    s.position_push(offset, SEEK_SET);
    aet_sub2->u00 = s.read_uint16_t_reverse_endianness();
    aet_sub2->u02 = s.read_uint16_t_reverse_endianness();
    aet_sub2->u04 = s.read_uint16_t_reverse_endianness();
    aet_sub2->u06 = s.read_uint16_t_reverse_endianness();
    aet_sub2->u08 = s.read_uint16_t_reverse_endianness();
    aet_sub2->u0a = s.read_uint16_t_reverse_endianness();
    aet_sub2->u0c = s.read_uint16_t_reverse_endianness();
    s.align_read(0x08);
    s.position_pop();
    return true;
}
