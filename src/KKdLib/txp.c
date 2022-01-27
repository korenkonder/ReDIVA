/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "txp.h"
#include "f2/struct.h"
#include "io/stream.h"

vector_func(txp_mipmap)
vector_func(txp)

void txp_init(txp* t) {
    memset(t, 0, sizeof(txp));
}

void txp_copy(txp* src, txp* dst) {
    memset(dst, 0, sizeof(txp));
    dst->has_cube_map = src->has_cube_map;
    dst->array_size = src->array_size;
    dst->mipmaps_count = src->mipmaps_count;
    dst->data = vector_empty(txp_mipmap);
    vector_txp_mipmap_reserve(&dst->data, vector_length(src->data));
    txp_mipmap* tex_mipmap = src->data.begin;
    for (size_t j = 0; j < src->array_size; j++) {
        for (size_t k = 0; k < src->mipmaps_count; k++, tex_mipmap++) {
            txp_mipmap t_mip;
            t_mip.width = tex_mipmap->width;
            t_mip.height = tex_mipmap->height;
            t_mip.format = tex_mipmap->format;
            t_mip.size = tex_mipmap->size;

            ssize_t size = txp_get_size(t_mip.format, t_mip.width, t_mip.height);
            t_mip.data = force_malloc(max(size, t_mip.size));
            memcpy(t_mip.data, tex_mipmap->data, t_mip.size);
            size -= t_mip.size;
            if (size > 0)
                memset((void*)((size_t)t_mip.data + t_mip.size), 0, size);
        }
    }
}

uint32_t txp_get_size(txp_format format, uint32_t width, uint32_t height) {
    uint32_t size = width * height;
    switch (format) {
    case TXP_A8:
        return size;
    case TXP_RGB8:
        return size * 3;
    case TXP_RGBA8:
        return size * 4;
    case TXP_RGB5:
        return size * 2;
    case TXP_RGB5A1:
        return size * 2;
    case TXP_RGBA4:
        return size * 2;
    case TXP_L8:
        return size;
    case TXP_L8A8:
        return size * 2;
    case TXP_DXT1:
    case TXP_DXT1a:
    case TXP_DXT3:
    case TXP_DXT5:
    case TXP_ATI1:
    case TXP_ATI2:
        width = align_val(width, 4);
        height = align_val(height, 4);
        size = width * height;
        switch (format) {
        case TXP_DXT1:
            return size / 2;
        case TXP_DXT1a:
            return size / 2;
        case TXP_DXT3:
            return size;
        case TXP_DXT5:
            return size;
        case TXP_ATI1:
            return size / 2;
        case TXP_ATI2:
            return size;
        }
        break;
    }
    return 0;
}

void txp_free(txp* t) {
    if (!t)
        return;

    for (txp_mipmap* i = t->data.begin; i != t->data.end; i++)
        free(i->data);
    vector_txp_mipmap_free(&t->data, 0);
}

void txp_set_init(txp_set* ts) {
    *ts = vector_empty(txp);
}

