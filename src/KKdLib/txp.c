/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "txp.h"
#include "io_stream.h"

static void txp_dispose_sub(txp* t);

txp* txp_init() {
    txp* t = force_malloc(sizeof(txp));
    return t;
}

void txp_pack_file(txp* t, void** data, size_t* length, bool use_big_endian) {
    txp_data* t_data;
    txp_sub_data** t_sub_data;
    txp_sub_data* t_sub_sub_data;

    if (!t || !data || !length)
        return;

    *data = 0;
    *length = 0;

    if (!t->count || !t->data)
        return;

    size_t* txp4_offset = force_malloc_s(sizeof(size_t), t->count);
    size_t** txp2_offset = force_malloc_s(sizeof(size_t*), t->count);
    t_data = t->data;
    for (size_t i = 0; i < t->count; i++, t_data++)
        txp2_offset[i] = force_malloc_s(sizeof(size_t), (size_t)t_data->mipmaps_count * t_data->array_size);

    stream* s = io_open_memory(0, 0);
    s->is_big_endian = use_big_endian;
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    for (size_t i = 0; i < t->count; i++)
        io_write_uint32_t(s, 0);

    t_data = t->data;
    for (size_t i = 0; i < t->count; i++, t_data++) {
        txp4_offset[i] = io_get_position(s);

        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        for (size_t j = 0; j < t_data->array_size; j++)
            for (size_t k = 0; k < t_data->mipmaps_count; k++)
                io_write_uint32_t(s, 0);

        t_sub_data = t_data->data;
        for (size_t j = 0; j < t_data->array_size; j++, t_sub_data++) {
            t_sub_sub_data = *t_sub_data;
            for (size_t k = 0; k < t_data->mipmaps_count; k++, t_sub_sub_data++) {
                txp2_offset[i][j * t_data->mipmaps_count + k] = io_get_position(s);
                io_write_uint32_t(s, 0);
                io_write_uint32_t(s, 0);
                io_write_uint32_t(s, 0);
                io_write_uint32_t(s, 0);
                io_write_uint32_t(s, 0);
                io_write_uint32_t(s, 0);
                io_write(s, t_sub_sub_data->data, t_sub_sub_data->size);
            }
        }
    }

    io_set_position(s, 0, SEEK_SET);
    io_write_uint32_t_stream_reverse_endianess(s, 0x03505854);
    io_write_uint32_t_stream_reverse_endianess(s, t->count);
    io_write_uint32_t_stream_reverse_endianess(s, (uint8_t)t->count | 0x01010100);
    for (size_t i = 0; i < t->count; i++)
        io_write_uint32_t_stream_reverse_endianess(s, (uint32_t)txp4_offset[i]);

    t_data = t->data;
    for (size_t i = 0; i < t->count; i++, t_data++) {
        io_set_position(s, txp4_offset[i], SEEK_SET);
        io_write_uint32_t_stream_reverse_endianess(s, t_data->array_size > 1 ? 0x05505854 : 0x04505854);
        io_write_uint32_t_stream_reverse_endianess(s, t_data->mipmaps_count * t_data->array_size);
        io_write_uint32_t_stream_reverse_endianess(s, (uint8_t)t_data->mipmaps_count
            | ((uint8_t)t_data->array_size << 8) | 0x01010000);
        for (size_t j = 0; j < t_data->array_size; j++)
            for (size_t k = 0; k < t_data->mipmaps_count; k++)
                io_write_uint32_t_stream_reverse_endianess(s,
                    (uint32_t)(txp2_offset[i][j * t_data->mipmaps_count + k] - txp4_offset[i]));

        t_sub_data = t_data->data;
        for (size_t j = 0; j < t_data->array_size; j++, t_sub_data++) {
            t_sub_sub_data = *t_sub_data;
            for (size_t k = 0; k < t_data->mipmaps_count; k++, t_sub_sub_data++) {
                io_set_position(s, txp2_offset[i][j * t_data->mipmaps_count + k], SEEK_SET);
                io_write_uint32_t_stream_reverse_endianess(s, 0x02505854);
                io_write_uint32_t_stream_reverse_endianess(s, t_sub_sub_data->width);
                io_write_uint32_t_stream_reverse_endianess(s, t_sub_sub_data->height);
                io_write_uint32_t_stream_reverse_endianess(s, t_sub_sub_data->format);
                io_write_uint32_t_stream_reverse_endianess(s, (uint32_t)(j * t_data->mipmaps_count + k));
                io_write_uint32_t_stream_reverse_endianess(s, t_sub_sub_data->size);
            }
        }
    }
    io_set_position(s, 0, SEEK_END);
    io_align(s, 0x10);

    *length = s->io.data.vec.end - s->io.data.vec.begin;
    *data = force_malloc(*length);
    memcpy(*data, s->io.data.vec.begin, *length);
    io_dispose(s);

    for (size_t i = 0; i < t->count; i++)
        free(txp2_offset[i]);
    free(txp2_offset);
    free(txp4_offset);
}

