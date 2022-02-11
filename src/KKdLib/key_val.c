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

void key_val_init(key_val* kv, uint8_t* data, size_t length) {
    kv->buf = 0;
    kv->key = vector_ptr_empty(char);
    kv->key_len = vector_empty(size_t);
    kv->val = vector_ptr_empty(char);
    kv->val_len = vector_empty(size_t);
    kv->key_hash = vector_empty(uint64_t);
    kv->key_index = vector_empty(size_t);

    if (!data || !length)
        return;

    char** lines;
    size_t count;
    if (!str_utils_text_file_parse(data, length, &kv->buf, &lines, &count))
        return;

    vector_ptr_char_reserve(&kv->key, count);
    vector_size_t_reserve(&kv->key_len, count);
    vector_ptr_char_reserve(&kv->val, count);
    vector_size_t_reserve(&kv->val_len, count);

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

        vector_ptr_char_push_back(&kv->key, &key_str_data);
        vector_size_t_push_back(&kv->key_len, &key_length);
        vector_ptr_char_push_back(&kv->val, &val_str_data);
        vector_size_t_push_back(&kv->val_len, &val_length);
        j++;
    }
    key_val_sort(kv);
    free(lines)
}

void key_val_file_read(key_val* kv, char* path) {
    if (!kv)
        return;

    if (!path || !path_check_file_exists(path)) {
        key_val_init(kv, 0, 0);
        return;
    }

    stream s;
    io_open(&s, path, "rb");
    if (s.io.stream) {
        char* d = force_malloc_s(char, s.length);
        io_read(&s, d, s.length);
        key_val_init(kv, (uint8_t*)d, s.length);
        free(d);
    }
    io_free(&s);
}

void key_val_wfile_read(key_val* kv, wchar_t* path) {
    if (!kv)
        return;

    if (!path || !path_wcheck_file_exists(path)) {
        key_val_init(kv, 0, 0);
        return;
    }

    stream s;
    io_wopen(&s, path, L"rb");
    if (s.io.stream) {
        char* d = force_malloc_s(char, s.length);
        io_read(&s, d, s.length);
        key_val_init(kv, (uint8_t*)d, s.length);
        free(d);
    }
    io_free(&s);
}

bool key_val_get_local_key_val(key_val* kv, char* str, key_val* lkv) {
    lkv->buf = 0;
    lkv->key = vector_ptr_empty(char);
    lkv->key_len = vector_empty(size_t);
    lkv->val = vector_ptr_empty(char);
    lkv->key_hash = vector_empty(uint64_t);
    lkv->key_index = vector_empty(size_t);

    if (!str)
        return false;

    char** first = 0;
    char** last = 0;
    size_t str_length = utf8_length(str);

    char** i = kv->key.begin;
    size_t* i_len = kv->key_len.begin;
    size_t j = vector_length(kv->key);
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

    size_t index = first - kv->key.begin;
    size_t count = last - first;
    lkv->key.begin = &kv->key.begin[index];
    lkv->key.end = &kv->key.begin[index + count];
    lkv->key_len.begin = &kv->key_len.begin[index];
    lkv->val.begin = &kv->val.begin[index];
    lkv->val_len.begin = &kv->val_len.begin[index];

    key_val_sort(lkv);
    return true;
}

inline bool key_val_get_local_key_val(key_val* kv, const char* str, key_val* lkv) {
    return key_val_get_local_key_val(kv, (char*)str, lkv);
}

bool key_val_has_key(key_val* kv, char* str) {
    size_t str_length = utf8_length(str);

    char** i = kv->key.begin;
    size_t* i_len = kv->key_len.begin;
    size_t j = vector_length(kv->key);
    for (; j; i++, i_len++, j--) {
        size_t len = min(str_length, *i_len);
        if (len && str_length <= *i_len && !memcmp(str, *i, len))
            return true;
    }
    return false;
}

bool key_val_read_bool(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, bool* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(kv, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = false;
        return false;
    }
    *value = atoi(kv->val.begin[index]) ? true : false;
    return true;
}

void key_val_write_bool(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, bool value) {
    if (!value)
        return;

    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    io_write_utf8_string(s, buf);
    io_write(s, "=1\n", 3);
}

bool key_val_read_float_t(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, float_t* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(kv, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0.0f;
        return false;
    }
    *value = (float_t)atof(kv->val.begin[index]);
    return true;
}

