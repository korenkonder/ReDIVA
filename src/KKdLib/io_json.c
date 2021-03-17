/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "io_json.h"
#include "utf8.h"

static void io_json_read_inner(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_read_float(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_read_string(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_read_map(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_read_array(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_read_bool(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_read_null(stream* s, wchar_t* name, msgpack* msg, int32_t* c);
static void io_json_write_inner(stream* s, msgpack* msg, size_t tabs);
static void io_json_write_null(stream* s);
static void io_json_write_bool(stream* s, bool val);
static void io_json_write_int8_t(stream* s, int8_t val);
static void io_json_write_uint8_t(stream* s, uint8_t val);
static void io_json_write_int16_t(stream* s, int16_t val);
static void io_json_write_uint16_t(stream* s, uint16_t val);
static void io_json_write_int32_t(stream* s, int32_t val);
static void io_json_write_uint32_t(stream* s, uint32_t val);
static void io_json_write_int64_t(stream* s, int64_t val);
static void io_json_write_uint64_t(stream* s, uint64_t val);
static void io_json_write_float_t(stream* s, float_t val);
static void io_json_write_double_t(stream* s, double_t val);
static void io_json_write_string(stream* s, wchar_t_buffer* val);

msgpack* io_json_read(stream* s) {
    if (!s->io.stream)
        return 0;

    int32_t c = io_read_char(s);
    msgpack* msg = force_malloc(sizeof(msgpack));
    io_json_read_inner(s, 0, msg, &c);
    return msg;
}

#define CHECK_WHITESPACE(c) (c == 0x20 || c == 0x0A || c == 0x0D || c == 0x09)

static void io_json_read_inner(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    msgpack_set_null(msg, name);
    while (*c != IO_EOF) {
        if (CHECK_WHITESPACE(*c)) {
            *c = io_read_char(s);
        }

        if (isdigit(*c)) {
            io_json_read_float(s, name, msg, c);
            return;
        }

        switch (*c) {
        case '\"':
            io_json_read_string(s, name, msg, c);
            break;
        case '{':
            io_json_read_map(s, name, msg, c);
            break;
        case '[':
            io_json_read_array(s, name, msg, c);
            break;
        case '-':
            io_json_read_float(s, name, msg, c);
            break;
        case 't':
            io_json_read_bool(s, name, msg, c);
            break;
        case 'f':
            io_json_read_bool(s, name, msg, c);
            break;
        case 'n':
            io_json_read_null(s, name, msg, c);
            break;
        }
        return;
    }
}

static void io_json_read_digit(stream* s, int32_t* c, char** buf, char* buf_end) {
    char* b = *buf;
    while (isdigit(*c) && b < buf_end) {
        *b++ = *c;
        if ((*c = io_read_char(s)) == IO_EOF)
            break;
    }
    *buf = b;
}

static void io_json_read_float(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    char buf[0x100];
    char* t_buf = buf;
    char* buf_end = buf + 0x100;
    memset(buf, 0, 0x100);

    if (*c == '-') {
        *t_buf++ = *c;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
    }

    if (*c == '0') {
        *t_buf++ = *c;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
    }
    else
        io_json_read_digit(s, c, &t_buf, buf_end);

    bool f = false;
    if (*c == '.') {
        f = true;
        *t_buf++ = *c;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        io_json_read_digit(s, c, &t_buf, buf_end);
    }

    if (*c != 'e' && *c != 'E' && !f) {
        int64_t val;
        if (sscanf_s(buf, "%lli", &val) == IO_EOF)
            return;

        if (val >= 0 && val < 0x0100)
            msgpack_set_uint8_t(msg, name, (uint8_t)val);
        else if (val >= -0x80 && val < 0x80)
            msgpack_set_int8_t(msg, name, (int8_t)val);
        else if (val >= 0 && val < 0x10000)
            msgpack_set_uint16_t(msg, name, (uint16_t)val);
        else if (val >= -0x8000 && val < 0x8000)
            msgpack_set_int16_t(msg, name, (int16_t)val);
        else if (val >= 0 && val < 0x100000000)
            msgpack_set_uint32_t(msg, name, (uint32_t)val);
        else if (val >= -0x80000000LL && val < 0x80000000)
            msgpack_set_int32_t(msg, name, (int32_t)val);
        else
            msgpack_set_int64_t(msg, name, val);
        io_set_position(s, -1, IO_SEEK_CUR);
        return;
    }
    else if (*c == 'e' || *c == 'E') {
        *t_buf++ = *c;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;

        if (*c == '+' || *c == '-') {
            *t_buf++ = *c;
            if ((*c = io_read_char(s)) == IO_EOF)
                return;
        }
        io_json_read_digit(s, c, &t_buf, buf_end);
    }

    double_t val;
#pragma warning(suppress: 6054)
    if (sscanf_s(buf, "%lg", &val) == IO_EOF)
        return;

    if ((float_t)val == val)
        msgpack_set_float_t(msg, name, (float_t)val);
    else
        msgpack_set_double_t(msg, name, val);
    io_set_position(s, -1, IO_SEEK_CUR);
}

static uint8_t io_json_read_string_hex(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 0xA;
    else
        return c - 'a' + 0xA;
}

static wchar_t* io_json_read_string_inner(stream* s, msgpack* msg, int32_t* c) {
    if (*c != '"')
        return 0;

    size_t pos = io_get_position(s);
    size_t len = 0;
    while (*c != IO_EOF) {
        *c = io_read_char(s);
        if (*c == '\\')
        {
            if ((*c = io_read_char(s)) == IO_EOF)
                break;

            switch (*c)
            {
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
                wchar_t uc = 0;
                if ((*c = io_read_char(s)) == IO_EOF)
                    break;
                uc = io_json_read_string_hex(*c);

                if ((*c = io_read_char(s)) == IO_EOF)
                    break;
                uc = (uc << 8) | io_json_read_string_hex(*c);

                if ((*c = io_read_char(s)) == IO_EOF)
                    break;
                uc = (uc << 8) | io_json_read_string_hex(*c);

                if ((*c = io_read_char(s)) == IO_EOF)
                    break;
                uc = (uc << 8) | io_json_read_string_hex(*c);

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
    if (*c == IO_EOF)
        return 0;

    io_set_position(s, pos, IO_SEEK_SET);
    char* temp = force_malloc(len + 1);
    for (size_t i = 0; i < len;) {
        *c = io_read_char(s);

        if (*c == '\\')
        {
            *c = io_read_char(s);

            switch (*c)
            {
            case '"':
                temp[i++] = '\"';
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
                wchar_t uc = 0;
                uc = io_json_read_string_hex(io_read_uint8_t(s));
                uc = (uc << 8) | io_json_read_string_hex(io_read_uint8_t(s));
                uc = (uc << 8) | io_json_read_string_hex(io_read_uint8_t(s));
                uc = (uc << 8) | io_json_read_string_hex(io_read_uint8_t(s));
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

    if ((*c = io_read_char(s)) == IO_EOF)
        return 0;

    wchar_t* temp_w = utf8_decode(temp);
    free(temp);
    return temp_w;
}


static void io_json_read_string(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    wchar_t* temp = io_json_read_string_inner(s, msg, c);
    if (temp)
        msgpack_set_string(msg, name, temp);
    free(temp);
}

FORCE_INLINE void io_json_read_skip_whitespace(stream* s, int32_t* c) {
    while ((*c = io_read_char(s)) != IO_EOF) {
        if (CHECK_WHITESPACE(*c))
            continue;
        break;
    }
}

static void io_json_read_map(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    size_t i = 0;
    msgpack* m;
    msgpack_map map;
    memset(&map, 0, sizeof(msgpack_map));
    if (*c != '}') {
        map.length = 0;
        map.fulllength = 1;
        map.data = force_malloc_s(sizeof(msgpack), 1);
        while (true) {
            io_json_read_skip_whitespace(s, c);

            if (*c == '}')
                break;

            wchar_t* key = io_json_read_string_inner(s, msg, c);
            if (!key)
                goto End;

            io_json_read_skip_whitespace(s, c);
            if (*c != ':') {
                free(key);
                goto End;
            }
            io_json_read_skip_whitespace(s, c);

            m = &map.data[i];
            io_json_read_inner(s, key, m, c);
            free(key);
            i++;

            io_json_read_skip_whitespace(s, c);

            if (*c == '}')
                break;
            else if (*c != ',')
                goto End;

            if (i == map.fulllength) {
                map.fulllength *= 2;
                map.fulllength++;
                msgpack* t_m = force_malloc_s(sizeof(msgpack), map.fulllength);
                memcpy(t_m, map.data, sizeof(msgpack) * i);
                free(map.data);
                map.data = t_m;
                map.length = i;
            }
        }

        if (i < map.fulllength) {
            msgpack* t_m = force_malloc_s(sizeof(msgpack), i);
            memcpy(t_m, map.data, sizeof(msgpack) * i);
            free(map.data);
            map.data = t_m;
        }
        map.length = map.fulllength = i;
        goto Success;

    End:
        m = msgpack_init_map(0);
        msgpack_set_map(m, 0, &map);
        msgpack_dispose(m);
        return;
    }

Success:
    msgpack_set_map(msg, name, &map);
}

static void io_json_read_array(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    size_t i = 0;
    msgpack* m;
    msgpack_array array;
    memset(&array, 0, sizeof(msgpack_array));
    if (*c != ']') {
        array.length = 0;
        array.fulllength = 1;
        array.data = force_malloc_s(sizeof(msgpack), 1);
        while (true) {
            io_json_read_skip_whitespace(s, c);

            if (*c == ']')
                break;

            m = &array.data[i];
            io_json_read_inner(s, 0, m, c);
            i++;

            io_json_read_skip_whitespace(s, c);

            if (*c == ']')
                break;
            else if (*c != ',')
                goto End;

            if (i == array.fulllength) {
                array.fulllength *= 2;
                array.fulllength++;
                msgpack* t_m = force_malloc_s(sizeof(msgpack), array.fulllength);
                memcpy(t_m, array.data, sizeof(msgpack) * i);
                free(array.data);
                array.data = t_m;
                array.length = i;
            }
        }

        if (i < array.fulllength) {
            msgpack* t_m = force_malloc_s(sizeof(msgpack), i);
            memcpy(t_m, array.data, sizeof(msgpack) * i);
            free(array.data);
            array.data = t_m;
        }
        array.length = array.fulllength = i;
        goto Success;

        msgpack* m;
    End:
        m = msgpack_init_map(0);
        msgpack_set_array(m, 0, &array);
        msgpack_dispose(m);
        return;
    }

Success:
    msgpack_set_array(msg, name, &array);
}

static void io_json_read_bool(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    if (*c != 't') {
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        msgpack_set_bool(msg, name, false);
    }
    else {
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        if ((*c = io_read_char(s)) == IO_EOF)
            return;
        msgpack_set_bool(msg, name, true);
    }
}

static void io_json_read_null(stream* s, wchar_t* name, msgpack* msg, int32_t* c) {
    if ((*c = io_read_char(s)) == IO_EOF)
        return;
    if ((*c = io_read_char(s)) == IO_EOF)
        return;
    if ((*c = io_read_char(s)) == IO_EOF)
        return;

    msgpack_set_null(msg, name);
}

void io_json_write(stream* s, msgpack* msg) {
    if (!s->io.stream || !msg)
        return;

    io_json_write_inner(s, msg, 0);
}

void io_json_write_inner(stream* s, msgpack* msg, size_t tabs) {
    for (size_t i = 0; i < tabs; i++)
        io_write(s, "  ", 2);

    if (msg->name.length) {
        io_json_write_string(s, &msg->name);
        io_write(s, ": ", 2);
    }

    switch (msg->type) {
    case MSGPACK_NULL:
        io_json_write_null(s);
        break;
    case MSGPACK_BOOL:
        io_json_write_bool(s, *SELECT_MSGPACK(bool, msg));
        break;
    case MSGPACK_INT8: {
        io_json_write_int8_t(s, *SELECT_MSGPACK(int8_t, msg));
    } break;
    case MSGPACK_UINT8:
        io_json_write_uint8_t(s, *SELECT_MSGPACK(uint8_t, msg));
        break;
    case MSGPACK_INT16:
        io_json_write_int16_t(s, *SELECT_MSGPACK(int16_t, msg));
        break;
    case MSGPACK_UINT16:
        io_json_write_uint16_t(s, *SELECT_MSGPACK(uint16_t, msg));
        break;
    case MSGPACK_INT32:
        io_json_write_int32_t(s, *SELECT_MSGPACK(int32_t, msg));
        break;
    case MSGPACK_UINT32:
        io_json_write_uint32_t(s, *SELECT_MSGPACK(uint32_t, msg));
        break;
    case MSGPACK_INT64:
        io_json_write_int64_t(s, *SELECT_MSGPACK(int64_t, msg));
        break;
    case MSGPACK_UINT64:
        io_json_write_uint64_t(s, *SELECT_MSGPACK(uint64_t, msg));
        break;
    case MSGPACK_FLOAT:
        io_json_write_float_t(s, *SELECT_MSGPACK(float_t, msg));
        break;
    case MSGPACK_DOUBLE:
        io_json_write_double_t(s, *SELECT_MSGPACK(double_t, msg));
        break;
    case MSGPACK_STRING:
        io_json_write_string(s, SELECT_MSGPACK(wchar_t_buffer, msg));
        break;
    case MSGPACK_ARRAY: {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, msg);
        io_write(s, "[\n", 2);

        for (size_t i = 0; i < ptr->length; i++) {
            io_json_write_inner(s, &ptr->data[i], tabs + 1);
            if (i + 1 < ptr->length)
                io_write_char(s, ',');
            io_write_char(s, '\n');
        }

        for (size_t i = 0; i < tabs; i++)
            io_write(s, "  ", 2);
        io_write_char(s, ']');
    } break;
    case MSGPACK_MAP: {
        msgpack_map* ptr = SELECT_MSGPACK(msgpack_map, msg);
        io_write(s, "{\n", 2);

        for (size_t i = 0; i < ptr->length; i++) {
            io_json_write_inner(s, &ptr->data[i], tabs + 1);
            if (i + 1 < ptr->length)
                io_write_char(s, ',');
            io_write_char(s, '\n');
        }

        for (size_t i = 0; i < tabs; i++)
            io_write(s, "  ", 2);
        io_write_char(s, '}');
    } break;
    }
}

static void io_json_write_null(stream* s) {
    io_write(s, "null", 4);
}

static void io_json_write_bool(stream* s, bool val) {
    if (val)
        io_write(s, "true", 4);
    else
        io_write(s, "false", 4);
}

static void io_json_write_int8_t(stream* s, int8_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hhi", val);
    io_write(s, temp, len);
}

static void io_json_write_uint8_t(stream* s, uint8_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hhu", val);
    io_write(s, temp, len);
}

static void io_json_write_int16_t(stream* s, int16_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hi", val);
    io_write(s, temp, len);
}

static void io_json_write_uint16_t(stream* s, uint16_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%hu", val);
    io_write(s, temp, len);
}

static void io_json_write_int32_t(stream* s, int32_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%i", val);
    io_write(s, temp, len);
}

static void io_json_write_uint32_t(stream* s, uint32_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%u", val);
    io_write(s, temp, len);
}

static void io_json_write_int64_t(stream* s, int64_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%lli", val);
    io_write(s, temp, len);
}

static void io_json_write_uint64_t(stream* s, uint64_t val) {
    char temp[0x20];
    size_t len = sprintf_s(temp, 0x20, "%llu", val);
    io_write(s, temp, len);
}

static void io_json_write_float_t(stream* s, float_t val) {
    char temp[0x40];
    size_t len = sprintf_s(temp, 0x40, "%.15lg", (double_t)val);
    io_write(s, temp, len);
}

static void io_json_write_double_t(stream* s, double_t val) {
    char temp[0x40];
    size_t len = sprintf_s(temp, 0x40, "%.15lg", val);
    io_write(s, temp, len);
}

static void io_json_write_string(stream* s, wchar_t_buffer* val) {

    char* temp = utf8_encode(wchar_t_buffer_select(val));
    if (!temp) {
        io_json_write_null(s);
        return;
    }

    io_write_char(s, '\"');
    size_t len = strlen(temp);
    for (size_t i = 0; i < len; i++) {
        char c = temp[i];
        if (c == 0x22)
            io_write(s, "\\\"", 2);
        else if (c == 0x5C)
            io_write(s, "\\\\", 2);
        else if (c == 0x2F)
            io_write(s, "\\/", 2);
        else if (c == 0x08)
            io_write(s, "\\b", 2);
        else if (c == 0x0C)
            io_write(s, "\\f", 2);
        else if (c == 0x0A)
            io_write(s, "\\n", 2);
        else if (c == 0x0D)
            io_write(s, "\\r", 2);
        else if (c == 0x09)
            io_write(s, "\\t", 2);
        else if ((uint8_t)c < 0x20) {
            io_write(s, "\\u", 2);

            char t[5];
            size_t len = sprintf_s(t, 5, "%04x", (wchar_t)c);
            io_write(s, t, len);
        }
        else
            io_write_char(s, c);
    }
    io_write_char(s, '\"');
    free(temp);
}
