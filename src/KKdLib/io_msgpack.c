/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "io_msgpack.h"
#include "utf8.h"

static void io_msgpack_read_inner(stream* s, wchar_t* name, msgpack* msg);
static void io_msgpack_read_string(stream* s, wchar_t* name, msgpack* msg, size_t length);
static void io_msgpack_read_array(stream* s, wchar_t* name, msgpack* msg, size_t length);
static void io_msgpack_read_map(stream* s, wchar_t* name, msgpack* msg, size_t length);
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
static void io_msgpack_write_string(stream* s, wchar_t_buffer* val);
static void io_msgpack_write_array(stream* s, size_t val);
static void io_msgpack_write_map(stream* s, size_t val);

msgpack* io_msgpack_read(stream* s) {
    msgpack* msg = force_malloc(sizeof(msgpack));
    io_msgpack_read_inner(s, 0, msg);
    return msg;
}

static void io_msgpack_read_inner(stream* s, wchar_t* name, msgpack* msg) {
    uint8_t type = io_read_uint8_t(s);

    msgpack_set_null(msg, name);
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
        io_set_position(s, io_read_uint8_t(s), IO_SEEK_CUR);
        break;
    case 0xC5:
        io_set_position(s, io_read_uint16_t_reverse_endianess(s, true), IO_SEEK_CUR);
        break;
    case 0xC6:
        io_set_position(s, io_read_uint32_t_reverse_endianess(s, true), IO_SEEK_CUR);
        break;
    case 0xC7:
        io_set_position(s, io_read_uint8_t(s) + 1LL, IO_SEEK_CUR);
        break;
    case 0xC8:
        io_set_position(s, io_read_uint16_t_reverse_endianess(s, true) + 1LL, IO_SEEK_CUR);
        break;
    case 0xC9:
        io_set_position(s, io_read_uint32_t_reverse_endianess(s, true) + 1LL, IO_SEEK_CUR);
        break;
    case 0xCA:
        msgpack_set_float_t(msg, name, io_read_float_t_reverse_endianess(s, true));
        break;
    case 0xCB:
        msgpack_set_double_t(msg, name, io_read_double_t_reverse_endianess(s, true));
        break;
    case 0xCC:
        msgpack_set_uint8_t(msg, name, io_read_uint8_t(s));
        break;
    case 0xCD:
        msgpack_set_uint16_t(msg, name, io_read_uint16_t_reverse_endianess(s, true));
        break;
    case 0xCE:
        msgpack_set_uint32_t(msg, name, io_read_uint32_t_reverse_endianess(s, true));
        break;
    case 0xCF:
        msgpack_set_uint64_t(msg, name, io_read_uint64_t_reverse_endianess(s, true));
        break;
    case 0xD0:
        msgpack_set_int8_t(msg, name, io_read_int8_t(s));
        break;
    case 0xD1:
        msgpack_set_int16_t(msg, name, io_read_int16_t_reverse_endianess(s, true));
        break;
    case 0xD2:
        msgpack_set_int32_t(msg, name, io_read_int32_t_reverse_endianess(s, true));
        break;
    case 0xD3:
        msgpack_set_int64_t(msg, name, io_read_int64_t_reverse_endianess(s, true));
        break;
    case 0xD4:
        io_set_position(s, 2, IO_SEEK_CUR);
        break;
    case 0xD5:
        io_set_position(s, 3, IO_SEEK_CUR);
        break;
    case 0xD6:
        io_set_position(s, 5, IO_SEEK_CUR);
        break;
    case 0xD7:
        io_set_position(s, 9, IO_SEEK_CUR);
        break;
    case 0xD8:
        io_set_position(s, 17, IO_SEEK_CUR);
        break;
    case 0xD9:
        io_msgpack_read_string(s, name, msg, io_read_uint8_t(s));
        break;
    case 0xDA:
        io_msgpack_read_string(s, name, msg, io_read_uint16_t_reverse_endianess(s, true));
        break;
    case 0xDB:
        io_msgpack_read_string(s, name, msg, io_read_uint32_t_reverse_endianess(s, true));
        break;
    case 0xDC:
        io_msgpack_read_array(s, name, msg, io_read_uint16_t_reverse_endianess(s, true));
        break;
    case 0xDD:
        io_msgpack_read_array(s, name, msg, io_read_uint32_t_reverse_endianess(s, true));
        break;
    case 0xDE:
        io_msgpack_read_map(s, name, msg, io_read_uint16_t_reverse_endianess(s, true));
        break;
    case 0xDF:
        io_msgpack_read_map(s, name, msg, io_read_uint16_t_reverse_endianess(s, true));
        break;
    }
}

