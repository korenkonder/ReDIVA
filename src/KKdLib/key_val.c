/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "key_val.h"
#include "io/path.h"
#include "hash.h"
#include "str_utils.h"

static ssize_t key_val_get_key_index(key_val* kv, char* str, size_t length);
static void key_val_sort(key_val* kv);

key_val::key_val() : buf() {

}

key_val::~key_val() {
    free(buf);
}

void key_val::file_read(char* path) {
    if (!path || !path_check_file_exists(path))
        return;

    stream s;
    io_open(&s, path, "rb");
    if (s.io.stream) {
        char* d = force_malloc_s(char, s.length);
        io_read(&s, d, s.length);
        parse((uint8_t*)d, s.length);
        free(d);
    }
    io_free(&s);
}

void key_val::file_read(wchar_t* path) {
    if (!path || !path_check_file_exists(path))
        return;

    stream s;
    io_open(&s, path, L"rb");
    if (s.io.stream) {
        char* d = force_malloc_s(char, s.length);
        io_read(&s, d, s.length);
        parse((uint8_t*)d, s.length);
        free(d);
    }
    io_free(&s);
}

bool key_val::get_local_key_val(const char* str, key_val* lkv) {
    lkv->buf = 0;
    lkv->key.clear();
    lkv->key_len.clear();
    lkv->val.clear();
    lkv->key_hash.clear();
    lkv->key_index.clear();

    if (!str)
        return false;

    char** first = 0;
    char** last = 0;
    size_t str_length = utf8_length(str);

    char** i = key.data();
    size_t* i_len = key_len.data();
    size_t j = key.size();
    for (; j; i++, i_len++, j--)
        if (str_length <= *i_len && !memcmp(str, (char*)*i, str_length)) {
            if (!first)
                first = i;
            last = i + 1;
        }
        else if (first)
            break;

    if (!first)
        return false;

    size_t index = first - key.data();
    size_t count = last - first;
    lkv->key = std::vector<char*>(key.begin() + index, key.begin() + index + count);
    lkv->key_len = std::vector<size_t>(key_len.begin() + index, key_len.begin() + index + count);
    lkv->val = std::vector<char*>(val.begin() + index, val.begin() + index + count);
    lkv->val_len = std::vector<size_t>(val_len.begin() + index, val_len.begin() + index + count);

    key_val_sort(lkv);
    return true;
}

bool key_val::has_key(char* str) {
    size_t str_length = utf8_length(str);

    char** i = key.data();
    size_t* i_len = key_len.data();
    size_t j = key.size();
    for (; j; i++, i_len++, j--) {
        size_t len = min(str_length, *i_len);
        if (len && str_length <= *i_len && !memcmp(str, *i, len))
            return true;
    }
    return false;
}

void key_val::parse(uint8_t* data, size_t length) {
    if (!data || !length)
        return;

    char** lines;
    size_t count;
    if (!str_utils_text_file_parse(data, length, &buf, &lines, &count))
        return;

    key.reserve(count);
    key_len.reserve(count);
    val.reserve(count);
    val_len.reserve(count);

    for (size_t i = 0, j = 0; i < count; i++) {
        char* s = lines[i];
        if (*s == '#')
            continue;

        size_t len = utf8_length(s);
        char* c = (char*)memchr(s, '=', len);
        if (!c)
            continue;

        *c = 0;

        char* key_str_data = s;
        char* val_str_data = c + 1;

        size_t key_length = c - s;
        size_t val_length = len - (key_length + 1);

        uint64_t key_hash = hash_fnv1a64m((uint8_t*)key_str_data, key_length, false);

        key.push_back(key_str_data);
        key_len.push_back(key_length);
        val.push_back(val_str_data);
        val_len.push_back(val_length);
        j++;
    }
    key_val_sort(this);
    free(lines)
}

bool key_val::read_bool(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, bool* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = false;
        return false;
    }
    *value = atoi(val[index]) ? true : false;
    return true;
}

bool key_val::read_float_t(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, float_t* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0.0f;
        return false;
    }
    *value = (float_t)atof(val[index]);
    return true;
}

bool key_val::read_int32_t(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, int32_t* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0;
        return false;
    }
    *value = atoi(val[index]);
    return true;
}

bool key_val::read_uint32_t(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, uint32_t* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0;
        return false;
    }
    *value = atoi(val[index]);
    return true;
}

bool key_val::read_string(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, string* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = string_empty;
        return false;
    }
    string_init_length(value, val[index], val_len[index]);
    return true;
}

bool key_val::read_string(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, std::string* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = std::string();
        return false;
    }
    *value = std::string(val[index], val_len[index]);
    return true;
}

bool key_val::read_string(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, char** value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(this, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0;
        return false;
    }
    *value = val[index];
    return true;
}

bool key_val::read_vec3(char* buf, size_t offset,
    const char* str_add, size_t str_add_len, vec3* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    key_val lkv;
    if (!get_local_key_val(buf, &lkv)) {
        memset(value, 0, sizeof(vec3));
        return false;
    }

    lkv.read_float_t(buf, offset, ".x", 3, &value->x);
    lkv.read_float_t(buf, offset, ".y", 3, &value->y);
    lkv.read_float_t(buf, offset, ".z", 3, &value->z);
    return true;
}

