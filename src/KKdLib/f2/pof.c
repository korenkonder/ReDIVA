/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pof.h"

enum pof_value_type {
    POF_VALUE_INVALID = 0x0,
    POF_VALUE_INT8    = 0x1,
    POF_VALUE_INT16   = 0x2,
    POF_VALUE_INT32   = 0x3,
};

inline static bool pof_length_get_size(uint32_t* length, size_t val);
static size_t pof_read_offsets_count(stream* s);
inline static bool pof_write_packed_value(stream* s, size_t val);

pof::pof() {

}

pof::~pof() {

}

void pof::add(stream* s,size_t offset) {
    if (!s)
        return;

    vec.push_back(io_get_position(s) + offset);
}

void pof::read(stream* s, bool shift_x) {
    vec.clear();

    size_t length = pof_read_offsets_count(s);

    uint8_t bit_shift = (uint8_t)(shift_x ? 3 : 2);
    size_t l = io_read_uint32_t(s) - 4ULL;

    vec.reserve(length);

    size_t i = 0;
    size_t j = 0;
    size_t offset = 0;
    while (i < l) {
        size_t v = io_read_uint8_t(s);
        pof_value_type value = (pof_value_type)((v >> 6) & 0x03);
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
        vec.push_back(offset << bit_shift);
        i++;
        j++;
    }
}

void pof::write(stream* s, bool shift_x) {
    size_t j = 0;
    size_t o = 0;
    uint8_t bit_shift = (uint8_t)(shift_x ? 3 : 2);
    size_t v = ((size_t)1 << bit_shift) - 1;
    size_t l = length(shift_x);
    if (shift_x)
        io_write_uint32_t(s, (uint32_t)l);
    else
        io_write_uint32_t(s, (uint32_t)align_val(l, 4));

    for (size_t& i : vec) {
        o = i;
        if (o & v) {
            pof_write_packed_value(s, 0x7FFFFFFF);
            break;
        }

        size_t k = o - j;
        if (&i != vec.data() && !k)
            continue;
        j = o;
        o = k;

        pof_write_packed_value(s, o >> bit_shift);
    }

    size_t pos = io_get_position(s);
    for (size_t c = align_val(pos, 0x10) - pos; c > 0; c--)
        io_write_uint8_t(s, 0);
}

uint32_t pof::length(bool shift_x) {
    uint32_t l = 4;
    size_t j = 0;
    uint8_t bit_shift = (uint8_t)(shift_x ? 3 : 2);
    size_t v = ((size_t)1 << bit_shift) - 1;

    for (size_t& i : vec) {
        size_t o = i;
        if (o & v)
            break;
        else if (&i != vec.data()) {
            size_t k = o - j;
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

inline void io_write_offset_pof_add(stream* s, ssize_t val,
    int32_t offset, bool is_x, pof* pof) {
    if (!is_x) {
        if (val)
            val += offset;
        pof->add(s, offset);
        io_write_int32_t_stream_reverse_endianness(s, (int32_t)val);
    }
    else {
        io_align_write(s, 0x08);
        pof->add(s, 0);
        io_write_int64_t_stream_reverse_endianness(s, val);
    }
}

inline void io_write_offset_f2_pof_add(stream* s, ssize_t val,
    int32_t offset, pof* pof) {
    if (val)
        val += offset;
    pof->add(s, offset);
    io_write_int32_t_stream_reverse_endianness(s, (int32_t)val);
}

inline void io_write_offset_x_pof_add(stream* s, ssize_t val, pof* pof) {
    io_align_write(s, 0x08);
    pof->add(s, 0);
    io_write_int64_t_stream_reverse_endianness(s, val);
}

inline static bool pof_length_get_size(uint32_t* length, size_t val) {
    *length += val < 0x40 ? 1 : val < 0x4000 ? 2 : val < 0x40000000 ? 4 : 1;
    return val >= 0x40000000;
}

static size_t pof_read_offsets_count(stream* s) {
    size_t pos = io_get_position(s);
    size_t i, j, l;
    pof_value_type val;

    l = io_read_uint32_t(s) - 4ULL;
    i = 0;
    j = 0;
    while (i < l) {
        val = (pof_value_type)((io_read_uint8_t(s) >> 6) & 0x03);
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
    io_set_position(s, pos, SEEK_SET);
    return j;
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
