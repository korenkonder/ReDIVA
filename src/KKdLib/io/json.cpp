/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "json.hpp"

#define IO_JSON_READ_BUF_SIZE 0x400

struct io_json_read_buffer {
    char data[IO_JSON_READ_BUF_SIZE];
    int32_t first;
    int32_t length;
};

static int32_t io_json_read_char(stream& s, io_json_read_buffer* buf);
static void io_json_seek(stream& s, io_json_read_buffer* buf, int64_t offset);
static void io_json_seek_one(stream& s, io_json_read_buffer* buf);
static void io_json_read_inner(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static void io_json_read_digit(stream& s, io_json_read_buffer* buf,
    int32_t* c, char* dig_buf, size_t* dig_buf_pos, size_t dig_buf_end);
static void io_json_read_float(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static uint8_t io_json_read_string_hex(int32_t c);
static char* io_json_read_string_inner(stream& s, io_json_read_buffer* buf, int32_t* c);
static void io_json_read_string(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static void io_json_read_map(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static void io_json_read_array(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static void io_json_read_bool(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static void io_json_read_null(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c);
static void io_json_write_inner(stream& s, msgpack* msg, size_t tabs);
static void io_json_write_null(stream& s);
static void io_json_write_bool(stream& s, bool val);
static void io_json_write_int8_t(stream& s, int8_t val);
static void io_json_write_uint8_t(stream& s, uint8_t val);
static void io_json_write_int16_t(stream& s, int16_t val);
static void io_json_write_uint16_t(stream& s, uint16_t val);
static void io_json_write_int32_t(stream& s, int32_t val);
static void io_json_write_uint32_t(stream& s, uint32_t val);
static void io_json_write_int64_t(stream& s, int64_t val);
static void io_json_write_uint64_t(stream& s, uint64_t val);
static void io_json_write_float_t(stream& s, float_t val);
static void io_json_write_double_t(stream& s, double_t val);
static void io_json_write_string(stream& s, std::string& val);

void io_json_read(stream& s, msgpack* msg) {
    if (!msg || s.check_null())
        return;

    io_json_read_buffer buf;
    buf.first = 0;
    buf.length = 0;
    buf.data[0] = 0;
    buf.data[IO_JSON_READ_BUF_SIZE - 1] = 0;
    int32_t c = io_json_read_char(s, &buf);
    io_json_read_inner(s, &buf, msg, &c);
}

void io_json_write(stream& s, msgpack* msg) {
    if (s.check_null() || !msg)
        return;

    io_json_write_inner(s, msg, 0);
}

#define CHECK_WHITESPACE(c) (c == 0x20 || c == 0x0A || c == 0x0D || c == 0x09)

inline static int32_t io_json_read_char(stream& s, io_json_read_buffer* buf) {
    if (buf->length == 0) {
        buf->first = 1;
        buf->data[0] = buf->data[IO_JSON_READ_BUF_SIZE - 1];
        buf->length = (int32_t)s.read(&buf->data[0x01], IO_JSON_READ_BUF_SIZE - 1);
    }

    if (buf->length < 1)
        return EOF;

    int32_t c = buf->data[buf->first];
    buf->first++;
    buf->length--;
    return c;
}

inline static void io_json_seek(stream& s, io_json_read_buffer* buf, int64_t offset) {
    if (buf->first < offset || offset >= buf->length - 1LL) {
        int64_t pos = s.get_position() - buf->length - offset;
        int64_t off = pos % IO_JSON_READ_BUF_SIZE;
        pos -= off;
        if (pos > 1) {
            s.set_position(pos, SEEK_SET);
            buf->first = (int32_t)off;
            buf->length = (int32_t)s.read(&buf->data[0x00], IO_JSON_READ_BUF_SIZE);
        }
        else {
            s.set_position(pos, SEEK_SET);
            buf->first = (int32_t)(off + 1);
            buf->data[0] = 0;
            buf->length = (int32_t)s.read(&buf->data[0x01], IO_JSON_READ_BUF_SIZE - 1);
        }
        buf->length -= (int32_t)off;
    }
    else {
        buf->first -= (int32_t)offset;
        buf->length += (int32_t)offset;
    }
}

inline static void io_json_seek_one(stream& s, io_json_read_buffer* buf) {
    buf->first--;
    buf->length++;
}

static void io_json_read_inner(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    *msg = {};
    if (*c == EOF)
        return;

    if (CHECK_WHITESPACE(*c))
        *c = io_json_read_char(s, buf);

    switch (*c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
        io_json_read_float(s, buf, msg, c);
        break;
    case '"':
        io_json_read_string(s, buf, msg, c);
        break;
    case '{':
        io_json_read_map(s, buf, msg, c);
        break;
    case '[':
        io_json_read_array(s, buf, msg, c);
        break;
    case 't':
        io_json_read_bool(s, buf, msg, c);
        break;
    case 'f':
        io_json_read_bool(s, buf, msg, c);
        break;
    case 'n':
        io_json_read_null(s, buf, msg, c);
        break;
    }
}

inline static void io_json_read_digit(stream& s, io_json_read_buffer* buf,
    int32_t* c, char* dig_buf, size_t* dig_buf_pos, size_t dig_buf_end) {
    size_t b = *dig_buf_pos;
    while (*c >= '0' && *c <= '9' && b < dig_buf_end) {
        dig_buf[b++] = *c;
        if ((*c = io_json_read_char(s, buf)) == EOF)
            break;
    }
    *dig_buf_pos = b;
}

static void io_json_read_float(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    char dig_buf[0x100];
    size_t buf_pos = 0;
    size_t buf_end = 0x100;
    bool negate = false;
    bool zero = false;
    bool fraction = false;
    memset(dig_buf, 0, 0x100);

    if (*c == '-') {
        dig_buf[buf_pos++] = *c;
        if ((*c = io_json_read_char(s, buf)) == EOF)
            return;
        negate = true;
    }

    if (*c == '0') {
        dig_buf[buf_pos++] = *c;
        if ((*c = io_json_read_char(s, buf)) == EOF)
            return;
        zero = true;
    }
    else
        io_json_read_digit(s, buf, c, dig_buf, &buf_pos, buf_end);

    if (*c == '.') {
        dig_buf[buf_pos++] = *c;
        if ((*c = io_json_read_char(s, buf)) == EOF)
            return;
        io_json_read_digit(s, buf, c, dig_buf, &buf_pos, buf_end);
        fraction = true;
    }

    if (zero && !fraction) {
        if (negate)
            *msg = -0.0f;
        else
            *msg = 0;
        io_json_seek_one(s, buf);
        return;
    }
    else if (*c != 'e' && *c != 'E' && !fraction) {
        int64_t val;
        if (sscanf_s(dig_buf, "%lli", &val) == EOF)
            return;

        if (val >= 0x00 && val <= 0xFF)
            *msg = (uint8_t)val;
        else if (val >= (int8_t)0x80 && val <= (int8_t)0x7F)
            *msg = (int8_t)val;
        else if (val >= 0x0000 && val <= 0xFFFF)
            *msg = (uint16_t)val;
        else if (val >= (int16_t)0x8000 && val <= (int16_t)0x7FFF)
            *msg = (int16_t)val;
        else if (val >= 0x00000000 && val <= 0xFFFFFFFF)
            *msg = (uint32_t)val;
        else if (val >= (int32_t)0x80000000 && val <= (int32_t)0x7FFFFFFF)
            *msg = (int32_t)val;
        else
            *msg = val;
        io_json_seek_one(s, buf);
        return;
    }
    else if (*c == 'e' || *c == 'E') {
        dig_buf[buf_pos++] = *c;
        if ((*c = io_json_read_char(s, buf)) == EOF)
            return;

        if (*c == '+' || *c == '-') {
            dig_buf[buf_pos++] = *c;
            if ((*c = io_json_read_char(s, buf)) == EOF)
                return;
        }
        io_json_read_digit(s, buf, c, dig_buf, &buf_pos, buf_end);
    }

    double_t val;
#pragma warning(suppress: 6054)
    if (sscanf_s(dig_buf, "%lg", &val) == EOF)
        return;

    if (val == (float_t)val)
        *msg = (float_t)val;
    else
        *msg = val;
    io_json_seek_one(s, buf);
}

inline static uint8_t io_json_read_string_hex(int32_t c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 0xA;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 0xA;
    else
        return 0;
}

static char* io_json_read_string_inner(stream& s, io_json_read_buffer* buf, int32_t* c) {
    if (*c != '"')
        return 0;

    int64_t pos = s.get_position() - buf->length;
    size_t len = 0;
    while (*c != EOF) {
        *c = io_json_read_char(s, buf);
        if (*c == '\\') {
            if ((*c = io_json_read_char(s, buf)) == EOF)
                break;

            switch (*c) {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                len++;
                break;
            case 'u': {
                uint32_t uc = 0;
                if ((*c = io_json_read_char(s, buf)) == EOF)
                    break;
                uc |= io_json_read_string_hex(*c) << 24;

                if ((*c = io_json_read_char(s, buf)) == EOF)
                    break;
                uc |= io_json_read_string_hex(*c) << 16;

                if ((*c = io_json_read_char(s, buf)) == EOF)
                    break;
                uc |= io_json_read_string_hex(*c) << 8;

                if ((*c = io_json_read_char(s, buf)) == EOF)
                    break;
                uc |= io_json_read_string_hex(*c);

                if (uc <= 0x7F)
                    len++;
                else if (*c <= 0x7FF)
                    len += 2;
                else
                    len += 3;
            } break;
            }
        }
        else if (*c == '"')
            break;
        else
            len++;
    }
    if (*c == EOF)
        return 0;
    int64_t pos_end = s.get_position() - buf->length;

    io_json_seek(s, buf, pos_end - pos);
    char* temp = force_malloc<char>(len + 1);
    for (size_t i = 0; i < len;) {
        *c = io_json_read_char(s, buf);

        if (*c == '\\') {
            *c = io_json_read_char(s, buf);

            switch (*c) {
            case '"':
                temp[i++] = '"';
                break;
            case '\\':
                temp[i++] = '\\';
                break;
            case '/':
                temp[i++] = '/';
                break;
            case 'b':
                temp[i++] = '\b';
                break;
            case 'f':
                temp[i++] = '\f';
                break;
            case 'n':
                temp[i++] = '\n';
                break;
            case 'r':
                temp[i++] = '\r';
                break;
            case 't':
                temp[i++] = '\t';
                break;
            case 'u': {
                uint32_t uc = 0;
                uc |= io_json_read_string_hex(io_json_read_char(s, buf)) << 24;
                uc |= io_json_read_string_hex(io_json_read_char(s, buf)) << 16;
                uc |= io_json_read_string_hex(io_json_read_char(s, buf)) << 8;
                uc |= io_json_read_string_hex(io_json_read_char(s, buf));
                if (uc <= 0x7F)
                    temp[i] = (char)uc;
                else if (uc <= 0x7FF) {
                    temp[i++] = 0xC0 | ((uc >> 6) & 0x1F);
                    temp[i] = 0x80 | (uc & 0x3F);
                }
                else {
                    temp[i++] = 0xE0 | ((uc >> 12) & 0xF);
                    temp[i++] = 0x80 | ((uc >> 6) & 0x3F);
                    temp[i++] = 0x80 | (uc & 0x3F);
                }
            } break;
            }
        }
        else if (*c == '"')
            break;
        else
            temp[i++] = *c;
    }

    if ((*c = io_json_read_char(s, buf)) == EOF)
        return 0;

    return temp;
}

inline static void io_json_read_string(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    char* temp = io_json_read_string_inner(s, buf, c);
    *msg = temp;
    free_def(temp);
}

inline static void io_json_read_skip_whitespace(stream& s, io_json_read_buffer* buf, int32_t* c) {
    while ((*c = io_json_read_char(s, buf)) != EOF) {
        if (CHECK_WHITESPACE(*c))
            continue;
        break;
    }
}

static void io_json_read_map(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    size_t i = 0;
    msgpack m;
    msgpack_map map;
    if (*c != '}') {
        while (true) {
            io_json_read_skip_whitespace(s, buf, c);

            if (*c == '}')
                break;

            char* key = io_json_read_string_inner(s, buf, c);
            if (!key)
                return;

            io_json_read_skip_whitespace(s, buf, c);
            if (*c != ':') {
                free_def(key);
                return;
            }
            io_json_read_skip_whitespace(s, buf, c);

            map.push_back(key, {});
            io_json_read_inner(s, buf, &map.back().second, c);
            free_def(key);

            io_json_read_skip_whitespace(s, buf, c);

            if (*c == '}')
                break;
            else if (*c != ',')
                return;
        }
    }

    *msg = map;
}

static void io_json_read_array(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    size_t i = 0;
    msgpack m;
    msgpack_array array;
    if (*c != ']') {
        while (true) {
            io_json_read_skip_whitespace(s, buf, c);

            if (*c == ']')
                break;

            array.push_back({});
            io_json_read_inner(s, buf, &array.back(), c);

            io_json_read_skip_whitespace(s, buf, c);

            if (*c == ']')
                break;
            else if (*c != ',')
                return;
        }
    }

    *msg = array;
}

inline static void io_json_read_bool(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    if (*c != 't') {
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'a')
            return;
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'l')
            return;
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 's')
            return;
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'e')
            return;
        *msg = false;
    }
    else {
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'r')
            return;
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'u')
            return;
        if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'e')
            return;
        *msg = true;
    }
}

