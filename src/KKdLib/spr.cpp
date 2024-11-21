/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <map>
#include "spr.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/memory_stream.hpp"

struct spr_layer_header {
    int64_t markers_offset;
    int64_t video_offset;
    int64_t audio_offset;
};

struct spr_scene_header {
    int64_t camera_offset;
    int64_t comp_offset;
    int64_t video_offset;
    int64_t audio_offset;
};

static void spr_set_classic_read_inner(spr_set* ss,
    prj::shared_ptr<prj::stack_allocator>& alloc, const void* data, size_t size);
static void spr_set_classic_write_inner(spr_set* ss, void** data, size_t* size);
static void spr_set_modern_read_inner(spr_set* ss,
    prj::shared_ptr<prj::stack_allocator>& alloc, const void* data, size_t size);
static void spr_set_modern_write_inner(spr_set* ss, void** data, size_t* size);

static const char* spr_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc);

static const char* spr_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset);

spr_set::spr_set() : ready(), modern(), big_endian(), is_x(), flag(),
num_of_texture(), num_of_sprite(), sprinfo(), texname(), sprname(), sprdata(), txp() {

}

void spr_set::move_data(spr_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!set_src->ready) {
        ready = false;
        modern = false;
        big_endian = false;
        is_x = false;
        return;
    }

    ready = true;
    modern = set_src->modern;
    big_endian = set_src->big_endian;
    is_x = set_src->is_x;

    flag = set_src->flag;

    num_of_texture = set_src->num_of_texture;
    num_of_sprite = set_src->num_of_sprite;

    sprinfo = alloc->allocate<spr::SprInfo>(set_src->sprinfo, set_src->num_of_sprite);

    const char** texname_src = set_src->texname;
    const char** texname_dst = alloc->allocate<const char*>(set_src->num_of_texture);
    texname = texname_dst;

    for (uint32_t i = set_src->num_of_texture; i; i--)
        *texname_dst++ = spr_move_data_string(*texname_src++, alloc);

    const char** sprname_src = set_src->sprname;
    const char** sprname_dst = alloc->allocate<const char*>(set_src->num_of_sprite);
    sprname = sprname_dst;

    for (uint32_t i = set_src->num_of_sprite; i; i--)
        *sprname_dst++ = spr_move_data_string(*sprname_src++, alloc);

    sprdata = alloc->allocate<SpriteData>(set_src->sprdata, set_src->num_of_sprite);

    txp = new txp_set;
    *txp = *set_src->txp;
}

void spr_set::pack_file(void** data, size_t* size) {
    if (!data || !ready)
        return;

    if (!modern)
        spr_set_classic_write_inner(this, data, size);
    else
        spr_set_modern_write_inner(this, data, size);
}

void spr_set::unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern)
        spr_set_classic_read_inner(this, alloc, data, size);
    else
        spr_set_modern_read_inner(this, alloc, data, size);
}

static void spr_set_classic_read_inner(spr_set* ss,
    prj::shared_ptr<prj::stack_allocator>& alloc, const void* data, size_t size) {
    memory_stream s;
    s.open(data, size);

    ss->flag = s.read_uint32_t();
    uint32_t texofs = s.read_uint32_t();
    ss->num_of_texture = s.read_uint32_t();
    ss->num_of_sprite = s.read_uint32_t();
    uint32_t sprinfo_offset = s.read_uint32_t();
    uint32_t texname_offset = s.read_uint32_t();
    uint32_t sprname_offset = s.read_uint32_t();
    uint32_t sprdata_offset = s.read_uint32_t();

    spr::SprInfo* sprinfo = alloc->allocate<spr::SprInfo>(ss->num_of_sprite);
    ss->sprinfo = sprinfo;

    s.set_position(sprinfo_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_sprite; i; i--, sprinfo++) {
        sprinfo->texid = s.read_uint32_t();
        sprinfo->rotate = s.read_int32_t();
        sprinfo->su = s.read_float_t();
        sprinfo->sv = s.read_float_t();
        sprinfo->eu = s.read_float_t();
        sprinfo->ev = s.read_float_t();
        sprinfo->px = s.read_float_t();
        sprinfo->py = s.read_float_t();
        sprinfo->width = s.read_float_t();
        sprinfo->height = s.read_float_t();
    }

    const char** texname = alloc->allocate<const char*>(ss->num_of_texture);
    ss->texname = texname;

    s.set_position(texname_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_texture; i; i--, texname++)
        *texname = spr_read_utf8_string_null_terminated_offset(alloc, s, s.read_uint32_t());

    const char** sprname = alloc->allocate<const char*>(ss->num_of_sprite);
    ss->sprname = sprname;

    s.set_position(sprname_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_sprite; i; i--, sprname++)
        *sprname = spr_read_utf8_string_null_terminated_offset(alloc, s, s.read_uint32_t());

    SpriteData* sprdata = alloc->allocate<SpriteData>(ss->num_of_sprite);
    ss->sprdata = sprdata;

    s.set_position(sprdata_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_sprite; i; i--, sprdata++) {
        sprdata->attr = s.read_uint32_t();
        sprdata->resolution_mode = (resolution_mode)s.read_uint32_t();
    }
    s.close();

    ss->txp = new txp_set;
    ss->txp->unpack_file((const void*)((size_t)data + texofs), false);

    ss->ready = true;
    ss->modern = false;
    ss->big_endian = false;
    ss->is_x = false;
}