void key_val::get_lexicographic_order(std::vector<int32_t>* vec, int32_t length) {
    vec->clear();

    int32_t i, j, m;

    if (length < 1)
        return;

    vec->reserve(length);

    j = 0;
    vec->push_back(j);
    i = 1;
    j = 1;

    m = 1;
    while (m < length)
        m *= 10;

    while (i < length) {
        if (j * 10 < m) {
            vec->push_back(j);
            i++;
            j *= 10;
        }
        else if (j >= length) {
            m /= 10;
            j /= 10;
            while (j % 10 == 9)
                j /= 10;
            j++;
        }
        else if (j % 10 != 9) {
            vec->push_back(j);
            i++;
            j++;
        }

        if (i < length)
            if (j == length && j % 10 != 9) {
                m /= 10;
                j /= 10;
                while (j % 10 == 9)
                    j /= 10;
                j++;
            }
            else if (j < length && j % 10 == 9) {
                vec->push_back(j);
                i++;
                while (j % 10 == 9)
                    j /= 10;
                j++;
            }
    }
}

void key_val::write_bool(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, bool value) {
    if (!value)
        return;

    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    io_write_utf8_string(s, buf);
    io_write(s, "=1\n", 3);
}

void key_val::write_float_t(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, float_t value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%g", value);

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write_utf8_string(s, val_buf);
    io_write_char(s, '\n');
}

void key_val::write_int32_t(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, int32_t value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%d", value);

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write_utf8_string(s, val_buf);
    io_write_char(s, '\n');
}

void key_val::write_uint32_t(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, uint32_t value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%u", value);

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write_utf8_string(s, val_buf);
    io_write_char(s, '\n');
}

void key_val::write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, string* value) {
    if (!string_data(value))
        return;

    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write(s, string_data(value), value->length);
    io_write_char(s, '\n');
}

void key_val::write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, std::string* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write(s, value->c_str(), value->size());
    io_write_char(s, '\n');
}

void key_val::write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, char* value) {
    if (!value)
        return;

    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write_utf8_string(s, value);
    io_write_char(s, '\n');
}

void key_val::write_string(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, const char* value) {
    if (!value)
        return;

    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    io_write_utf8_string(s, buf);
    io_write_char(s, '=');
    io_write_utf8_string(s, value);
    io_write_char(s, '\n');
}

void key_val::write_vec3(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, vec3* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    write_float_t(s, buf, offset, ".x", 3, value->x);
    write_float_t(s, buf, offset, ".y", 3, value->y);
    write_float_t(s, buf, offset, ".z", 3, value->z);
}

static ssize_t key_val_get_key_index(key_val* kv, char* str, size_t length) {
    if (kv->key_hash.size() < 1)
        return -1;

    uint64_t hash = hash_fnv1a64m((uint8_t*)str, length, false);

    std::vector<uint64_t>::iterator key = kv->key_hash.begin();
    size_t len = kv->key.size();
    size_t temp;
    while (len > 0) {
        if (hash < key[temp = len / 2])
            len = temp;
        else {
            key += temp + 1;
            len -= temp + 1;
        }
    }

    if (key - kv->key_hash.begin() == 0)
        if (key[0] == hash)
            return kv->key_index[key - kv->key_hash.begin()];
        else
            return -1;
    else if (key[-1] == hash)
        return kv->key_index[key - kv->key_hash.begin() - 1];
    else if (key == kv->key_hash.end())
        return -1;
    else if (key[0] == hash)
        return kv->key_index[key - kv->key_hash.begin()];
    return -1;
}

#define RADIX_BASE 8
#define RADIX (1 << RADIX_BASE)

static void key_val_sort(key_val* kv) {
    size_t count = kv->key.size();

    kv->key_hash.resize(count);
    kv->key_index.resize(count);

    char** key = kv->key.data();
    size_t* key_len = kv->key_len.data();
    uint64_t* key_hash = kv->key_hash.data();
    size_t* key_index = kv->key_index.data();
    for (size_t i = 0; i < count; i++) {
        *key_index++ = i;
        *key_hash++ = hash_fnv1a64m((uint8_t*)*key++, *key_len++, false);
    }

    if (count < 2)
        return;

    uint64_t* o_key_hash = (uint64_t*)force_malloc_s(uint64_t, count);
    size_t* o_key_index = (size_t*)force_malloc_s(size_t, count);
    size_t* c = (size_t*)force_malloc_s(size_t, (1 << 8));
    uint64_t* arr_key_hash = kv->key_hash.data();
    size_t* arr_key_index = kv->key_index.data();
    uint64_t* org_arr = arr_key_hash;

    for (size_t shift = 0, s = 0; shift < sizeof(uint64_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < count; i++)
            c[(arr_key_hash[i] >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (ssize_t i = count - 1; i >= 0; i--) {
            size_t index = --c[(arr_key_hash[i] >> s) & (RADIX - 1)];
            o_key_hash[index] = arr_key_hash[i];
            o_key_index[index] = arr_key_index[i];
        }

        uint64_t* t_key_hash = arr_key_hash;
        arr_key_hash = o_key_hash;
        o_key_hash = t_key_hash;

        size_t* t_key_index = arr_key_index;
        arr_key_index = o_key_index;
        o_key_index = t_key_index;
    }

    if (org_arr == o_key_hash) {
        uint64_t* t_key_hash = arr_key_hash;
        arr_key_hash = o_key_hash;
        o_key_hash = t_key_hash;

        size_t* t_key_index = arr_key_index;
        arr_key_index = o_key_index;
        o_key_index = t_key_index;

        memmove(arr_key_hash, o_key_hash, sizeof(uint64_t) * count);
        memmove(arr_key_index, o_key_index, sizeof(size_t) * count);
    }

    free(o_key_hash);
    free(o_key_index);
    free(c);
}
