/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "txp.h"
#include "io_stream.h"

vector_func(txp_mipmap)
vector_func(txp)

void txp_copy(txp* src, txp* dst) {
    memset(dst, 0, sizeof(txp));
    dst->has_cubemap = src->has_cubemap;
    dst->array_size = src->array_size;
    dst->mipmaps_count = src->mipmaps_count;
    vector_txp_mipmap_append(&dst->data, src->data.end - src->data.begin);
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
    vector_txp_mipmap_free(&t->data);
}

bool tex_set_pack_file(vector_txp* t, void** data, size_t* length, bool use_big_endian) {
    size_t l;
    txp* tex;
    txp_mipmap* tex_mipmap;

    if (!t || !data || !length)
        return false;

    *data = 0;
    *length = 0;

    size_t count = t->end - t->begin;
    if (count < 1)
        return false;

    size_t* txp4_offset = force_malloc_s(size_t, count);
    size_t** txp2_offset = force_malloc_s(size_t*, count);
    tex = t->begin;
    for (size_t i = 0; i < count; i++, tex++)
        txp2_offset[i] = force_malloc_s(size_t, (size_t)tex->mipmaps_count * tex->array_size);

    l = 12 + count * 4;

    tex = t->begin;
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

    stream* s = io_open_memory(0, l);
    s->is_big_endian = use_big_endian;
    io_write_uint32_t_stream_reverse_endianess(s, 0x03505854);
    io_write_uint32_t_stream_reverse_endianess(s, (uint32_t)count);
    io_write_uint32_t_stream_reverse_endianess(s, (uint8_t)count | 0x01010100);
    for (size_t i = 0; i < count; i++)
        io_write_uint32_t_stream_reverse_endianess(s, (uint32_t)txp4_offset[i]);

    tex = t->begin;
    for (size_t i = 0; i < count; i++, tex++) {
        io_set_position(s, txp4_offset[i], SEEK_SET);
        io_write_uint32_t_stream_reverse_endianess(s, tex->array_size > 1 ? 0x05505854 : 0x04505854);
        io_write_uint32_t_stream_reverse_endianess(s, tex->mipmaps_count * tex->array_size);
        io_write_uint32_t_stream_reverse_endianess(s, (uint8_t)tex->mipmaps_count
            | ((uint8_t)tex->array_size << 8) | 0x01010000);
        for (size_t j = 0; j < tex->array_size; j++)
            for (size_t k = 0; k < tex->mipmaps_count; k++)
                io_write_uint32_t_stream_reverse_endianess(s,
                    (uint32_t)(txp2_offset[i][j * tex->mipmaps_count + k] - txp4_offset[i]));

        tex_mipmap = tex->data.begin;
        for (size_t j = 0; j < tex->array_size; j++)
            for (size_t k = 0; k < tex->mipmaps_count; k++, tex_mipmap++) {
                io_set_position(s, txp2_offset[i][j * tex->mipmaps_count + k], SEEK_SET);
                io_write_uint32_t_stream_reverse_endianess(s, 0x02505854);
                io_write_uint32_t_stream_reverse_endianess(s, tex_mipmap->width);
                io_write_uint32_t_stream_reverse_endianess(s, tex_mipmap->height);
                io_write_uint32_t_stream_reverse_endianess(s, tex_mipmap->format);
                io_write_uint32_t_stream_reverse_endianess(s, (uint32_t)(j * tex->mipmaps_count + k));
                io_write_uint32_t_stream_reverse_endianess(s, tex_mipmap->size);
                io_write(s, tex_mipmap->data, tex_mipmap->size);
                io_align(s, 0x04);
            }
    }
    io_set_position(s, 0, SEEK_END);
    io_align(s, 0x10);

    *length = s->io.data.vec.end - s->io.data.vec.begin;
    *data = force_malloc(*length);
    memcpy(*data, s->io.data.vec.begin, *length);
    io_dispose(s);

    for (size_t i = 0; i < count; i++)
        free(txp2_offset[i]);
    free(txp2_offset);
    free(txp4_offset);
    return true;
}