inline static void io_json_read_null(stream& s, io_json_read_buffer* buf, msgpack* msg, int32_t* c) {
    if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'u')
        return;
    if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'l')
        return;
    if ((*c = io_json_read_char(s, buf)) == EOF || *c != 'l')
        return;

    *msg = {};
}

inline static void io_json_write_inner(stream& s, msgpack* msg, size_t tabs) {
    switch (msg->type) {
    case MSGPACK_NULL:
        io_json_write_null(s);
        break;
    case MSGPACK_BOOL:
        io_json_write_bool(s, msg->data.b);
        break;
    case MSGPACK_INT8: {
        io_json_write_int8_t(s, msg->data.i8);
    } break;
    case MSGPACK_UINT8:
        io_json_write_uint8_t(s, msg->data.u8);
        break;
    case MSGPACK_INT16:
        io_json_write_int16_t(s, msg->data.i16);
        break;
    case MSGPACK_UINT16:
        io_json_write_uint16_t(s, msg->data.u16);
        break;
    case MSGPACK_INT32:
        io_json_write_int32_t(s, msg->data.i32);
        break;
    case MSGPACK_UINT32:
        io_json_write_uint32_t(s, msg->data.u32);
        break;
    case MSGPACK_INT64:
        io_json_write_int64_t(s, msg->data.i64);
        break;
    case MSGPACK_UINT64:
        io_json_write_uint64_t(s, msg->data.u64);
        break;
    case MSGPACK_FLOAT32:
        io_json_write_float_t(s, msg->data.f32);
        break;
    case MSGPACK_FLOAT64:
        io_json_write_double_t(s, msg->data.f64);
        break;
    case MSGPACK_STRING:
        io_json_write_string(s, *msg->data.str);
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = msg->data.arr;
        if (!ptr->size()) {
            s.write("[]", 2);
            break;
        }

        s.write("[\n", 2);

        std::string tabs_str;
        for (size_t i = 0; i <= tabs; i++)
            tabs_str.append("  ");

        for (msgpack& i : *ptr) {
            s.write(tabs_str.c_str(), tabs_str.size());
            io_json_write_inner(s, &i, tabs + 1);
            if (&i + 1 != ptr->data() + ptr->size())
                s.write_char(',');
            s.write_char('\n');
        }

        for (size_t i = 0; i < tabs; i++)
            s.write("  ", 2);
        s.write_char(']');
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = msg->data.map;
        if (!ptr->size()) {
            s.write("{}", 2);
            break;
        }

        s.write("{\n", 2);

        std::string tabs_str;
        for (size_t i = 0; i <= tabs; i++)
            tabs_str.append("  ");

        for (auto& i : *ptr) {
            s.write(tabs_str.c_str(), tabs_str.size());
            io_json_write_string(s, i.first);
            s.write(": ", 2);
            io_json_write_inner(s, &i.second, tabs + 1);
            if (&i + 1 != ptr->data() + ptr->size())
                s.write_char(',');
            s.write_char('\n');
        }

        for (size_t i = 0; i < tabs; i++)
            s.write("  ", 2);
        s.write_char('}');
    } break;
    }
}

