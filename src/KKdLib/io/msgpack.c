/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.h"

static void io_msgpack_read_inner(stream* s, char* name, msgpack* msg);
static void io_msgpack_read_string(stream* s, char* name, msgpack* msg, size_t length);
static void io_msgpack_read_array(stream* s, char* name, msgpack* msg, size_t length);
static void io_msgpack_read_map(stream* s, char* name, msgpack* msg, size_t length);
static void io_msgpack_write_inner(stream* s, msgpack* msg);
static void io_msgpack_write_null(stream* s);
static void io_msgpack_write_bool(stream* s, bool val);
static void io_msgpack_write_int8_t(stream* s, int8_t val);
static void io_msgpack_write_uint8_t(stream* s, uint8_t val);
static void io_msgpack_write_int16_t(stream* s, int16_t val);
static void io_msgpack_write_uint16_t(stream* s, uint16_t val);
static void io_msgpack_write_int32_t(stream* s, int32_t val);
static void io_msgpack_write_uint32_t(stream* s, uint32_t val);
static void io_msgpack_write_int64_t(stream* s, int64_t val);
static void io_msgpack_write_uint64_t(stream* s, uint64_t val);
static void io_msgpack_write_float_t(stream* s, float_t val);
static void io_msgpack_write_double_t(stream* s, double_t val);
static void io_msgpack_write_string(stream* s, string* val);
static void io_msgpack_write_array(stream* s, size_t val);
static void io_msgpack_write_map(stream* s, size_t val);

void io_msgpack_read(stream* s, msgpack* msg) {
    io_msgpack_read_inner(s, 0, msg);
}

static void io_msgpack_read_inner(stream* s, char* name, msgpack* msg) {
    uint8_t type = io_read_uint8_t(s);

    msgpack_init_null(msg, name);
    switch (type >> 4) {
    case 0x0: case 0x1: case 0x2: case 0x3:
    case 0x4: case 0x5: case 0x6: case 0x7:
    case 0xE: case 0xF:
        msgpack_set_int8_t(msg, name, (int8_t)type);
        return;
    case 0x8:
        io_msgpack_read_map(s, name, msg, type & 0x0F);
        return;
    case 0x9:
        io_msgpack_read_array(s, name, msg, type & 0x0F);
        return;
    case 0xA: case 0xB:
        io_msgpack_read_string(s, name, msg, type & 0x1F);
        return;
    }

    switch (type) {
    case 0xC2:
        msgpack_set_bool(msg, name, false);
        break;
    case 0xC3:
        msgpack_set_bool(msg, name, true);
        break;
    case 0xC4:
        io_set_position(s, io_read_uint8_t(s), SEEK_CUR);
        break;
    case 0xC5:
        io_set_position(s, io_read_uint16_t_reverse_endianness(s, true), SEEK_CUR);
        break;
    case 0xC6:
        io_set_position(s, io_read_uint32_t_reverse_endianness(s, true), SEEK_CUR);
        break;
    case 0xC7:
        io_set_position(s, io_read_uint8_t(s) + 1ULL, SEEK_CUR);
        break;
    case 0xC8:
        io_set_position(s, io_read_uint16_t_reverse_endianness(s, true) + 1ULL, SEEK_CUR);
        break;
    case 0xC9:
        io_set_position(s, io_read_uint32_t_reverse_endianness(s, true) + 1ULL, SEEK_CUR);
        break;
    case 0xCA:
        msgpack_set_float_t(msg, name, io_read_float_t_reverse_endianness(s, true));
        break;
    case 0xCB:
        msgpack_set_double_t(msg, name, io_read_double_t_reverse_endianness(s, true));
        break;
    case 0xCC:
        msgpack_set_uint8_t(msg, name, io_read_uint8_t(s));
        break;
    case 0xCD:
        msgpack_set_uint16_t(msg, name, io_read_uint16_t_reverse_endianness(s, true));
        break;
    case 0xCE:
        msgpack_set_uint32_t(msg, name, io_read_uint32_t_reverse_endianness(s, true));
        break;
    case 0xCF:
        msgpack_set_uint64_t(msg, name, io_read_uint64_t_reverse_endianness(s, true));
        break;
    case 0xD0:
        msgpack_set_int8_t(msg, name, io_read_int8_t(s));
        break;
    case 0xD1:
        msgpack_set_int16_t(msg, name, io_read_int16_t_reverse_endianness(s, true));
        break;
    case 0xD2:
        msgpack_set_int32_t(msg, name, io_read_int32_t_reverse_endianness(s, true));
        break;
    case 0xD3:
        msgpack_set_int64_t(msg, name, io_read_int64_t_reverse_endianness(s, true));
        break;
    case 0xD4:
        io_set_position(s, 2, SEEK_CUR);
        break;
    case 0xD5:
        io_set_position(s, 3, SEEK_CUR);
        break;
    case 0xD6:
        io_set_position(s, 5, SEEK_CUR);
        break;
    case 0xD7:
        io_set_position(s, 9, SEEK_CUR);
        break;
    case 0xD8:
        io_set_position(s, 17, SEEK_CUR);
        break;
    case 0xD9:
        io_msgpack_read_string(s, name, msg, io_read_uint8_t(s));
        break;
    case 0xDA:
        io_msgpack_read_string(s, name, msg, io_read_uint16_t_reverse_endianness(s, true));
        break;
    case 0xDB:
        io_msgpack_read_string(s, name, msg, io_read_uint32_t_reverse_endianness(s, true));
        break;
    case 0xDC:
        io_msgpack_read_array(s, name, msg, io_read_uint16_t_reverse_endianness(s, true));
        break;
    case 0xDD:
        io_msgpack_read_array(s, name, msg, io_read_uint32_t_reverse_endianness(s, true));
        break;
    case 0xDE:
        io_msgpack_read_map(s, name, msg, io_read_uint16_t_reverse_endianness(s, true));
        break;
    case 0xDF:
        io_msgpack_read_map(s, name, msg, io_read_uint16_t_reverse_endianness(s, true));
        break;
    }
}