bool txp_set_pack_file(txp_set* ts, void** data, size_t* length, bool big_endian) {
    size_t l;
    txp* tex;
    txp_mipmap* tex_mipmap;

    if (!ts || !data)
        return false;

    *data = 0;
    *length = 0;

    size_t count = vector_length(*ts);
    if (count < 1)
        return false;

    size_t* txp4_offset = force_malloc_s(size_t, count);
    size_t** txp2_offset = force_malloc_s(size_t*, count);
    tex = ts->begin;
    for (size_t i = 0; i < count; i++, tex++)
        txp2_offset[i] = force_malloc_s(size_t, (size_t)tex->mipmaps_count * tex->array_size);

    l = 12 + count * 4;

    tex = ts->begin;
    for (size_t i = 0; i < count; i++, tex++) {
        txp4_offset[i] = l;
        l += 12 + (size_t)tex->array_size * tex->mipmaps_count * 4;

        tex_mipmap = tex->data.begin;
        for (size_t j = 0; j < tex->array_size; j++) {
            for (size_t k = 0; k < tex->mipmaps_count; k++, tex_mipmap++) {
                txp2_offset[i][j * tex->mipmaps_count + k] = l;
                l += 24;
                l += tex_mipmap->size;
            }
        }
    }

    stream s;
    io_mopen(&s, 0, l);
    s.is_big_endian = big_endian;
    io_write_uint32_t_stream_reverse_endianness(&s, 0x03505854);
    io_write_uint32_t_stream_reverse_endianness(&s, (uint32_t)count);
    io_write_uint32_t_stream_reverse_endianness(&s, (uint8_t)count | 0x01010100);
    for (size_t i = 0; i < count; i++)
        io_write_uint32_t_stream_reverse_endianness(&s, (uint32_t)txp4_offset[i]);

    tex = ts->begin;
    for (size_t i = 0; i < count; i++, tex++) {
        io_set_position(&s, txp4_offset[i], SEEK_SET);
        io_write_uint32_t_stream_reverse_endianness(&s, tex->array_size > 1 ? 0x05505854 : 0x04505854);
        io_write_uint32_t_stream_reverse_endianness(&s, tex->mipmaps_count * tex->array_size);
        io_write_uint32_t_stream_reverse_endianness(&s, (uint8_t)tex->mipmaps_count
            | ((uint8_t)tex->array_size << 8) | 0x01010000);
        for (size_t j = 0; j < tex->array_size; j++)
            for (size_t k = 0; k < tex->mipmaps_count; k++)
                io_write_uint32_t_stream_reverse_endianness(&s,
                    (uint32_t)(txp2_offset[i][j * tex->mipmaps_count + k] - txp4_offset[i]));

        tex_mipmap = tex->data.begin;
        for (size_t j = 0; j < tex->array_size; j++)
            for (size_t k = 0; k < tex->mipmaps_count; k++, tex_mipmap++) {
                io_set_position(&s, txp2_offset[i][j * tex->mipmaps_count + k], SEEK_SET);
                io_write_uint32_t_stream_reverse_endianness(&s, 0x02505854);
                io_write_uint32_t_stream_reverse_endianness(&s, tex_mipmap->width);
                io_write_uint32_t_stream_reverse_endianness(&s, tex_mipmap->height);
                io_write_uint32_t_stream_reverse_endianness(&s, tex_mipmap->format);
                io_write_uint32_t_stream_reverse_endianness(&s, (uint32_t)(j * tex->mipmaps_count + k));
                io_write_uint32_t_stream_reverse_endianness(&s, tex_mipmap->size);
                io_write(&s, tex_mipmap->data, tex_mipmap->size);
                io_align_write(&s, 0x04);
            }
    }
    io_set_position(&s, 0, SEEK_END);

    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);

    for (size_t i = 0; i < count; i++)
        free(txp2_offset[i]);
    free(txp2_offset);
    free(txp4_offset);
    return true;
}

bool txp_set_pack_file_modern(txp_set* ts, void** data, size_t* length, bool big_endian) {
    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));

    if (!txp_set_pack_file(ts, &st.data, &st.length, big_endian)) {
        *data = 0;
        *length = 0;
        return false;
    }

    txp_set_produce_enrs(ts, &st.enrs);

    st.header.signature = reverse_endianness_uint32_t('MTXD');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;
    f2_struct_mwrite(&st, data, length, true, false);
    f2_struct_free(&st);
    return true;
}

