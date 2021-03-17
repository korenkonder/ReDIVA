/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "f2_pof.h"

static bool pof_write_packed_value(stream* s, size_t val);

FORCE_INLINE size_t pof_read_offsets_count(stream* s) {
    size_t pos = io_get_position(s);
    size_t i, j, l;
    pof_value val;

    l = io_read_uint32_t(s) - 4LL;
    i = 0;
    j = 0;
    while (i < l) {
        val = (pof_value)((io_read_uint8_t(s) >> 6) & 0x03);
        if (val == POF_VALUE_INT32) {
            io_read_uint8_t(s);
            io_read_uint8_t(s);
            io_read_uint8_t(s);
            i += 3;
        }
        else if (val == POF_VALUE_INT16) {
            io_read_uint8_t(s);
            i++;
        }
        else if (val != POF_VALUE_INT8)
            break;
        i++;
        j++;
    }
    io_set_position(s, pos, IO_SEEK_SET);
    return j;
}

void pof_read(stream* s, len_array_size_t* pof, bool shift_x) {
    memset(pof, 0, sizeof(len_array_size_t));

    len_array_size_t p;
    size_t i, j, l, length, offset, v;
    uint8_t bit_shift;
    pof_value value;

    length = pof_read_offsets_count(s);

    bit_shift = (uint8_t)(shift_x ? 3 : 2);
    l = io_read_uint32_t(s) - 4LL;

    p.data = force_malloc_s(sizeof(size_t), length);
    p.length = p.fulllength = length;

    memset(p.data, 0, sizeof(size_t) * length);

    i = 0;
    j = 0;
    offset = 0;
    while (i < l) {

        v = io_read_uint8_t(s);
        value = (pof_value)((v >> 6) & 0x3);
        v &= 0x3F;

        if (value == POF_VALUE_INT32) {
            v = (((((v << 8) | io_read_uint8_t(s)) << 8) | io_read_uint8_t(s)) << 8) | io_read_uint8_t(s);
            i += 3;
        }
        else if (value == POF_VALUE_INT16) {
            v = (v << 8) | io_read_uint8_t(s);
            i++;
        }
        else if (value == POF_VALUE_INVALID)
            break;

        offset += v;
        p.data[j] = offset << bit_shift;
        i++;
        j++;
    }
    *pof = p;
}

void pof_write(stream* s, len_array_size_t* pof, bool shift_x) {
    len_array_size_t p;
    size_t i, j, k, l, o, v;
    uint8_t bit_shift;

    p = *pof;

    j = 0;
    o = 0;
    bit_shift = (uint8_t)(shift_x ? 3 : 2);
    v = ((size_t)1 << bit_shift) - 1;
    l = pof_length(pof, shift_x);
    io_write_uint32_t(s, (uint32_t)l);

    for (i = 0; i < p.length; i++) {
        o = p.data[i];
        if (o & v) {
            pof_write_packed_value(s, 0x7FFFFFFF);
            break;
        }

        k = o - j;
        if ((i > 0) & !k)
            continue;
        j = o;
        o = k;

        pof_write_packed_value(s, o >> bit_shift);
    }

    size_t pos = io_get_position(s);
    for (size_t c = align_val(pos, 0x10) - pos; c > 0; c--)
        io_write_uint8_t(s, 0);
}

static bool pof_write_packed_value(stream* s, size_t val) {
    if (val < 0x40)
        io_write_uint8_t(s, (uint8_t)((POF_VALUE_INT8 << 6) | (val & 0x3F)));
    else if (val < 0x4000) {
        io_write_uint8_t(s, (uint8_t)((POF_VALUE_INT16 << 6) | ((val >> 8) & 0x3F)));
        io_write_uint8_t(s, (uint8_t)val);
    }
    else if (val < 0x40000000) {
        io_write_uint8_t(s, (uint8_t)((POF_VALUE_INT32 << 6) | ((val >> 24) & 0x3F)));
        io_write_uint8_t(s, (uint8_t)(val >> 16));
        io_write_uint8_t(s, (uint8_t)(val >> 8));
        io_write_uint8_t(s, (uint8_t)val);
    }
    else {
        io_write_uint8_t(s, POF_VALUE_INVALID << 6);
        return true;
    }
    return false;
}

FORCE_INLINE bool pof_length_get_size(uint32_t* length, size_t val) {
    *length += val < 0x40 ? 1 : val < 0x4000 ? 2 : val < 0x40000000 ? 4 : 1;
    return val >= 0x40000000;
}

uint32_t pof_length(len_array_size_t* pof, bool shift_x) {
    len_array_size_t p;
    size_t i, j, k, o, v;
    uint32_t l;
    uint8_t bit_shift;

    l = 4;
    o = 0;
    bit_shift = (uint8_t)(shift_x ? 3 : 2);
    v = ((size_t)1 << bit_shift) - 1;
    p = *pof;

    for (i = 0; i < p.length; i++) {
        o = p.data[i];
        if (o & v)
            break;
        else if (i > 0) {
            k = o - j;
            if (!k)
                continue;
            j = o;
            o = k;
        }
        else
            j = o;

        if (pof_length_get_size(&l, o >> bit_shift))
            break;
    }
    return l;
}