bool tex_set_produce_enrs(vector_txp* t, vector_enrs_entry* enrs) {
    size_t l;
    txp* tex;
    txp_mipmap* tex_mipmap;

    if (!t || !enrs)
        return false;

    *enrs = (vector_enrs_entry){ 0, 0, 0 };
    l = 0;

    size_t count = t->end - t->begin;
    if (count < 1)
        return false;

    uint32_t o;
    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 1, 12, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 12;

    ee = (enrs_entry){ o, 1, (uint32_t)(count * 4), 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, (uint32_t)count, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += (size_t)(o = (uint32_t)(count * 4LL));

    tex = t->begin;
    for (size_t i = 0; i < count; i++, tex++) {
        ee = (enrs_entry){ o, 1, 12, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 12;

        ee = (enrs_entry){ o, 1, tex->array_size * 4, tex->mipmaps_count, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, tex->array_size, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += (size_t)(o = (uint32_t)((size_t)tex->array_size * tex->mipmaps_count * 4));

        tex_mipmap = tex->data.begin;
        for (size_t j = 0; j < tex->array_size; j++) {
            for (size_t k = 0; k < tex->mipmaps_count; k++, tex_mipmap++) {
                ee = (enrs_entry){ o, 1, 24, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_TYPE_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                l += (size_t)(o = (uint32_t)(24 + tex_mipmap->size));
            }
        }
    }

    *enrs = e;
    return true;
}

bool tex_set_unpack_file(vector_txp* t, void* data, bool use_big_endian) {
    uint32_t sign;
    uint32_t tex_count;
    txp tex;
    txp_mipmap tex_mipmap;
    size_t set_d;
    size_t d;
    size_t mipmap_d;
    uint32_t sub_tex_count;
    uint32_t info;

    if (!t || !data)
        return false;

    if (use_big_endian)
        sign = reverse_endianess_uint32_t(*(uint32_t*)data);
    else
        sign = *(uint32_t*)data;

    if (sign != 0x03505854)
        return false;

    tex_set_free(t);
    set_d = (size_t)data;
    if (use_big_endian)
        tex_count = reverse_endianess_uint32_t(*(uint32_t*)(set_d + 4));
    else
        tex_count = *(uint32_t*)(set_d + 4);

    *t = (vector_txp){ 0, 0, 0 };
    vector_txp_append(t, tex_count);
    for (size_t i = 0; i < tex_count; i++) {
        if (use_big_endian) {
            d = set_d + (size_t)reverse_endianess_uint32_t(((uint32_t*)(set_d + 12))[i]);
            sign = reverse_endianess_uint32_t(*(uint32_t*)d);
        }
        else {
            d = set_d + (size_t)((uint32_t*)(set_d + 12))[i];
            sign = *(uint32_t*)d;
        }

        if (sign != 0x04505854 && sign != 0x05505854)
            continue;

        if (use_big_endian) {
            sub_tex_count = reverse_endianess_uint32_t(*(uint32_t*)(d + 4));
            info = reverse_endianess_uint32_t(*(uint32_t*)(d + 8));
        }
        else {
            sub_tex_count = *(uint32_t*)(d + 4);
            info = *(uint32_t*)(d + 8);
        }

        memset(&tex, 0, sizeof(txp));
        tex.has_cubemap = sign == 0x05505854;
        tex.mipmaps_count = info & 0xFF;
        tex.array_size = (info >> 8) & 0xFF;

        if (tex.array_size == 1 && tex.mipmaps_count != sub_tex_count)
            tex.mipmaps_count = sub_tex_count & 0xFF;

        uint32_t mipmaps_count = tex.mipmaps_count;
        tex.data = (vector_txp_mipmap){ 0, 0, 0 };
        vector_txp_mipmap_append(&tex.data, (size_t)tex.array_size * tex.mipmaps_count);
        for (size_t j = 0; j < tex.array_size; j++)
            for (size_t k = 0; k < tex.mipmaps_count; k++) {
                if (use_big_endian) {
                    mipmap_d = d + (size_t)reverse_endianess_uint32_t(((uint32_t*)(d + 12))[j * mipmaps_count + k]);
                    sign = reverse_endianess_uint32_t(*(uint32_t*)mipmap_d);
                }
                else {
                    mipmap_d = d + (size_t)((uint32_t*)(d + 12))[j * mipmaps_count + k];
                    sign = *(uint32_t*)mipmap_d;
                }

                memset(&tex_mipmap, 0, sizeof(txp_mipmap));

                if (use_big_endian) {
                    tex_mipmap.width = reverse_endianess_uint32_t(*(uint32_t*)(mipmap_d + 4));
                    tex_mipmap.height = reverse_endianess_uint32_t(*(uint32_t*)(mipmap_d + 8));
                    tex_mipmap.format = reverse_endianess_uint32_t(*(uint32_t*)(mipmap_d + 12));
                    tex_mipmap.size = reverse_endianess_uint32_t(*(uint32_t*)(mipmap_d + 20));
                }
                else {
                    tex_mipmap.width = *(uint32_t*)(mipmap_d + 4);
                    tex_mipmap.height = *(uint32_t*)(mipmap_d + 8);
                    tex_mipmap.format = *(uint32_t*)(mipmap_d + 12);
                    tex_mipmap.size = *(uint32_t*)(mipmap_d + 20);
                }

                ssize_t size = txp_get_size(tex_mipmap.format, tex_mipmap.width, tex_mipmap.height);
                tex_mipmap.data = force_malloc(max(size, tex_mipmap.size));
                memcpy(tex_mipmap.data, (void*)(mipmap_d + 24), tex_mipmap.size);
                size -= tex_mipmap.size;
                if (size > 0)
                    memset((void*)((size_t)tex_mipmap.data + tex_mipmap.size), 0, size);
                vector_txp_mipmap_push_back(&tex.data, &tex_mipmap);
            }
        vector_txp_push_back(t, &tex);
    }
    return true;
}

void tex_set_free(vector_txp* t) {
    if (!t)
        return;

    for (txp* i = t->begin; i != t->end; i++) {
        for (txp_mipmap* j = i->data.begin; j != i->data.end; j++)
            free(j->data);
        vector_txp_mipmap_free(&i->data);
    }
    vector_txp_free(t);
}