static void spr_set_classic_write_inner(spr_set* ss, void** data, size_t* size) {
    memory_stream s;
    s.open();

    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.write_int32_t(0);
    s.align_write(0x10);

    uint32_t sprname_offset = (uint32_t)s.get_position();
    s.write(sizeof(uint32_t) * ss->num_of_sprite);
    s.align_write(0x20);

    uint32_t sprinfo_offset = (uint32_t)s.get_position();
    spr::SprInfo* sprinfo = ss->sprinfo;
    for (uint32_t i = ss->num_of_sprite; i; i--, sprinfo++) {
        s.write_uint32_t(sprinfo->texid);
        s.write_int32_t(sprinfo->rotate);
        s.write_float_t(sprinfo->su);
        s.write_float_t(sprinfo->sv);
        s.write_float_t(sprinfo->eu);
        s.write_float_t(sprinfo->ev);
        s.write_float_t(sprinfo->px);
        s.write_float_t(sprinfo->py);
        s.write_float_t(sprinfo->width);
        s.write_float_t(sprinfo->height);
    }
    s.align_write(0x20);

    uint32_t texname_offset = (uint32_t)s.get_position();
    s.write(sizeof(uint32_t) * ss->num_of_texture);
    s.align_write(0x20);

    uint32_t sprdata_offset = (uint32_t)s.get_position();
    SpriteData* sprdata = ss->sprdata;
    for (uint32_t i = ss->num_of_sprite; i; i--, sprdata++) {
        s.write_uint32_t(sprdata->attr);
        s.write_uint32_t(sprdata->resolution_mode);
    }
    s.align_write(0x20);

    int64_t* string_offsets = force_malloc<int64_t>(max_def(ss->num_of_texture, ss->num_of_sprite));

    const char** sprname = ss->sprname;
    for (uint32_t i = ss->num_of_sprite; i; i--) {
        *string_offsets++ = s.get_position();
        s.write_string_null_terminated(*sprname++);
    }
    s.align_write(0x04);
    string_offsets -= ss->num_of_sprite;

    s.position_push(sprname_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_sprite; i; i--)
        s.write_uint32_t((uint32_t)*string_offsets++);
    s.position_pop();
    string_offsets -= ss->num_of_sprite;

    const char** texname = ss->texname;
    for (uint32_t i = ss->num_of_texture; i; i--) {
        *string_offsets++ = s.get_position();
        s.write_string_null_terminated(*texname++);
    }
    s.align_write(0x04);
    string_offsets -= ss->num_of_texture;

    s.position_push(texname_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_texture; i; i--)
        s.write_uint32_t((uint32_t)*string_offsets++);
    s.position_pop();
    string_offsets -= ss->num_of_texture;

    free_def(string_offsets);

    uint32_t texofs = (uint32_t)s.get_position();

    void* txp_data = 0;
    size_t txp_size = 0;
    ss->txp->pack_file(&txp_data, &txp_size, false);
    s.write(txp_data, txp_size);
    free_def(txp_data);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(ss->flag);
    s.write_uint32_t(texofs);
    s.write_uint32_t(ss->num_of_texture);
    s.write_uint32_t(ss->num_of_sprite);
    s.write_uint32_t(sprinfo_offset);
    s.write_uint32_t(texname_offset);
    s.write_uint32_t(sprname_offset);
    s.write_uint32_t(sprdata_offset);
    s.position_pop();

    s.copy(data, size);
}