static void io_msgpack_read_string(stream* s, char* name, msgpack* msg, size_t length) {
    string str;
    io_read_string(s, &str, length);
    msgpack_set_string(msg, name, &str);
    string_free(&str);
}

static void io_msgpack_read_array(stream* s, char* name, msgpack* msg, size_t length) {
    msgpack m;
    msgpack_init_array(&m, 0, length);
    msgpack_array* ptr = MSGPACK_SELECT(msgpack_array, m);
    for (size_t i = 0; i < length; i++)
        io_msgpack_read_inner(s, 0, &ptr->begin[i]);
    msgpack_set_array(msg, name, ptr);
    msgpack_free(&m);
}

static void io_msgpack_read_map(stream* s, char* name, msgpack* msg, size_t length) {
    msgpack m;
    msgpack_init_map(&m, 0);
    msgpack_map* ptr = MSGPACK_SELECT(msgpack_map, m);
    *ptr = vector_empty(msgpack);
    vector_msgpack_reserve(ptr, length);
    for (size_t i = 0; i < length; i++) {
        msgpack t_m;
        msgpack_init_null(&t_m, 0);
        io_msgpack_read_inner(s, 0, &t_m);

        char* n = 0;
        if (t_m.type == MSGPACK_STRING)
            n = string_data(MSGPACK_SELECT(string, t_m));

        msgpack* m = vector_msgpack_reserve_back(ptr);
        io_msgpack_read_inner(s, n, m);
        msgpack_free(&t_m);
    }
    msgpack_set_map(msg, name, ptr);
    msgpack_free(&m);
}

void io_msgpack_write(stream* s, msgpack* msg) {
    if (!s->io.stream || !msg)
        return;

    io_msgpack_write_inner(s, msg);
}