bool txp_set_produce_enrs(txp_set* ts, vector_enrs_entry* enrs) {
    size_t l;
    txp* tex;
    txp_mipmap* tex_mipmap;

    if (!ts || !enrs)
        return false;

    *enrs = vector_empty(enrs_entry);
    l = 0;

    size_t count = vector_length(*ts);
    if (count < 1)
        return false;

    uint32_t o;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;

    ee = (enrs_entry){ 0, 1, 12, 1, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 12;

    ee = (enrs_entry){ o, 1, (uint32_t)(count * 4), 1, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, (uint32_t)count, ENRS_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += (size_t)(o = (uint32_t)(count * 4ULL));

    tex = ts->begin;
    for (size_t i = 0; i < count; i++, tex++) {
        ee = (enrs_entry){ o, 1, 12, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 12;

        ee = (enrs_entry){ o, 1, tex->array_size * 4, tex->mipmaps_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, tex->array_size, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += (size_t)(o = (uint32_t)((size_t)tex->array_size * tex->mipmaps_count * 4));

        tex_mipmap = tex->data.begin;
        for (size_t j = 0; j < tex->array_size; j++) {
            for (size_t k = 0; k < tex->mipmaps_count; k++, tex_mipmap++) {
                ee = (enrs_entry){ o, 1, 24, 1, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                l += (size_t)(o = (uint32_t)(24 + tex_mipmap->size));
            }
        }
    }

    *enrs = e;
    return true;
}

bool txp_set_unpack_file(txp_set* ts, void* data, bool big_endian) {
    uint32_t sign;
    uint32_t tex_count;
    txp* tex;
    txp_mipmap* tex_mipmap;
    size_t set_d;
    size_t d;
    size_t mipmap_d;
    uint32_t sub_tex_count;
    uint32_t info;

    if (!ts || !data)
        return false;

    if (big_endian)
        sign = load_reverse_endianness_uint32_t((void*)data);
    else
        sign = *(uint32_t*)data;

    if (sign != 0x03505854)
        return false;

    set_d = (size_t)data;
    if (big_endian)
        tex_count = load_reverse_endianness_uint32_t((void*)(set_d + 4));
    else
        tex_count = *(uint32_t*)(set_d + 4);

    *ts = vector_empty(txp);
    vector_txp_reserve(ts, tex_count);
    for (size_t i = 0; i < tex_count; i++) {
        if (big_endian) {
            d = set_d + (size_t)load_reverse_endianness_uint32_t((uint32_t*)(set_d + 12) + i);
            sign = load_reverse_endianness_uint32_t((void*)d);
        }
        else {
            d = set_d + (size_t)((uint32_t*)(set_d + 12))[i];
            sign = *(uint32_t*)d;
        }

        if (sign != 0x04505854 && sign != 0x05505854)
            continue;

        if (big_endian) {
            sub_tex_count = load_reverse_endianness_uint32_t((void*)(d + 4));
            info = load_reverse_endianness_uint32_t((void*)(d + 8));
        }
        else {
            sub_tex_count = *(uint32_t*)(d + 4);
            info = *(uint32_t*)(d + 8);
        }

        tex = vector_txp_reserve_back(ts);
        tex->has_cube_map = sign == 0x05505854;
        tex->mipmaps_count = info & 0xFF;
        tex->array_size = (info >> 8) & 0xFF;

        if (tex->array_size == 1 && tex->mipmaps_count != sub_tex_count)
            tex->mipmaps_count = sub_tex_count & 0xFF;

        uint32_t mipmaps_count = tex->mipmaps_count;
        tex->data = vector_empty(txp_mipmap);
        vector_txp_mipmap_reserve(&tex->data, (size_t)tex->array_size * tex->mipmaps_count);
        for (size_t j = 0; j < tex->array_size; j++)
            for (size_t k = 0; k < tex->mipmaps_count; k++) {
                if (big_endian) {
                    mipmap_d = d + (size_t)load_reverse_endianness_uint32_t((uint32_t*)(d + 12) + j * mipmaps_count + k);
                    sign = load_reverse_endianness_uint32_t((void*)mipmap_d);
                }
                else {
                    mipmap_d = d + (size_t)((uint32_t*)(d + 12))[j * mipmaps_count + k];
                    sign = *(uint32_t*)mipmap_d;
                }

                tex_mipmap = vector_txp_mipmap_reserve_back(&tex->data);

                if (big_endian) {
                    tex_mipmap->width = load_reverse_endianness_uint32_t((void*)(mipmap_d + 4));
                    tex_mipmap->height = load_reverse_endianness_uint32_t((void*)(mipmap_d + 8));
                    tex_mipmap->format = load_reverse_endianness_uint32_t((void*)(mipmap_d + 12));
                    tex_mipmap->size = load_reverse_endianness_uint32_t((void*)(mipmap_d + 20));
                }
                else {
                    tex_mipmap->width = *(uint32_t*)(mipmap_d + 4);
                    tex_mipmap->height = *(uint32_t*)(mipmap_d + 8);
                    tex_mipmap->format = *(uint32_t*)(mipmap_d + 12);
                    tex_mipmap->size = *(uint32_t*)(mipmap_d + 20);
                }

                ssize_t size = txp_get_size(tex_mipmap->format, tex_mipmap->width, tex_mipmap->height);
                tex_mipmap->data = force_malloc(max(size, tex_mipmap->size));
                memcpy(tex_mipmap->data, (void*)(mipmap_d + 24), tex_mipmap->size);
                size -= tex_mipmap->size;
                if (size > 0)
                    memset((void*)((size_t)tex_mipmap->data + tex_mipmap->size), 0, size);
            }
    }
    return true;
}

bool txp_set_unpack_file_modern(txp_set* ts, void* data, size_t length) {
    bool ret = false;
    f2_struct st;
    f2_struct_mread(&st, data, length);
    if (st.header.signature == reverse_endianness_uint32_t('MTXD'))
        ret = txp_set_unpack_file(ts, st.data, st.header.use_big_endian);
    f2_struct_free(&st);
    return ret;
}

void txp_set_free(txp_set* t) {
    if (!t)
        return;

    for (txp* i = t->begin; i != t->end; i++) {
        for (txp_mipmap* j = i->data.begin; j != i->data.end; j++)
            free(j->data);
        vector_txp_mipmap_free(&i->data, 0);
    }
    vector_txp_free(t, 0);
}