static void spr_set_modern_read_inner(spr_set* ss,
    prj::shared_ptr<prj::stack_allocator>& alloc, const void* data, size_t size) {
    f2_struct st;
    st.read(data, size);
    if (st.header.signature != reverse_endianness_uint32_t('SPRC') || !st.data.size())
        return;

    uint32_t header_length = st.header.get_length();
    bool big_endian = st.header.attrib.get_big_endian();
    bool is_x = st.pof.shift_x;

    memory_stream s_sprc;
    s_sprc.open(st.data);
    s_sprc.big_endian = big_endian;

    ss->flag = s_sprc.read_uint32_t_reverse_endianness();
    uint32_t texofs = s_sprc.read_uint32_t_reverse_endianness();
    ss->num_of_texture = s_sprc.read_uint32_t_reverse_endianness();
    ss->num_of_sprite = s_sprc.read_uint32_t_reverse_endianness();
    int64_t sprinfo_offset = s_sprc.read_offset(header_length, is_x);
    int64_t texname_offset = s_sprc.read_offset(header_length, is_x);
    int64_t sprname_offset = s_sprc.read_offset(header_length, is_x);
    int64_t sprdata_offset = s_sprc.read_offset(header_length, is_x);

    spr::SprInfo* sprinfo = alloc->allocate<spr::SprInfo>(ss->num_of_sprite);
    ss->sprinfo = sprinfo;

    s_sprc.set_position(sprinfo_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_sprite; i; i--, sprinfo++) {
        sprinfo->texid = s_sprc.read_uint32_t_reverse_endianness();
        sprinfo->rotate = s_sprc.read_int32_t_reverse_endianness();
        sprinfo->su = s_sprc.read_float_t_reverse_endianness();
        sprinfo->sv = s_sprc.read_float_t_reverse_endianness();
        sprinfo->eu = s_sprc.read_float_t_reverse_endianness();
        sprinfo->ev = s_sprc.read_float_t_reverse_endianness();
        sprinfo->px = s_sprc.read_float_t_reverse_endianness();
        sprinfo->py = s_sprc.read_float_t_reverse_endianness();
        sprinfo->width = s_sprc.read_float_t_reverse_endianness();
        sprinfo->height = s_sprc.read_float_t_reverse_endianness();
    }

    const char** texname = alloc->allocate<const char*>(ss->num_of_texture);
    ss->texname = texname;

    s_sprc.set_position(texname_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = ss->num_of_texture; i; i--, texname++)
            *texname = spr_read_utf8_string_null_terminated_offset(
                alloc, s_sprc, s_sprc.read_offset_f2(header_length));
    else
        for (uint32_t i = ss->num_of_texture; i; i--, texname++)
            *texname = spr_read_utf8_string_null_terminated_offset(
                alloc, s_sprc, s_sprc.read_offset_x());

    const char** sprname = alloc->allocate<const char*>(ss->num_of_sprite);
    ss->sprname = sprname;

    s_sprc.set_position(sprname_offset, SEEK_SET);
    if (!is_x)
        for (uint32_t i = ss->num_of_sprite; i; i--, sprname++)
            *sprname = spr_read_utf8_string_null_terminated_offset(
                alloc, s_sprc, s_sprc.read_offset_f2(header_length));
    else
        for (uint32_t i = ss->num_of_sprite; i; i--, sprname++)
            *sprname = spr_read_utf8_string_null_terminated_offset(
                alloc, s_sprc, s_sprc.read_offset_x());

    SpriteData* sprdata = alloc->allocate<SpriteData>(ss->num_of_sprite);
    ss->sprdata = sprdata;

    s_sprc.set_position(sprdata_offset, SEEK_SET);
    for (uint32_t i = ss->num_of_sprite; i; i--, sprdata++) {
        sprdata->attr = s_sprc.read_uint32_t_reverse_endianness();
        sprdata->resolution_mode = (resolution_mode)s_sprc.read_uint32_t_reverse_endianness();
    }
    s_sprc.close();

    texofs = st.header.get_data_size() + F2_HEADER_DEFAULT_LENGTH;

    ss->txp = new txp_set;
    ss->txp->unpack_file_modern((const void*)((size_t)data + texofs), size - texofs, 'TXPC');

    ss->ready = true;
    ss->modern = true;
    ss->big_endian = big_endian;
    ss->is_x = is_x;
}

static void spr_set_modern_write_inner(spr_set* ss, void** data, size_t* size) {
    bool big_endian = ss->big_endian;
    bool is_x = ss->is_x;

    memory_stream s_sprc;
    s_sprc.open();
    s_sprc.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    off = align_val(0, 0x10);

    f2_struct st;
    s_sprc.align_write(0x10);
    s_sprc.copy(st.data);
    s_sprc.close();

    st.enrs = e;
    st.pof = pof;

    new (&st.header) f2_header('SPRC');
    st.header.attrib.set_big_endian(big_endian);

    st.write(data, size, true, is_x);
}

inline static const char* spr_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc) {
    if (str)
        return alloc->allocate<char>(str, utf8_length(str) + 1);
    return 0;
}

inline static const char* spr_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset) {
    size_t len = s.read_utf8_string_null_terminated_offset_length(offset);
    char* str = alloc->allocate<char>(len + 1);
    s.position_push(offset, SEEK_SET);
    s.read(str, len);
    s.position_pop();
    str[len] = 0;
    return str;
}