static void io_msgpack_write_inner(stream* s, msgpack* msg) {
    if (msg->name.length)
        io_msgpack_write_string(s, &msg->name);

    switch (msg->type) {
    case MSGPACK_NULL:
        io_msgpack_write_null(s);
        break;
    case MSGPACK_bool:
        io_msgpack_write_bool(s, *MSGPACK_SELECT_PTR(bool, msg));
        break;
    case MSGPACK_INT8: {
        io_msgpack_write_int8_t(s, *MSGPACK_SELECT_PTR(int8_t, msg));
    } break;
    case MSGPACK_UINT8:
        io_msgpack_write_uint8_t(s, *MSGPACK_SELECT_PTR(uint8_t, msg));
        break;
    case MSGPACK_INT16:
        io_msgpack_write_int16_t(s, *MSGPACK_SELECT_PTR(int16_t, msg));
        break;
    case MSGPACK_UINT16:
        io_msgpack_write_uint16_t(s, *MSGPACK_SELECT_PTR(uint16_t, msg));
        break;
    case MSGPACK_INT32:
        io_msgpack_write_int32_t(s, *MSGPACK_SELECT_PTR(int32_t, msg));
        break;
    case MSGPACK_UINT32:
        io_msgpack_write_uint32_t(s, *MSGPACK_SELECT_PTR(uint32_t, msg));
        break;
    case MSGPACK_INT64:
        io_msgpack_write_int64_t(s, *MSGPACK_SELECT_PTR(int64_t, msg));
        break;
    case MSGPACK_UINT64:
        io_msgpack_write_uint64_t(s, *MSGPACK_SELECT_PTR(uint64_t, msg));
        break;
    case MSGPACK_FLOAT:
        io_msgpack_write_float_t(s, *MSGPACK_SELECT_PTR(float_t, msg));
        break;
    case MSGPACK_DOUBLE:
        io_msgpack_write_double_t(s, *MSGPACK_SELECT_PTR(double_t, msg));
        break;
    case MSGPACK_STRING:
        io_msgpack_write_string(s, MSGPACK_SELECT_PTR(string, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
        io_msgpack_write_array(s, vector_length(*ptr));
        for (msgpack* i = ptr->begin; i != ptr->end; i++)
            io_msgpack_write_inner(s, i);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = MSGPACK_SELECT_PTR(msgpack_map, msg);
        io_msgpack_write_map(s, vector_length(*ptr));
        for (msgpack* i = ptr->begin; i != ptr->end; i++)
            io_msgpack_write_inner(s, i);
    } break;
    }
}

static void io_msgpack_write_null(stream* s) {
    io_write_uint8_t(s, 0xC0);
}

static void io_msgpack_write_bool(stream* s, bool val) {
    io_write_uint8_t(s, val ? 0xC3 : 0xC2);
}

static void io_msgpack_write_int8_t(stream* s, int8_t val) {
    if (val < -0x20)
        io_write_uint8_t(s, 0xD0);
    io_write_int8_t(s, val);
}

static void io_msgpack_write_uint8_t(stream* s, uint8_t val) {
    if (val >= 0x80)
        io_write_uint8_t(s, 0xCC);
    io_write_uint8_t(s, val);
}

static void io_msgpack_write_int16_t(stream* s, int16_t val) {
    if ((int8_t)val == val)
        io_msgpack_write_int8_t(s, (int8_t)val);
    else if ((uint8_t)val == val)
        io_msgpack_write_uint8_t(s, (uint8_t)val);
    else {
        io_write_uint8_t(s, 0xD1);
        io_write_int16_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_uint16_t(stream* s, uint16_t val) {
    if ((uint8_t)val == val)
        io_msgpack_write_uint8_t(s, (uint8_t)val);
    else {
        io_write_uint8_t(s, 0xCD);
        io_write_uint16_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_int32_t(stream* s, int32_t val) {
    if ((int16_t)val == val)
        io_msgpack_write_int16_t(s, (int16_t)val);
    else if ((uint16_t)val == val)
        io_msgpack_write_uint16_t(s, (uint16_t)val);
    else {
        io_write_uint8_t(s, 0xD2);
        io_write_int32_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_uint32_t(stream* s, uint32_t val) {
    if ((uint16_t)val == val)
        io_msgpack_write_uint16_t(s, (uint16_t)val);
    else {
        io_write_uint8_t(s, 0xCE);
        io_write_uint32_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_int64_t(stream* s, int64_t val) {
    if ((int32_t)val == val)
        io_msgpack_write_int32_t(s, (int32_t)val);
    else if ((uint32_t)val == val)
        io_msgpack_write_uint32_t(s, (uint32_t)val);
    else {
        io_write_uint8_t(s, 0xD3);
        io_write_int64_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_uint64_t(stream* s, uint64_t val) {
    if ((uint32_t)val == val)
        io_msgpack_write_uint32_t(s, (uint32_t)val);
    else {
        io_write_uint8_t(s, 0xCF);
        io_write_uint64_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_float_t(stream* s, float_t val) {
    if (*(uint32_t*)&val == 0x80000000) {
        io_write_uint8_t(s, 0xCA);
        io_write_float_t_reverse_endianness(s, -0.0f, true);
    }
    else if ((int64_t)val == val)
        io_msgpack_write_int64_t(s, (int64_t)val);
    else {
        io_write_uint8_t(s, 0xCA);
        io_write_float_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_double_t(stream* s, double_t val) {
    if (*(uint64_t*)&val == 0x8000000000000000) {
        io_write_uint8_t(s, 0xCA);
        io_write_float_t_reverse_endianness(s, -0.0f, true);
    }
    else if ((int64_t)val == val)
        io_msgpack_write_int64_t(s, (int64_t)val);
    else if ((float_t)val == val)
        io_msgpack_write_float_t(s, (float_t)val);
    else {
        io_write_uint8_t(s, 0xCB);
        io_write_double_t_reverse_endianness(s, val, true);
    }
}

static void io_msgpack_write_string(stream* s, string* val) {
    if (!val) {
        io_msgpack_write_null(s);
        return;
    }

    size_t len = val->length;
    if (len < 0x20)
        io_write_uint8_t(s, 0xA0 | (len & 0x1F));
    else if (len < 0x100) {
        io_write_uint8_t(s, 0xD9);
        io_write_uint8_t(s, (uint8_t)len);
    }
    else if (len < 0x10000) {
        io_write_uint8_t(s, 0xDA);
        io_write_uint16_t_reverse_endianness(s, (uint16_t)len, true);
    }
    else {
        io_write_uint8_t(s, 0xDB);
        io_write_uint32_t_reverse_endianness(s, (uint32_t)len, true);
    }
    io_write(s, string_data(val), len);
}

static void io_msgpack_write_array(stream* s, size_t val) {
    size_t len = val;
    if (!len)
        io_msgpack_write_null(s);
    else if (len < 0x10)
        io_write_uint8_t(s, 0x90 | (len & 0x0F));
    else if (len < 0x10000) {
        io_write_uint8_t(s, 0xDC);
        io_write_uint16_t_reverse_endianness(s, (uint16_t)len, true);
    }
    else {
        io_write_uint8_t(s, 0xDD);
        io_write_uint32_t_reverse_endianness(s, (uint32_t)len, true);
    }
}

static void io_msgpack_write_map(stream* s, size_t val) {
    size_t len = val;
    if (!len)
        io_msgpack_write_null(s);
    else if (len < 0x10)
        io_write_uint8_t(s, 0x80 | (len & 0x0F));
    else if (len < 0x10000) {
        io_write_uint8_t(s, 0xDE);
        io_write_uint16_t_reverse_endianness(s, (uint16_t)len, true);
    }
    else {
        io_write_uint8_t(s, 0xDF);
        io_write_uint32_t_reverse_endianness(s, (uint32_t)len, true);
    }
}