static void io_msgpack_read_string(stream* s, wchar_t* name, msgpack* msg, size_t length) {
    char* temp_utf8 = force_malloc(length + 1);
    io_read(s, temp_utf8, length);
    temp_utf8[length] = 0;

    wchar_t* temp = utf8_decode(temp_utf8);
    msgpack_set_string(msg, name, temp);
    free(temp);
    free(temp_utf8);
}

static void io_msgpack_read_array(stream* s, wchar_t* name, msgpack* msg, size_t length) {
    msgpack* m = msgpack_init_array(0, length);
    msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, m);
    for (size_t i = 0; i < length; i++)
        io_msgpack_read_inner(s, 0, &ptr->data[i]);
    msgpack_set_array(msg, name, ptr);
    memset(m, 0, sizeof(msgpack));
    msgpack_dispose(m);
}

static void io_msgpack_read_map(stream* s, wchar_t* name, msgpack* msg, size_t length) {
    msgpack* m = msgpack_init_map(0);
    msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, m);
    ptr->length = length;
    for (size_t i = 0; i < length; i++) {
        msgpack* t_m = msgpack_init_null(0);
        io_msgpack_read_inner(s, 0, t_m);

        wchar_t* n = 0;
        if (t_m->type == MSGPACK_STRING)
            n = wchar_t_buffer_select(SELECT_MSGPACK(wchar_t_buffer, t_m));
        io_msgpack_read_inner(s, n, &ptr->data[i]);
        msgpack_dispose(t_m);
    }
    msgpack_set_map(msg, name, ptr);
    memset(m, 0, sizeof(msgpack));
    msgpack_dispose(m);
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
    case MSGPACK_BOOL:
        io_msgpack_write_bool(s, *SELECT_MSGPACK(bool, msg));
        break;
    case MSGPACK_INT8: {
        io_msgpack_write_int8_t(s, *SELECT_MSGPACK(int8_t, msg));
    } break;
    case MSGPACK_UINT8:
        io_msgpack_write_uint8_t(s, *SELECT_MSGPACK(uint8_t, msg));
        break;
    case MSGPACK_INT16:
        io_msgpack_write_int16_t(s, *SELECT_MSGPACK(int16_t, msg));
        break;
    case MSGPACK_UINT16:
        io_msgpack_write_uint16_t(s, *SELECT_MSGPACK(uint16_t, msg));
        break;
    case MSGPACK_INT32:
        io_msgpack_write_int32_t(s, *SELECT_MSGPACK(int32_t, msg));
        break;
    case MSGPACK_UINT32:
        io_msgpack_write_uint32_t(s, *SELECT_MSGPACK(uint32_t, msg));
        break;
    case MSGPACK_INT64:
        io_msgpack_write_int64_t(s, *SELECT_MSGPACK(int64_t, msg));
        break;
    case MSGPACK_UINT64:
        io_msgpack_write_uint64_t(s, *SELECT_MSGPACK(uint64_t, msg));
        break;
    case MSGPACK_FLOAT:
        io_msgpack_write_float_t(s, *SELECT_MSGPACK(float_t, msg));
        break;
    case MSGPACK_DOUBLE:
        io_msgpack_write_double_t(s, *SELECT_MSGPACK(double_t, msg));
        break;
    case MSGPACK_STRING:
        io_msgpack_write_string(s, SELECT_MSGPACK(wchar_t_buffer, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
        io_msgpack_write_array(s, ptr->length);
        for (size_t i = 0; i < ptr->length; i++)
            io_msgpack_write_inner(s, &ptr->data[i]);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
        io_msgpack_write_map(s, ptr->length);
        for (size_t i = 0; i < ptr->length; i++)
            io_msgpack_write_inner(s, &ptr->data[i]);
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
        io_write_int16_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_uint16_t(stream* s, uint16_t val) {
    if ((uint8_t)val == val)
        io_msgpack_write_uint8_t(s, (uint8_t)val);
    else {
        io_write_uint8_t(s, 0xCD);
        io_write_uint16_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_int32_t(stream* s, int32_t val) {
    if ((int16_t)val == val)
        io_msgpack_write_int16_t(s, (int16_t)val);
    else if ((uint16_t)val == val)
        io_msgpack_write_uint16_t(s, (uint16_t)val);
    else {
        io_write_uint8_t(s, 0xD2);
        io_write_int32_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_uint32_t(stream* s, uint32_t val) {
    if ((uint16_t)val == val)
        io_msgpack_write_uint16_t(s, (uint16_t)val);
    else {
        io_write_uint8_t(s, 0xCE);
        io_write_uint32_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_int64_t(stream* s, int64_t val) {
    if ((int32_t)val == val)
        io_msgpack_write_int32_t(s, (int32_t)val);
    else if ((uint32_t)val == val)
        io_msgpack_write_uint32_t(s, (uint32_t)val);
    else {
        io_write_uint8_t(s, 0xD3);
        io_write_int64_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_uint64_t(stream* s, uint64_t val) {
    if ((uint32_t)val == val)
        io_msgpack_write_uint32_t(s, (uint32_t)val);
    else {
        io_write_uint8_t(s, 0xCF);
        io_write_uint64_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_float_t(stream* s, float_t val) {
    if ((int64_t)val == val)
        io_msgpack_write_int64_t(s, (int64_t)val);
    else {
        io_write_uint8_t(s, 0xCA);
        io_write_float_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_double_t(stream* s, double_t val) {
    if ((int64_t)val == val)
        io_msgpack_write_int64_t(s, (int64_t)val);
    else if ((float_t)val == val)
        io_msgpack_write_float_t(s, (float_t)val);
    else {
        io_write_uint8_t(s, 0xCB);
        io_write_double_t_reverse_endianess(s, val, true);
    }
}

static void io_msgpack_write_string(stream* s, wchar_t_buffer* val) {

    char* temp = utf8_encode(wchar_t_buffer_select(val));
    if (!temp) {
        io_msgpack_write_null(s);
        return;
    }

    size_t len = strlen(temp);
    if (len < 0x20)
        io_write_uint8_t(s, 0xA0 | (len & 0x1F));
    else if (len < 0x100) {
        io_write_uint8_t(s, 0xD9);
        io_write_uint8_t(s, (uint8_t)len);
    }
    else if (len < 0x10000) {
        io_write_uint8_t(s, 0xDA);
        io_write_uint16_t_reverse_endianess(s, (uint16_t)len, true);
    }
    else {
        io_write_uint8_t(s, 0xDB);
        io_write_uint32_t_reverse_endianess(s, (uint32_t)len, true);
    }
    io_write(s, temp, len);
    free(temp);
}

static void io_msgpack_write_array(stream* s, size_t val) {
    size_t len = val;
    if (!len)
        io_msgpack_write_null(s);
    else if (len < 0x10)
        io_write_uint8_t(s, 0x90 | (len & 0x0F));
    else if (len < 0x10000) {
        io_write_uint8_t(s, 0xDC);
        io_write_uint16_t_reverse_endianess(s, (uint16_t)len, true);
    }
    else {
        io_write_uint8_t(s, 0xDD);
        io_write_uint32_t_reverse_endianess(s, (uint32_t)len, true);
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
        io_write_uint16_t_reverse_endianess(s, (uint16_t)len, true);
    }
    else {
        io_write_uint8_t(s, 0xDF);
        io_write_uint32_t_reverse_endianess(s, (uint32_t)len, true);
    }
}
