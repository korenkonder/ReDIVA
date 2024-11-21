/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvpp.hpp"
#include "f2/struct.hpp"
#include "io/file_stream.hpp"
#include "io/memory_stream.hpp"
#include "io/path.hpp"
#include "str_utils.hpp"

static void pvpp_auth_3d_read(string_hash* a3d, stream& s);
static void pvpp_chara_read(pvpp_chara* chr, stream& s);
static void pvpp_chara_effect_read(pvpp_chara_effect* chr_eff, stream& s);
static void pvpp_chara_effect_auth_3d_read(pvpp_chara_effect_auth_3d* chr_pv_eff, stream& s);
static void pvpp_chara_item_read(pvpp_chara_item* chr_itm, stream& s);
static void pvpp_chara_item_auth_3d_read(string_hash* a3d, stream& s);
static void pvpp_effect_read(pvpp_effect* eff, stream& s);
static void pvpp_glitter_read(pvpp_glitter* glt, stream& s);
static void pvpp_motion_read(string_hash* mot, stream& s);
static void pvpp_object_set_read(string_hash* objset, stream& s);
static void pvpp_read_inner(pvpp* pp, stream& s);

pvpp_chara_effect_auth_3d::pvpp_chara_effect_auth_3d() : has_object_set(),
u00(), u01(), u02(), u03(), u04(), u05(), u06(), u07() {

}

pvpp_chara_effect_auth_3d::~pvpp_chara_effect_auth_3d() {

}

pvpp_chara_effect::pvpp_chara_effect() : base_chara(), chara_id() {

}

pvpp_chara_effect::~pvpp_chara_effect() {

}

pvpp_chara_item::pvpp_chara_item() : type(), u18() {

}

pvpp_chara_item::~pvpp_chara_item() {

}

pvpp_glitter::pvpp_glitter() : unk2() {

}

pvpp_glitter::~pvpp_glitter() {

}

pvpp_effect::pvpp_effect() : chara_id() {

}

pvpp_effect::~pvpp_effect() {

}

pvpp_chara::pvpp_chara() : chara_effect_init() {

}

pvpp_chara::~pvpp_chara() {

}

pvpp::pvpp() : ready(), big_endian() {

}

pvpp::~pvpp() {

}

void pvpp::read(const char* path) {
    if (!path)
        return;

    char* path_pvpp = str_utils_add(path, ".pvpp");
    if (path_check_file_exists(path_pvpp)) {
        file_stream s;
        s.open(path_pvpp, "rb");
        if (s.check_not_null())
            pvpp_read_inner(this, s);
    }
    free_def(path_pvpp);
}

void pvpp::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_pvpp = str_utils_add(path, L".pvpp");
    if (path_check_file_exists(path_pvpp)) {
        file_stream s;
        s.open(path_pvpp, L"rb");
        if (s.check_not_null())
            pvpp_read_inner(this, s);
    }
    free_def(path_pvpp);
}

void pvpp::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    pvpp_read_inner(this, s);
}

bool pvpp::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    pvpp* pp = (pvpp*)data;
    pp->read(path.c_str());

    return pp->ready;
}

