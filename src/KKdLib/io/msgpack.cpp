/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.hpp"

static void io_msgpack_read_inner(stream& s, msgpack* msg);
static void io_msgpack_read_string(stream& s, msgpack* msg, size_t length);
static void io_msgpack_read_array(stream& s, msgpack* msg, size_t length);
static void io_msgpack_read_map(stream& s, msgpack* msg, size_t length);
static void io_msgpack_write_inner(stream& s, msgpack* msg);
static void io_msgpack_write_null(stream& s);
static void io_msgpack_write_bool(stream& s, bool val);
static void io_msgpack_write_int8_t(stream& s, int8_t val);
static void io_msgpack_write_uint8_t(stream& s, uint8_t val);
static void io_msgpack_write_int16_t(stream& s, int16_t val);
static void io_msgpack_write_uint16_t(stream& s, uint16_t val);
static void io_msgpack_write_int32_t(stream& s, int32_t val);
static void io_msgpack_write_uint32_t(stream& s, uint32_t val);
static void io_msgpack_write_int64_t(stream& s, int64_t val);
static void io_msgpack_write_uint64_t(stream& s, uint64_t val);
static void io_msgpack_write_float_t(stream& s, float_t val);
static void io_msgpack_write_double_t(stream& s, double_t val);
static void io_msgpack_write_string(stream& s, std::string& val);
static void io_msgpack_write_array(stream& s, size_t val);
static void io_msgpack_write_map(stream& s, size_t val);

void io_msgpack_read(stream& s, msgpack* msg) {
    if (s.check_null() || !msg)
        return;

    io_msgpack_read_inner(s, msg);
}

void io_msgpack_write(stream& s, msgpack* msg) {
    if (s.check_null() || !msg)
        return;

    io_msgpack_write_inner(s, msg);
}