void key_val_write_float_t(stream* s, char* buf,
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

bool key_val_read_int32_t(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, int32_t* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(kv, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0;
        return false;
    }
    *value = atoi(kv->val.begin[index]);
    return true;
}

void key_val_write_int32_t(stream* s, char* buf,
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

bool key_val_read_uint32_t(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, uint32_t* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(kv, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0;
        return false;
    }
    *value = atoi(kv->val.begin[index]);
    return true;
}

void key_val_write_uint32_t(stream* s, char* buf,
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

bool key_val_read_string(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, string* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(kv, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = string_empty;
        return false;
    }
    string_init_length(value, kv->val.begin[index], kv->val_len.begin[index]);
    return true;
}

bool key_val_read_string_ptr(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, char** value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    ssize_t index = key_val_get_key_index(kv, buf, offset);
    buf[offset - (str_add_len - 1)] = 0;
    if (index == -1) {
        *value = 0;
        return false;
    }
    *value = kv->val.begin[index];
    return true;
}

void key_val_write_string(stream* s, char* buf,
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

void key_val_write_string_ptr(stream* s, char* buf,
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

void key_val_write_string_ptr(stream* s, char* buf,
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

bool key_val_read_vec3(key_val* kv, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, vec3* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    key_val lkv;
    if (!key_val_get_local_key_val(kv, buf, &lkv)) {
        memset(value, 0, sizeof(vec3));
        return false;
    }

    key_val_read_float_t(&lkv, buf, offset, ".x", 3, &value->x);
    key_val_read_float_t(&lkv, buf, offset, ".y", 3, &value->y);
    key_val_read_float_t(&lkv, buf, offset, ".z", 3, &value->z);
    key_val_free(&lkv);
    return true;
}

void key_val_write_vec3(stream* s, char* buf,
    size_t offset, const char* str_add, size_t str_add_len, vec3* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    key_val_write_float_t(s, buf, offset, ".x", 3, value->x);
    key_val_write_float_t(s, buf, offset, ".y", 3, value->y);
    key_val_write_float_t(s, buf, offset, ".z", 3, value->z);
}

void key_val_free(key_val* kv) {
    if (kv->buf) {
        size_t count = vector_length(kv->key);
        memset(kv->key.begin, 0, sizeof(char*) * count);
        memset(kv->val.begin, 0, sizeof(char*) * count);

        vector_ptr_char_free(&kv->key, 0);
        vector_size_t_free(&kv->key_len, 0);
        vector_ptr_char_free(&kv->val, 0);
        vector_size_t_free(&kv->val_len, 0);
    }
    vector_uint64_t_free(&kv->key_hash, 0);
    vector_size_t_free(&kv->key_index, 0);
    free(kv->buf);
}

void key_val_get_lexicographic_order(vector_int32_t* vec, int32_t length) {
    vector_int32_t_clear(vec, 0);

    int32_t i, j, m;

    if (length < 1)
        return;

    vector_int32_t_reserve(vec, length);

    j = 0;
    vector_int32_t_push_back(vec, &j);
    i = 1;
    j = 1;

    m = 1;
    while (m < length)
        m *= 10;

    while (i < length) {
        if (j * 10 < m) {
            vector_int32_t_push_back(vec, &j);
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
            vector_int32_t_push_back(vec, &j);
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
                vector_int32_t_push_back(vec, &j);
                i++;
                while (j % 10 == 9)
                    j /= 10;
                j++;
            }
    }
}

static ssize_t key_val_get_key_index(key_val* kv, char* str, size_t length) {
    if (vector_length(kv->key_hash) < 1)
        return -1;

    uint64_t hash = hash_fnv1a64m((uint8_t*)str, length, false);

    uint64_t* key = kv->key_hash.begin;
    size_t len = vector_length(kv->key);
    size_t temp;
    while (len > 0) {
        if (hash < key[temp = len / 2])
            len = temp;
        else {
            key += temp + 1;
            len -= temp + 1;
        }
    }

    if (key - kv->key_hash.begin == 0)
        if (key[0] == hash)
            return kv->key_index.begin[key - kv->key_hash.begin];
        else
            return -1;
    else if (key[-1] == hash)
        return kv->key_index.begin[key - kv->key_hash.begin - 1];
    else if (key[0] == hash)
        return kv->key_index.begin[key - kv->key_hash.begin];
    return -1;
}

#define RADIX_BASE 8
#define RADIX (1 << RADIX_BASE)

static void key_val_sort(key_val* kv) {
    size_t count = vector_length(kv->key);

    kv->key_hash = vector_empty(uint64_t);
    kv->key_index = vector_empty(size_t);
    vector_uint64_t_reserve(&kv->key_hash, count);
    vector_size_t_reserve(&kv->key_index, count);
    kv->key_hash.end += count;
    kv->key_index.end += count;

    char** key = kv->key.begin;
    size_t* key_len = kv->key_len.begin;
    uint64_t* key_hash = kv->key_hash.begin;
    size_t* key_index = kv->key_index.begin;
    for (size_t i = 0; i < count; i++) {
        *key_index++ = i;
        *key_hash++ = hash_fnv1a64m((uint8_t*)*key++, *key_len++, false);
    }

    if (count < 2)
        return;

    uint64_t* o_key_hash = (uint64_t*)force_malloc_s(uint64_t, count);
    size_t* o_key_index = (size_t*)force_malloc_s(size_t, count);
    size_t* c = (size_t*)force_malloc_s(size_t, (1 << 8));
    uint64_t* arr_key_hash = kv->key_hash.begin;
    size_t* arr_key_index = kv->key_index.begin;
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
