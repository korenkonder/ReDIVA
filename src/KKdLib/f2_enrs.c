/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "f2_enrs.h"

typedef enum enrs_value {
    ENRS_VALUE_INT8    = 0x0,
    ENRS_VALUE_INT16   = 0x1,
    ENRS_VALUE_INT32   = 0x2,
    ENRS_VALUE_INVALID = 0x3,
} enrs_value;

vector_func(enrs_sub_entry)
vector_func(enrs_entry)

static inline bool enrs_length_get_size_type(uint32_t* length, size_t val);
static inline bool enrs_length_get_size(uint32_t* length, size_t val);
static bool enrs_read_packed_value_type(stream* s, uint32_t* val, enrs_type* type);
static bool enrs_read_packed_value(stream* s, uint32_t* val);
static bool enrs_write_packed_value_type(stream* s, uint32_t val, enrs_type type);
static bool enrs_write_packed_value(stream* s, uint32_t val);

void enrs_apply(vector_enrs_entry* enrs, void* data) {
    if (!enrs || !data)
        return;

    uint8_t* d = data;
    uint8_t* temp;
    for (enrs_entry* i = enrs->begin; i != enrs->end; i++) {
        d += i->offset;
        for (size_t j = 0; j < i->repeat_count; j++) {
            temp = d + i->size * j;
            for (enrs_sub_entry* k = i->sub.begin; k != i->sub.end; k++) {
                temp += k->skip_bytes;
                switch (k->type) {
                case ENRS_TYPE_WORD:
                    for (size_t l = 0; l < k->repeat_count; l++) {
                        *(uint16_t*)temp = reverse_endianness_uint16_t(*(uint16_t*)temp);
                        temp += 2;
                    }
                    break;
                case ENRS_TYPE_DWORD:
                    for (size_t l = 0; l < k->repeat_count; l++) {
                        *(uint32_t*)temp = reverse_endianness_uint32_t(*(uint32_t*)temp);
                        temp += 4;
                    }
                    break;
                case ENRS_TYPE_QWORD:
                    for (size_t l = 0; l < k->repeat_count; l++) {
                        *(uint64_t*)temp = reverse_endianness_uint64_t(*(uint64_t*)temp);
                        temp += 8;
                    }
                    break;
                }
            }
        }
    }
}


uint32_t enrs_length(vector_enrs_entry* enrs) {
    enrs_entry* i;
    enrs_sub_entry* j;
    uint32_t l;

    l = 0x10;
    for (i = enrs->begin; i != enrs->end; i++) {
        i->count = (uint32_t)(i->sub.end - i->sub.begin);
        enrs_entry entry = *i;
        if (enrs_length_get_size(&l, entry.offset)
            || enrs_length_get_size(&l, entry.count)
            || enrs_length_get_size(&l, entry.size)
            || enrs_length_get_size(&l, entry.repeat_count))
            goto End;

        if (entry.repeat_count < 1 || entry.count > 0x40000000)
            continue;

        for (j = entry.sub.begin; j != entry.sub.end; j++) {
            enrs_sub_entry sub = *j;
            if (enrs_length_get_size_type(&l, sub.skip_bytes)
                || enrs_length_get_size(&l, sub.repeat_count))
                goto End;
        }
    }
End:
    l = align_val(l, 0x10);
    return l;
}

void enrs_read(stream* s, vector_enrs_entry* enrs) {
    enrs_entry entry;
    enrs_sub_entry sub_entry;
    vector_enrs_entry e;
    vector_enrs_sub_entry sub;
    size_t i, j, l;

    vector_enrs_entry_free(enrs);

    io_read_uint32_t(s);
    l = io_read_uint32_t(s);
    io_read_uint32_t(s);
    io_read_uint32_t(s);

    e = (vector_enrs_entry){ 0, 0, 0 };
    vector_enrs_entry_append(&e, l);
    for (i = 0; i < l; i++) {
        memset(&entry, 0, sizeof(enrs_entry));
        if (enrs_read_packed_value(s, &entry.offset)
            || enrs_read_packed_value(s, &entry.count)
            || enrs_read_packed_value(s, &entry.size)
            || enrs_read_packed_value(s, &entry.repeat_count))
            goto End;

        if (!entry.count || !entry.repeat_count) {
            vector_enrs_entry_push_back(&e, &entry);
            continue;
        }

        sub = (vector_enrs_sub_entry){ 0, 0, 0 };
        vector_enrs_sub_entry_append(&sub, entry.count);
        for (j = 0; j < entry.count; j++) {
            memset(&sub_entry, 0, sizeof(sub_entry));
            if (enrs_read_packed_value_type(s, &sub_entry.skip_bytes, &sub_entry.type)
                || enrs_read_packed_value(s, &sub_entry.repeat_count))
                goto End;
            vector_enrs_sub_entry_push_back(&sub, &sub_entry);
        }
        entry.sub = sub;
        vector_enrs_entry_push_back(&e, &entry);
    }
End:
    *enrs = e;
}