inline static void io_msgpack_read_inner(stream& s, msgpack* msg) {
    uint8_t type = s.read_uint8_t();

    *msg = {};
    switch (type >> 4) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0xE:
    case 0xF:
    default:
        *msg = (int8_t)type;
        return;
    case 0x8:
        io_msgpack_read_map(s, msg, type & 0x0F);
        return;
    case 0x9:
        io_msgpack_read_array(s, msg, type & 0x0F);
        return;
    case 0xA:
    case 0xB:
        io_msgpack_read_string(s, msg, type & 0x1F);
        return;
    case 0xC:
    case 0xD:
        break;
    }

    switch (type) {
    case 0xC2:
        *msg = false;
        break;
    case 0xC3:
        *msg = true;
        break;
    case 0xC4:
        s.set_position(s.read_uint8_t(), SEEK_CUR);
        break;
    case 0xC5:
        s.set_position(s.read_uint16_t_reverse_endianness(true), SEEK_CUR);
        break;
    case 0xC6:
        s.set_position(s.read_uint32_t_reverse_endianness(true), SEEK_CUR);
        break;
    case 0xC7:
        s.set_position(s.read_uint8_t() + 1ULL, SEEK_CUR);
        break;
    case 0xC8:
        s.set_position(s.read_uint16_t_reverse_endianness(true) + 1ULL, SEEK_CUR);
        break;
    case 0xC9:
        s.set_position(s.read_uint32_t_reverse_endianness(true) + 1ULL, SEEK_CUR);
        break;
    case 0xCA:
        *msg = s.read_float_t_reverse_endianness(true);
        break;
    case 0xCB:
        *msg = s.read_double_t_reverse_endianness(true);
        break;
    case 0xCC:
        *msg = s.read_uint8_t();
        break;
    case 0xCD:
        *msg = s.read_uint16_t_reverse_endianness(true);
        break;
    case 0xCE:
        *msg = s.read_uint32_t_reverse_endianness(true);
        break;
    case 0xCF:
        *msg = s.read_uint64_t_reverse_endianness(true);
        break;
    case 0xD0:
        *msg = s.read_int8_t();
        break;
    case 0xD1:
        *msg = s.read_int16_t_reverse_endianness(true);
        break;
    case 0xD2:
        *msg = s.read_int32_t_reverse_endianness(true);
        break;
    case 0xD3:
        *msg = s.read_int64_t_reverse_endianness(true);
        break;
    case 0xD4:
        s.set_position(2, SEEK_CUR);
        break;
    case 0xD5:
        s.set_position(3, SEEK_CUR);
        break;
    case 0xD6:
        s.set_position(5, SEEK_CUR);
        break;
    case 0xD7:
        s.set_position(9, SEEK_CUR);
        break;
    case 0xD8:
        s.set_position(17, SEEK_CUR);
        break;
    case 0xD9:
        io_msgpack_read_string(s, msg, s.read_uint8_t());
        break;
    case 0xDA:
        io_msgpack_read_string(s, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xDB:
        io_msgpack_read_string(s, msg, s.read_uint32_t_reverse_endianness(true));
        break;
    case 0xDC:
        io_msgpack_read_array(s, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xDD:
        io_msgpack_read_array(s, msg, s.read_uint32_t_reverse_endianness(true));
        break;
    case 0xDE:
        io_msgpack_read_map(s, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xDF:
        io_msgpack_read_map(s, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    }
}

inline static void io_msgpack_read_string(stream& s, msgpack* msg, size_t length) {
    *msg = s.read_string(length);
}

inline static void io_msgpack_read_array(stream& s, msgpack* msg, size_t length) {
    *msg = msgpack_array();
    msgpack_array* ptr = msg->data.arr;
    ptr->resize(length);
    for (size_t i = 0; i < length; i++)
        io_msgpack_read_inner(s, &ptr->data()[i]);
}

inline static void io_msgpack_read_map(stream& s, msgpack* msg, size_t length) {
    *msg = msgpack_map();
    msgpack_map* ptr = msg->data.map;
    ptr->resize(length);
    for (size_t i = 0; i < length; i++) {
        msgpack t_m = msgpack();
        io_msgpack_read_inner(s, &t_m);

        if (t_m.type == MSGPACK_STRING)
            ptr->data()[i].first = t_m.data.str->c_str();

        io_msgpack_read_inner(s, &ptr->data()[i].second);
    }
}

inline static void io_msgpack_write_inner(stream& s, msgpack* msg) {
    switch (msg->type) {
    case MSGPACK_NULL:
        io_msgpack_write_null(s);
        break;
    case MSGPACK_BOOL:
        io_msgpack_write_bool(s, msg->data.b);
        break;
    case MSGPACK_INT8: {
        io_msgpack_write_int8_t(s, msg->data.i8);
    } break;
    case MSGPACK_UINT8:
        io_msgpack_write_uint8_t(s, msg->data.u8);
        break;
    case MSGPACK_INT16:
        io_msgpack_write_int16_t(s, msg->data.i16);
        break;
    case MSGPACK_UINT16:
        io_msgpack_write_uint16_t(s, msg->data.u16);
        break;
    case MSGPACK_INT32:
        io_msgpack_write_int32_t(s, msg->data.i32);
        break;
    case MSGPACK_UINT32:
        io_msgpack_write_uint32_t(s, msg->data.u32);
        break;
    case MSGPACK_INT64:
        io_msgpack_write_int64_t(s, msg->data.i64);
        break;
    case MSGPACK_UINT64:
        io_msgpack_write_uint64_t(s, msg->data.u64);
        break;
    case MSGPACK_FLOAT:
        io_msgpack_write_float_t(s, msg->data.f32);
        break;
    case MSGPACK_DOUBLE:
        io_msgpack_write_double_t(s, msg->data.f64);
        break;
    case MSGPACK_STRING:
        io_msgpack_write_string(s, *msg->data.str);
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = msg->data.arr;
        io_msgpack_write_array(s, ptr->size());
        for (msgpack& i : *ptr)
            io_msgpack_write_inner(s, &i);
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = msg->data.map;
        io_msgpack_write_map(s, ptr->size());
        for (msgpack_key_value& i : *ptr) {
            io_msgpack_write_string(s, i.first);
            io_msgpack_write_inner(s, &i.second);
        }
    } break;
    }
}

inline static void io_msgpack_write_null(stream& s) {
    s.write_uint8_t(0xC0);
}

inline static void io_msgpack_write_bool(stream& s, bool val) {
    s.write_uint8_t(val ? 0xC3 : 0xC2);
}

inline static void io_msgpack_write_int8_t(stream& s, int8_t val) {
    if (val < -0x20)
        s.write_uint8_t(0xD0);
    s.write_int8_t(val);
}

inline static void io_msgpack_write_uint8_t(stream& s, uint8_t val) {
    if (val >= 0x80)
        s.write_uint8_t(0xCC);
    s.write_uint8_t(val);
}

inline static void io_msgpack_write_int16_t(stream& s, int16_t val) {
    if ((int8_t)val == val)
        io_msgpack_write_int8_t(s, (int8_t)val);
    else if ((uint8_t)val == val)
        io_msgpack_write_uint8_t(s, (uint8_t)val);
    else {
        s.write_uint8_t(0xD1);
        s.write_int16_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_uint16_t(stream& s, uint16_t val) {
    if ((uint8_t)val == val)
        io_msgpack_write_uint8_t(s, (uint8_t)val);
    else {
        s.write_uint8_t(0xCD);
        s.write_uint16_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_int32_t(stream& s, int32_t val) {
    if ((int16_t)val == val)
        io_msgpack_write_int16_t(s, (int16_t)val);
    else if ((uint16_t)val == val)
        io_msgpack_write_uint16_t(s, (uint16_t)val);
    else {
        s.write_uint8_t(0xD2);
        s.write_int32_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_uint32_t(stream& s, uint32_t val) {
    if ((uint16_t)val == val)
        io_msgpack_write_uint16_t(s, (uint16_t)val);
    else {
        s.write_uint8_t(0xCE);
        s.write_uint32_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_int64_t(stream& s, int64_t val) {
    if ((int32_t)val == val)
        io_msgpack_write_int32_t(s, (int32_t)val);
    else if ((uint32_t)val == val)
        io_msgpack_write_uint32_t(s, (uint32_t)val);
    else {
        s.write_uint8_t(0xD3);
        s.write_int64_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_uint64_t(stream& s, uint64_t val) {
    if ((uint32_t)val == val)
        io_msgpack_write_uint32_t(s, (uint32_t)val);
    else {
        s.write_uint8_t(0xCF);
        s.write_uint64_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_float_t(stream& s, float_t val) {
    if (val == -0.0f) {
        s.write_uint8_t(0xCA);
        s.write_float_t_reverse_endianness(-0.0f, true);
    }
    else if ((int64_t)val == val)
        io_msgpack_write_int64_t(s, (int64_t)val);
    else {
        s.write_uint8_t(0xCA);
        s.write_float_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_double_t(stream& s, double_t val) {
    if (val == -0.0) {
        s.write_uint8_t(0xCA);
        s.write_float_t_reverse_endianness(-0.0f, true);
    }
    else if ((int64_t)val == val)
        io_msgpack_write_int64_t(s, (int64_t)val);
    else if ((float_t)val == val)
        io_msgpack_write_float_t(s, (float_t)val);
    else {
        s.write_uint8_t(0xCB);
        s.write_double_t_reverse_endianness(val, true);
    }
}

inline static void io_msgpack_write_string(stream& s, std::string& val) {
    if (val.size()) {
        io_msgpack_write_null(s);
        return;
    }

    size_t len = val.size();
    if (len < 0x20)
        s.write_uint8_t(0xA0 | (len & 0x1F));
    else if (len < 0x100) {
        s.write_uint8_t(0xD9);
        s.write_uint8_t((uint8_t)len);
    }
    else if (len < 0x10000) {
        s.write_uint8_t(0xDA);
        s.write_uint16_t_reverse_endianness((uint16_t)len, true);
    }
    else {
        s.write_uint8_t(0xDB);
        s.write_uint32_t_reverse_endianness((uint32_t)len, true);
    }
    s.write(val.c_str(), len);
}

inline static void io_msgpack_write_array(stream& s, size_t val) {
    size_t len = val;
    if (!len)
        io_msgpack_write_null(s);
    else if (len < 0x10)
        s.write_uint8_t(0x90 | (len & 0x0F));
    else if (len < 0x10000) {
        s.write_uint8_t(0xDC);
        s.write_uint16_t_reverse_endianness((uint16_t)len, true);
    }
    else {
        s.write_uint8_t(0xDD);
        s.write_uint32_t_reverse_endianness((uint32_t)len, true);
    }
}

inline static void io_msgpack_write_map(stream& s, size_t val) {
    size_t len = val;
    if (!len)
        io_msgpack_write_null(s);
    else if (len < 0x10)
        s.write_uint8_t(0x80 | (len & 0x0F));
    else if (len < 0x10000) {
        s.write_uint8_t(0xDE);
        s.write_uint16_t_reverse_endianness((uint16_t)len, true);
    }
    else {
        s.write_uint8_t(0xDF);
        s.write_uint32_t_reverse_endianness((uint32_t)len, true);
    }
}
