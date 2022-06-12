/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvpp.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/stream.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

static void pvpp_a3da_read(pvpp_a3da* a3d, stream& s);
static void pvpp_chara_read(pvpp_chara* chr, stream& s);
static void pvpp_chara_effect_read(pvpp_chara_effect* chr_eff, stream& s);
static void pvpp_chara_effect_a3da_read(pvpp_chara_effect_a3da* chr_pv_eff, stream& s);
static void pvpp_chara_item_read(pvpp_chara_item* chr_itm, stream& s);
static void pvpp_effect_read(pvpp_effect* eff, stream& s);
static void pvpp_glitter_read(pvpp_glitter* glt, stream& s);
static void pvpp_motion_read(pvpp_motion* mot, stream& s);
static void pvpp_object_set_read(pvpp_object_set* objset, stream& s);
static void pvpp_read_inner(pvpp* pp, stream& s);

pvpp::pvpp() : ready() {

}

pvpp::~pvpp() {

}

void pvpp::read(const char* path) {
    if (!path)
        return;

    char* path_pvpp = str_utils_add(path, ".pvpp");
    if (path_check_file_exists(path_pvpp)) {
        stream s;
        s.open(path_pvpp, "rb");
        if (s.io.stream)
            pvpp_read_inner(this, s);
    }
    free(path_pvpp);
}

void pvpp::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_pvpp = str_utils_add(path, L".pvpp");
    if (path_check_file_exists(path_pvpp)) {
        stream s;
        s.open(path_pvpp, L"rb");
        if (s.io.stream)
            pvpp_read_inner(this, s);
    }
    free(path_pvpp);
}

void pvpp::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    stream s;
    s.open(data, size);
    pvpp_read_inner(this, s);
}

bool pvpp::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    pvpp* pp = (pvpp*)data;
    pp->read(s.c_str());

    return pp->ready;
}

pvpp_a3da::pvpp_a3da() : hash(hash_murmurhash_empty) {

}

pvpp_a3da::~pvpp_a3da() {

}

pvpp_chara::pvpp_chara() : chara_effect_init() {

}

pvpp_chara::~pvpp_chara() {

}

pvpp_chara_effect::pvpp_chara_effect() : base_chara(), chara_index() {

}

pvpp_chara_effect::~pvpp_chara_effect() {

}

pvpp_chara_effect_a3da::pvpp_chara_effect_a3da() : has_object_set(),
u00(), u01(), u02(), u03(), u04(), u05(), u06(), u07() {

}

pvpp_chara_effect_a3da::~pvpp_chara_effect_a3da() {

}

pvpp_chara_item::pvpp_chara_item() : u18() {

}

pvpp_chara_item::~pvpp_chara_item() {

}

pvpp_effect::pvpp_effect() : chara_index() {

}

pvpp_effect::~pvpp_effect() {

}

pvpp_glitter::pvpp_glitter() : unk2() {

}

pvpp_glitter::~pvpp_glitter() {

}

pvpp_motion::pvpp_motion() : hash(hash_murmurhash_empty) {

}

pvpp_motion::~pvpp_motion() {

}

pvpp_object_set::pvpp_object_set() : hash(hash_murmurhash_empty) {

}

pvpp_object_set::~pvpp_object_set() {

}