static void pvpp_auth_3d_read(string_hash* a3d, stream& s) {
    *a3d = s.read_string_null_terminated_offset(s.read_offset_x());
    s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_chara_read(pvpp_chara* chr, stream& s) {
    int8_t u00 = s.read_int8_t();
    int8_t u01 = s.read_int8_t();
    int8_t motion_count = s.read_int8_t();
    int8_t auth_3d_count = s.read_int8_t();
    int8_t item_count = s.read_int8_t();
    int8_t u05 = s.read_int8_t();
    int8_t glitter_count = s.read_int8_t();
    int8_t u07 = s.read_int8_t();
    int64_t chr_eff_offset = s.read_offset_x();
    int64_t motion_offset = s.read_offset_x();
    int64_t auth_3d_offset = s.read_offset_x();
    int64_t item_offset = s.read_offset_x();
    int64_t glitter_offset = s.read_offset_x();
    int64_t o30 = s.read_offset_x();

    chr->chara_effect_init = false;
    if (chr_eff_offset > 0) {
        s.position_push(chr_eff_offset, SEEK_SET);
        pvpp_chara_effect_read(&chr->chara_effect, s);
        chr->chara_effect_init = true;
        s.position_pop();
    }

    if (motion_offset > 0) {
        chr->motion.resize(motion_count);

        s.position_push(motion_offset, SEEK_SET);
        for (string_hash& i : chr->motion) {
            s.position_push(s.read_offset_x(), SEEK_SET);
            pvpp_motion_read(&i, s);
            s.position_pop();
        }
        s.position_pop();
    }

    if (auth_3d_offset > 0) {
        chr->auth_3d.resize(auth_3d_count);

        s.position_push(auth_3d_offset, SEEK_SET);
        for (string_hash& i : chr->auth_3d)
            pvpp_auth_3d_read(&i, s);
        s.position_pop();
    }

    if (item_offset > 0) {
        chr->item.resize(item_count);

        s.position_push(item_offset, SEEK_SET);
        for (pvpp_chara_item& i : chr->item)
            pvpp_chara_item_read(&i, s);
        s.position_pop();
    }

    if (glitter_offset > 0) {
        chr->glitter.resize(glitter_count);

        s.position_push(glitter_offset, SEEK_SET);
        for (pvpp_glitter& i : chr->glitter)
            pvpp_glitter_read(&i, s);
        s.position_pop();
    }
}

static void pvpp_chara_effect_read(pvpp_chara_effect* chr_eff, stream& s) {
    chr_eff->base_chara = (pvpp_chara_index)s.read_uint8_t();
    int8_t auth_3d_count = s.read_int8_t();
    chr_eff->chara_id = (pvpp_chara_id)s.read_uint8_t();
    int64_t auth_3d_offstet = s.read_offset_x();

    if (auth_3d_offstet > 0) {
        chr_eff->auth_3d.resize(auth_3d_count);

        s.position_push(auth_3d_offstet, SEEK_SET);
        for (pvpp_chara_effect_auth_3d& i : chr_eff->auth_3d)
            pvpp_chara_effect_auth_3d_read(&i, s);
        s.position_pop();
    }
}

static void pvpp_chara_effect_auth_3d_read(pvpp_chara_effect_auth_3d* chr_eff_auth_3d, stream& s) {
    chr_eff_auth_3d->u00 = s.read_uint8_t();
    chr_eff_auth_3d->u01 = s.read_uint8_t();
    chr_eff_auth_3d->u02 = s.read_uint8_t();
    chr_eff_auth_3d->u03 = s.read_uint8_t();
    chr_eff_auth_3d->u04 = s.read_uint8_t();
    chr_eff_auth_3d->u05 = s.read_uint8_t();
    chr_eff_auth_3d->u06 = s.read_uint8_t();
    chr_eff_auth_3d->u07 = s.read_uint8_t();
    int64_t auth_3d_offset = s.read_offset_x();
    int64_t source_auth_3d_offset = s.read_offset_x();

    if (auth_3d_offset > 0) {
        s.position_push(auth_3d_offset, SEEK_SET);
        pvpp_auth_3d_read(&chr_eff_auth_3d->auth_3d, s);
        s.position_pop();
    }

    chr_eff_auth_3d->has_object_set = false;
    if (source_auth_3d_offset > 0) {
        s.position_push(source_auth_3d_offset, SEEK_SET);
        pvpp_object_set_read(&chr_eff_auth_3d->object_set, s);
        chr_eff_auth_3d->has_object_set = true;
        s.position_pop();
    }
}

static void pvpp_chara_item_read(pvpp_chara_item* chr_itm, stream& s) {
    chr_itm->type = (pvpp_chara_item_type)s.read_int8_t();
    int64_t auth_3ds_offset = s.read_offset_x();
    int64_t node_offset = s.read_offset_x();
    chr_itm->u18.x = s.read_float_t_reverse_endianness();
    chr_itm->u18.y = s.read_float_t_reverse_endianness();
    chr_itm->u18.z = s.read_float_t_reverse_endianness();
    chr_itm->u18.w = s.read_float_t_reverse_endianness();

    if (auth_3ds_offset > 0) {
        s.position_push(auth_3ds_offset, SEEK_SET);
        pvpp_chara_item_auth_3d_read(&chr_itm->auth_3d, s);
        s.position_pop();
    }

    if (node_offset > 0)
        chr_itm->node = s.read_string_null_terminated_offset(node_offset);
}

static void pvpp_chara_item_auth_3d_read(string_hash* a3d, stream& s) {
    *a3d = s.read_string_null_terminated_offset(s.read_offset_x());
    s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_effect_read(pvpp_effect* eff, stream& s) {
    int8_t auth_3ds_count = s.read_int8_t();
    int8_t glitters_count = s.read_int8_t();
    eff->chara_id = (pvpp_chara_id)s.read_uint8_t();
    int64_t auth_3ds_offset = s.read_offset_x();
    int64_t glitters_offset = s.read_offset_x();

    if (auth_3ds_offset) {
        eff->auth_3d.resize(auth_3ds_count);

        s.position_push(auth_3ds_offset, SEEK_SET);
        for (string_hash& i : eff->auth_3d)
            pvpp_auth_3d_read(&i, s);
        s.position_pop();
    }

    if (glitters_offset) {
        eff->glitter.resize(glitters_count);

        s.position_push(glitters_offset, SEEK_SET);
        for (pvpp_glitter& i : eff->glitter)
            pvpp_glitter_read(&i, s);
        s.position_pop();
    }
}

static void pvpp_glitter_read(pvpp_glitter* glt, stream& s) {
    glt->name = s.read_string_null_terminated_offset(s.read_offset_x());
    glt->unk1 = s.read_string_null_terminated_offset(s.read_offset_x());
    glt->unk2 = s.read_uint8_t() ? true : false;
    s.align_read(0x08);
}

static void pvpp_motion_read(string_hash* mot, stream& s) {
    *mot = s.read_string_null_terminated_offset(s.read_offset_x());
    s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_object_set_read(string_hash* objset, stream& s) {
    *objset = s.read_string_null_terminated_offset(s.read_offset_x());
    s.read_uint32_t_reverse_endianness();
    s.align_read(0x08);
}

static void pvpp_read_inner(pvpp* pp, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('PVPP') || !st.data.data()) {
        pp->ready = false;
        pp->big_endian = false;
        return;
    }

    bool big_endian = st.header.attrib.get_big_endian();

    memory_stream s_pvpp;
    s_pvpp.open(st.data);
    s_pvpp.big_endian = big_endian;

    s_pvpp.read_uint32_t_reverse_endianness();
    int8_t chara_count = s_pvpp.read_int8_t();
    int64_t effect_offset = s_pvpp.read_offset_x();
    int64_t chara_offset = s_pvpp.read_offset_x();
    s_pvpp.read_offset_x();

    if (chara_offset) {
        pp->chara.resize(chara_count);

        s_pvpp.position_push(chara_offset, SEEK_SET);
        for (pvpp_chara& i : pp->chara)
            pvpp_chara_read(&i, s_pvpp);
        s_pvpp.position_pop();
    }

    if (effect_offset) {
        s_pvpp.position_push(effect_offset, SEEK_SET);
        s_pvpp.read_uint8_t();
        int8_t efffect_count = s_pvpp.read_int8_t();
        int64_t effect_offset = s_pvpp.read_offset_x();
        s_pvpp.position_pop();

        if (effect_offset) {
            pp->effect.resize(efffect_count);

            s_pvpp.position_push(effect_offset, SEEK_SET);
            for (pvpp_effect& i : pp->effect)
                pvpp_effect_read(&i, s_pvpp);
            s_pvpp.position_pop();
        }
    }

    pp->ready = true;
    pp->big_endian = true;
}
