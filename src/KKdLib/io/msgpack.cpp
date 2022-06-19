/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "msgpack.hpp"

static void io_msgpack_read_inner(stream& s, const char* name, msgpack* msg);
static void io_msgpack_read_string(stream& s, const char* name, msgpack* msg, size_t length);
static void io_msgpack_read_array(stream& s, const char* name, msgpack* msg, size_t length);
static void io_msgpack_read_map(stream& s, const char* name, msgpack* msg, size_t length);
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
    io_msgpack_read_inner(s, 0, msg);
}

static void io_msgpack_read_inner(stream& s, const char* name, msgpack* msg) {
    uint8_t type = s.read_uint8_t();

    *msg = msgpack(name);
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
        *msg = msgpack(name, (int8_t)type);
        return;
    case 0x8:
        io_msgpack_read_map(s, name, msg, type & 0x0F);
        return;
    case 0x9:
        io_msgpack_read_array(s, name, msg, type & 0x0F);
        return;
    case 0xA:
    case 0xB:
        io_msgpack_read_string(s, name, msg, type & 0x1F);
        return;
    case 0xC:
    case 0xD:
        break;
    }

    switch (type) {
    case 0xC2:
        *msg = msgpack(name, false);
        break;
    case 0xC3:
        *msg = msgpack(name, true);
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
        *msg = msgpack(name, s.read_float_t_reverse_endianness(true));
        break;
    case 0xCB:
        *msg = msgpack(name, s.read_double_t_reverse_endianness(true));
        break;
    case 0xCC:
        *msg = msgpack(name, s.read_uint8_t());
        break;
    case 0xCD:
        *msg = msgpack(name, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xCE:
        *msg = msgpack(name, s.read_uint32_t_reverse_endianness(true));
        break;
    case 0xCF:
        *msg = msgpack(name, s.read_uint64_t_reverse_endianness(true));
        break;
    case 0xD0:
        *msg = msgpack(name, s.read_int8_t());
        break;
    case 0xD1:
        *msg = msgpack(name, s.read_int16_t_reverse_endianness(true));
        break;
    case 0xD2:
        *msg = msgpack(name, s.read_int32_t_reverse_endianness(true));
        break;
    case 0xD3:
        *msg = msgpack(name, s.read_int64_t_reverse_endianness(true));
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
        io_msgpack_read_string(s, name, msg, s.read_uint8_t());
        break;
    case 0xDA:
        io_msgpack_read_string(s, name, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xDB:
        io_msgpack_read_string(s, name, msg, s.read_uint32_t_reverse_endianness(true));
        break;
    case 0xDC:
        io_msgpack_read_array(s, name, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xDD:
        io_msgpack_read_array(s, name, msg, s.read_uint32_t_reverse_endianness(true));
        break;
    case 0xDE:
        io_msgpack_read_map(s, name, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    case 0xDF:
        io_msgpack_read_map(s, name, msg, s.read_uint16_t_reverse_endianness(true));
        break;
    }
}

inline static void io_msgpack_read_string(stream& s, const char* name, msgpack* msg, size_t length) {
    std::string str;
    str = s.read_string(length);
    *msg = msgpack(name, &str);
}

inline static void io_msgpack_read_array(stream& s, const char* name, msgpack* msg, size_t length) {
    *msg = msgpack(name, true, length);
    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    for (size_t i = 0; i < length; i++)
        io_msgpack_read_inner(s, 0, &ptr->data()[i]);
}

inline static void io_msgpack_read_map(stream& s, const char* name, msgpack* msg, size_t length) {
    *msg = msgpack(name, false, length);
    msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
    for (size_t i = 0; i < length; i++) {
        msgpack t_m = msgpack();
        io_msgpack_read_inner(s, 0, &t_m);

        const char* n = 0;
        if (t_m.type == MSGPACK_STRING)
            n = MSGPACK_SELECT(std::string, t_m)->c_str();

        io_msgpack_read_inner(s, n, &ptr->data()[i]);
    }
}

void io_msgpack_write(stream& s, msgpack* msg) {
    if (!s.io.stream || !msg)
        return;

    io_msgpack_write_inner(s, msg);
}

static void io_msgpack_write_inner(stream& s, msgpack* msg) {
    if (msg->name.size())
        io_msgpack_write_string(s, msg->name);

    switch (msg->type) {
    case MSGPACK_NULL:
        io_msgpack_write_null(s);
        break;
    case MSGPACK_BOOL:
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
        io_msgpack_write_string(s, *MSGPACK_SELECT_PTR(std::string, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
        io_msgpack_write_array(s, ptr->size());
        for (msgpack& i : *ptr)
            io_msgpack_write_inner(s, &i);
    } break;
    case MSGPACK_MAP: {
        msgpack_array* ptr = MSGPACK_SELECT_PTR(msgpack_array, msg);
        io_msgpack_write_map(s, ptr->size());
        for (msgpack& i : *ptr)
            io_msgpack_write_inner(s, &i);
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