static void pvpp_a3da_read(pvpp_a3da* a3d, stream& s) {
    a3d->name = s.read_string_null_terminated_offset(s.read_offset_x());
    a3d->hash = s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_chara_read(pvpp_chara* chr, stream& s) {
    int8_t u00 = s.read_int8_t();
    int8_t u01 = s.read_int8_t();
    int8_t motion_count = s.read_int8_t();
    int8_t a3da_count = s.read_int8_t();
    int8_t item_count = s.read_int8_t();
    int8_t u05 = s.read_int8_t();
    int8_t glitter_count = s.read_int8_t();
    int8_t u07 = s.read_int8_t();
    size_t chr_eff_offset = s.read_offset_x();
    size_t motion_offset = s.read_offset_x();
    size_t a3da_offset = s.read_offset_x();
    size_t item_offset = s.read_offset_x();
    size_t glitter_offset = s.read_offset_x();
    size_t o30 = s.read_offset_x();

    chr->chara_effect_init = false;
    if (chr_eff_offset) {
        s.position_push(chr_eff_offset, SEEK_SET);
        pvpp_chara_effect_read(&chr->chara_effect, s);
        chr->chara_effect_init = true;
        s.position_pop();
    }

    chr->motion.resize(motion_count);

    s.position_push(motion_offset, SEEK_SET);
    for (pvpp_motion& i : chr->motion) {
        s.position_push(s.read_offset_x(), SEEK_SET);
        pvpp_motion_read(&i, s);
        s.position_pop();
    }
    s.position_pop();

    chr->a3da.resize(a3da_count);

    s.position_push(a3da_offset, SEEK_SET);
    for (pvpp_a3da& i : chr->a3da)
        pvpp_a3da_read(&i, s);
    s.position_pop();

    chr->item.resize(item_count);

    s.position_push(item_offset, SEEK_SET);
    for (pvpp_chara_item& i : chr->item)
        pvpp_chara_item_read(&i, s);
    s.position_pop();

    chr->glitter.resize(glitter_count);

    s.position_push(glitter_offset, SEEK_SET);
    for (pvpp_glitter& i : chr->glitter)
        pvpp_glitter_read(&i, s);
    s.position_pop();
}

static void pvpp_chara_effect_read(pvpp_chara_effect* chr_eff, stream& s) {
    chr_eff->base_chara = (pvpp_chara_type)s.read_uint8_t();
    int8_t pv_effects_count = s.read_int8_t();
    chr_eff->chara_index = (pvpp_chara_index)s.read_uint8_t();
    size_t pv_effects_offstet = s.read_offset_x();

    chr_eff->effect_a3da.resize(pv_effects_count);

    s.position_push(pv_effects_offstet, SEEK_SET);
    for (pvpp_chara_effect_a3da& i : chr_eff->effect_a3da)
        pvpp_chara_effect_a3da_read(&i, s);
    s.position_pop();
}

static void pvpp_chara_effect_a3da_read(pvpp_chara_effect_a3da* chr_eff_a3da, stream& s) {
    chr_eff_a3da->u00 = s.read_uint8_t();
    chr_eff_a3da->u01 = s.read_uint8_t();
    chr_eff_a3da->u02 = s.read_uint8_t();
    chr_eff_a3da->u03 = s.read_uint8_t();
    chr_eff_a3da->u04 = s.read_uint8_t();
    chr_eff_a3da->u05 = s.read_uint8_t();
    chr_eff_a3da->u06 = s.read_uint8_t();
    chr_eff_a3da->u07 = s.read_uint8_t();
    size_t a3da_offset = s.read_offset_x();
    size_t source_a3da_offset = s.read_offset_x();

    s.position_push(a3da_offset, SEEK_SET);
    pvpp_a3da_read(&chr_eff_a3da->a3da, s);
    s.position_pop();

    chr_eff_a3da->has_object_set = false;
    if (source_a3da_offset) {
        s.position_push(source_a3da_offset, SEEK_SET);
        pvpp_object_set_read(&chr_eff_a3da->object_set, s);
        chr_eff_a3da->has_object_set = true;
        s.position_pop();
    }
}

static void pvpp_chara_item_read(pvpp_chara_item* chr_itm, stream& s) {
    int8_t a3das_count = s.read_int8_t();
    size_t a3das_offset = s.read_offset_x();
    size_t bone_offset = s.read_offset_x();
    chr_itm->u18.x = s.read_float_t_reverse_endianness();
    chr_itm->u18.y = s.read_float_t_reverse_endianness();
    chr_itm->u18.z = s.read_float_t_reverse_endianness();
    chr_itm->u18.w = s.read_float_t_reverse_endianness();

    chr_itm->a3da.resize(a3das_count);

    s.position_push(a3das_offset, SEEK_SET);
    for (pvpp_a3da& i : chr_itm->a3da)
        pvpp_a3da_read(&i, s);
    s.position_pop();

    chr_itm->bone = s.read_string_null_terminated_offset(bone_offset);
}

static void pvpp_effect_read(pvpp_effect* eff, stream& s) {
    int8_t a3das_count = s.read_int8_t();
    int8_t glitters_count = s.read_int8_t();
    eff->chara_index = (pvpp_chara_type)s.read_uint8_t();
    int64_t a3das_offset = s.read_offset_x();
    int64_t glitters_offset = s.read_offset_x();

    eff->a3da.resize(a3das_count);

    s.position_push(a3das_offset, SEEK_SET);
    for (pvpp_a3da& i : eff->a3da)
        pvpp_a3da_read(&i, s);
    s.position_pop();

    eff->glitter.resize(glitters_count);

    s.position_push(glitters_offset, SEEK_SET);
    for (pvpp_glitter& i : eff->glitter)
        pvpp_glitter_read(&i, s);
    s.position_pop();
}

static void pvpp_glitter_read(pvpp_glitter* glt, stream& s) {
    glt->name = s.read_string_null_terminated_offset(s.read_offset_x());
    glt->unk1 = s.read_string_null_terminated_offset(s.read_offset_x());
    glt->unk2 = s.read_uint8_t() ? true : false;
    s.align_read(0x08);
}

static void pvpp_motion_read(pvpp_motion* mot, stream& s) {
    mot->name = s.read_string_null_terminated_offset(s.read_offset_x());
    mot->hash = s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_object_set_read(pvpp_object_set* objset, stream& s) {
    objset->name = s.read_string_null_terminated_offset(s.read_offset_x());
    objset->hash = s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_read_inner(pvpp* pp, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('PVPP') || !st.data.data())
        return;

    stream s_pvpp;
    s_pvpp.open(st.data);
    s_pvpp.is_big_endian = st.header.use_big_endian;

    s_pvpp.read_uint32_t_reverse_endianness();
    int8_t chara_count = s_pvpp.read_int8_t();
    int64_t effect_offset = s_pvpp.read_offset_x();
    int64_t chara_offset = s_pvpp.read_offset_x();
    s_pvpp.read_offset_x();

    pp->chara.resize(chara_count);

    s_pvpp.position_push(chara_offset, SEEK_SET);
    for (pvpp_chara& i : pp->chara)
        pvpp_chara_read(&i, s_pvpp);
    s_pvpp.position_pop();

    s_pvpp.position_push( effect_offset, SEEK_SET);
    s_pvpp.read_uint8_t();
    int8_t efffect_count = s_pvpp.read_int8_t();
    effect_offset = s_pvpp.read_offset_x();
    s_pvpp.position_pop();

    pp->effect.resize(efffect_count);

    s_pvpp.position_push(effect_offset, SEEK_SET);
    for (pvpp_effect& i : pp->effect)
        pvpp_effect_read(&i, s_pvpp);
    s_pvpp.position_pop();

    pp->ready = true;
}