inline static void io_json_write_null(stream& s) {
    s.write("null", 4);
}

inline static void io_json_write_bool(stream& s, bool val) {
    if (val)
        s.write("true", 4);
    else
        s.write("false", 5);
}

inline static void io_json_write_int8_t(stream& s, int8_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hhi", val);
    s.write(temp, len);
}

inline static void io_json_write_uint8_t(stream& s, uint8_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hhu", val);
    s.write(temp, len);
}

inline static void io_json_write_int16_t(stream& s, int16_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hi", val);
    s.write(temp, len);
}

inline static void io_json_write_uint16_t(stream& s, uint16_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hu", val);
    s.write(temp, len);
}

inline static void io_json_write_int32_t(stream& s, int32_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%i", val);
    s.write(temp, len);
}

inline static void io_json_write_uint32_t(stream& s, uint32_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%u", val);
    s.write(temp, len);
}

inline static void io_json_write_int64_t(stream& s, int64_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%lli", val);
    s.write(temp, len);
}

inline static void io_json_write_uint64_t(stream& s, uint64_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%llu", val);
    s.write(temp, len);
}

inline static void io_json_write_float_t(stream& s, float_t val) {
    char temp[0x40];
    size_t len = sprintf_s(temp, 0x40, "%.15lg", (double_t)val);
    s.write(temp, len);
}

inline static void io_json_write_double_t(stream& s, double_t val) {
    char temp[0x40];
    size_t len = sprintf_s(temp, 0x40, "%.15lg", val);
    s.write(temp, len);
}

inline static void io_json_write_string(stream& s, std::string& val) {
    s.write_char('"');
    const char* str = val.c_str();
    size_t len = val.size();
    for (size_t i = len; i; i--, str++)
        switch (*str) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x0B:
        case 0x0E:
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F: {
            s.write("\\u", 2);

            char t[5];
            size_t len = sprintf_s(t, 5, "%04x", (wchar_t)*str);
            s.write(t, len);
        } break;
        case 0x08:
            s.write("\\b", 2);
            break;
        case 0x09:
            s.write("\\t", 2);
            break;
        case 0x0A:
            s.write("\\n", 2);
            break;
        case 0x0C:
            s.write("\\f", 2);
            break;
        case 0x0D:
            s.write("\\r", 2);
            break;
        case 0x22:
            s.write("\\\"", 2);
            break;
        case 0x5C:
            s.write("\\\\", 2);
            break;
        case 0x2F:
            s.write("\\/", 2);
            break;
        default:
            s.write_char(*str);
        }
    s.write_char('"');
}