bool txp_unpack_file(txp* t, void* data, bool use_big_endian) {
    uint32_t sign;
    uint32_t t_count;
    txp_data* t_data;
    txp_sub_data** t_sub_data;
    txp_sub_data* t_sub_sub_data;
    size_t d;
    size_t sub_d;
    size_t sub_sub_d;
    size_t sub_sub_tex_d;
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

    txp_dispose_sub(t);
    d = (size_t)data;
    t_count = *(uint32_t*)(d + 4);
    t->count = t_count;
    t_data = force_malloc_s(sizeof(txp_data), t_count);
    t->data = t_data;
    for (size_t i = 0; i < t_count; i++, t_data++) {
        sub_d = d + (size_t)((uint32_t*)(d + 12))[i];
        if (use_big_endian)
            sign = reverse_endianess_uint32_t(*(uint32_t*)sub_d);
        else
            sign = *(uint32_t*)sub_d;

        if (sign != 0x04505854 && sign != 0x05505854)
            continue;

        if (use_big_endian) {
            sub_tex_count = reverse_endianess_uint32_t(*(uint32_t*)(sub_d + 4));
            info = reverse_endianess_uint32_t(*(uint32_t*)(sub_d + 8));
        }
        else {
            sub_tex_count = *(uint32_t*)(sub_d + 4);
            info = *(uint32_t*)(sub_d + 8);
        }

        t_data->mipmaps_count = info & 0xFF;
        t_data->array_size = (info >> 8) & 0xFF;

        if (t_data->array_size == 1 && t_data->mipmaps_count != sub_tex_count)
            t_data->mipmaps_count = sub_tex_count & 0xFF;

        t_sub_data = force_malloc_s(sizeof(txp_sub_data*), t_data->array_size);
        t_data->data = t_sub_data;
        for (size_t j = 0; j < t_data->array_size; j++, t_sub_data++) {
            t_sub_sub_data = force_malloc_s(sizeof(txp_sub_data), t_data->mipmaps_count);
            *t_sub_data = t_sub_sub_data;
            for (size_t k = 0; k < t_data->mipmaps_count; k++, t_sub_sub_data++) {
                sub_sub_d = sub_d + (size_t)((uint32_t*)(sub_d + 12))[j * t_data->mipmaps_count + k];
                if (use_big_endian)
                    sign = reverse_endianess_uint32_t(*(uint32_t*)sub_sub_d);
                else
                    sign = *(uint32_t*)sub_sub_d;

                if (sign != 0x02505854)
                    continue;

                if (use_big_endian) {
                    t_sub_sub_data->width = reverse_endianess_uint32_t(*(uint32_t*)(sub_sub_d + 4));
                    t_sub_sub_data->height = reverse_endianess_uint32_t(*(uint32_t*)(sub_sub_d + 8));
                    t_sub_sub_data->format = reverse_endianess_uint32_t(*(uint32_t*)(sub_sub_d + 12));
                    t_sub_sub_data->size = reverse_endianess_uint32_t(*(uint32_t*)(sub_sub_d + 20));
                }
                else {
                    t_sub_sub_data->width = *(uint32_t*)(sub_sub_d + 4);
                    t_sub_sub_data->height = *(uint32_t*)(sub_sub_d + 8);
                    t_sub_sub_data->format = *(uint32_t*)(sub_sub_d + 12);
                    t_sub_sub_data->size = *(uint32_t*)(sub_sub_d + 20);
                }

                sub_sub_tex_d = sub_sub_d + 24;
                t_sub_sub_data->data = force_malloc(t_sub_sub_data->size);
                memcpy(t_sub_sub_data->data, (void*)sub_sub_tex_d, t_sub_sub_data->size);
            }
        }
    }
    return true;
}

static void txp_dispose_sub(txp* t) {
    size_t i;
    size_t j;
    size_t k;
    uint32_t t_count;
    txp_data* t_data;
    txp_sub_data** t_sub_data;
    txp_sub_data* t_sub_sub_data;
    if (!t)
        return;

    t_count = t->count;
    t_data = t->data;
    for (i = 0; i < t_count; i++, t_data++) {
        if (!t_data->data)
            continue;

        t_sub_data = t_data->data;
        for (j = 0; j < t_data->array_size; j++, t_sub_data++) {
            if (!*t_sub_data)
                continue;

            t_sub_sub_data = *t_sub_data;
            for (k = 0; k < t_data->mipmaps_count; k++, t_sub_sub_data++)
                free(t_sub_sub_data->data);
            free(*t_sub_data);
        }
        free(t_data->data);
    }
}

void txp_dispose(txp* t) {
    txp_dispose_sub(t);
    free(t);
}