void enrs_write(stream* s, vector_enrs_entry* enrs) {
    enrs_entry entry;
    enrs_sub_entry sub_entry;
    enrs_entry* i;
    enrs_sub_entry* j;

    size_t length = enrs_length(enrs);
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, (uint32_t)(enrs->end - enrs->begin));
    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    for (i = enrs->begin; i != enrs->end; i++) {
        i->count = (uint32_t)(i->sub.end - i->sub.begin);
        entry = *i;
        if (enrs_write_packed_value(s, entry.offset)
            || enrs_write_packed_value(s, entry.count)
            || enrs_write_packed_value(s, entry.size)
            || enrs_write_packed_value(s, entry.repeat_count))
            goto End;

        if (entry.repeat_count < 1)
            continue;

        for (j = entry.sub.begin; j != entry.sub.end; j++) {
            sub_entry = *j;
            if (enrs_write_packed_value_type(s, sub_entry.skip_bytes, sub_entry.type)
                || enrs_write_packed_value(s, sub_entry.repeat_count))
                goto End;
        }
    }

End:
    {}
    size_t pos = io_get_position(s);
    for (size_t c = align_val(pos, 0x10) - pos; c > 0; c--)
        io_write_uint8_t(s, 0);
}

void enrs_dispose(vector_enrs_entry* e) {
    if (!e)
        return;

    for (enrs_entry* i = e->begin; i != e->end; i++)
        vector_enrs_sub_entry_free(&i->sub);
    vector_enrs_entry_free(e);
}

static inline bool enrs_length_get_size_type(uint32_t* length, size_t val) {
    *length += val < 0x10 ? 1 : val < 0x1000 ? 2 : val < 0x10000000 ? 4 : 1;
    return val >= 0x10000000;
}

static inline bool enrs_length_get_size(uint32_t* length, size_t val) {
    if (!length)
        return true;

    *length += val < 0x40 ? 1 : val < 0x4000 ? 2 : val < 0x40000000 ? 4 : 1;
    return val >= 0x40000000;
}

static bool enrs_read_packed_value_type(stream* s, uint32_t* val, enrs_type* type) {
    *val = io_read_char(s);
    enrs_value value = (enrs_value)((*val >> 6) & 0x3);
    *type = (enrs_type)((*val >> 4) & 0x3);
    *val &= 0xF;

    if (value == ENRS_VALUE_INT32)
        *val = (((((*val << 8) | io_read_char(s)) << 8) | io_read_char(s)) << 8) | io_read_char(s);
    else if (value == ENRS_VALUE_INT16)
        *val = (*val << 8) | io_read_char(s);
    else if (value == ENRS_VALUE_INVALID) {
        *val = 0;
        return true;
    }
    return false;
}

static bool enrs_read_packed_value(stream* s, uint32_t* val) {
    *val = io_read_uint8_t(s);
    enrs_value value = (enrs_value)((*val >> 6) & 0x3);
    *val &= 0x3F;

    if (value == ENRS_VALUE_INT32)
        *val = (((((*val << 8) | io_read_uint8_t(s)) << 8) | io_read_uint8_t(s)) << 8) | io_read_uint8_t(s);
    else if (value == ENRS_VALUE_INT16)
        *val = (*val << 8) | io_read_uint8_t(s);
    else if (value == ENRS_VALUE_INVALID) {
        *val = 0;
        return true;
    }
    return false;
}

static bool enrs_write_packed_value_type(stream* s, uint32_t val, enrs_type type) {
    uint8_t t = ((uint8_t)type & 0x3) << 4;
    if (val < 0x10)
        io_write_uint8_t(s, (uint8_t)((ENRS_VALUE_INT8 << 6) | t | (val & 0xF)));
    else if (val < 0x1000) {
        io_write_uint8_t(s, (uint8_t)((ENRS_VALUE_INT16 << 6) | t | ((val >> 8) & 0xF)));
        io_write_uint8_t(s, (uint8_t)val);
    }
    else if (val < 0x10000000) {
        io_write_uint8_t(s, (uint8_t)((ENRS_VALUE_INT32 << 6) | t | ((val >> 24) & 0xF)));
        io_write_uint8_t(s, (uint8_t)(val >> 16));
        io_write_uint8_t(s, (uint8_t)(val >> 8));
        io_write_uint8_t(s, (uint8_t)val);
    }
    else {
        io_write_uint8_t(s, ENRS_VALUE_INVALID << 6);
        return true;
    }
    return false;
}

static bool enrs_write_packed_value(stream* s, uint32_t val) {
    if (val < 0x40)
        io_write_uint8_t(s, (uint8_t)((ENRS_VALUE_INT8 << 6) | (val & 0x3F)));
    else if (val < 0x4000) {
        io_write_uint8_t(s, (uint8_t)((ENRS_VALUE_INT16 << 6) | ((val >> 8) & 0x3F)));
        io_write_uint8_t(s, (uint8_t)val);
    }
    else if (val < 0x40000000) {
        io_write_uint8_t(s, (uint8_t)((ENRS_VALUE_INT32 << 6) | ((val >> 24) & 0x3F)));
        io_write_uint8_t(s, (uint8_t)(val >> 16));
        io_write_uint8_t(s, (uint8_t)(val >> 8));
        io_write_uint8_t(s, (uint8_t)val);
    }
    else {
        io_write_uint8_t(s, ENRS_VALUE_INVALID << 6);
        return true;
    }
    return false;
}
