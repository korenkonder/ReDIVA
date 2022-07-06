/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "key_val.hpp"
#include "io/path.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

static int64_t key_val_get_key_index(key_val* kv, std::string& str);
static int64_t key_val_get_key_index(key_val* kv, std::string&& str);
static void key_val_sort(key_val* kv);

key_val_hash_index_pair::key_val_hash_index_pair() {
    hash = hash_fnv1a64m_empty;
    index = 0;
}

key_val_hash_index_pair::key_val_hash_index_pair(uint64_t hash, size_t index) : hash(hash), index(index) {

}

key_val_scope::key_val_scope() : index(), count() {

}

key_val_scope::~key_val_scope() {

}

key_val_pair::key_val_pair() : str(), length() {

}

key_val_pair::key_val_pair(const char* str, size_t length) : str(str), length(length) {

}

key_val_pair::~key_val_pair() {

}

key_val::key_val() : curr_scope(), buf() {

}

key_val::~key_val() {
    free(buf);
}

void key_val::close_scope() {
    if (scope.size() <= 1)
        return;

    scope.pop_back();
    curr_scope = &scope[scope.size() - 1];
}

void key_val::file_read(const char* path) {
    if (!path || !path_check_file_exists(path))
        return;

    stream s;
    s.open(path, "rb");
    if (s.io.stream) {
        char* d = force_malloc_s(char, s.length);
        s.read(d, s.length);
        parse((uint8_t*)d, s.length);
        free(d);
    }
}

void key_val::file_read(const wchar_t* path) {
    if (!path || !path_check_file_exists(path))
        return;

    stream s;
    s.open(path, L"rb");
    if (s.io.stream) {
        char* d = force_malloc_s(char, s.length);
        s.read(d, s.length);
        parse((uint8_t*)d, s.length);
        free(d);
    }
}

bool key_val::has_key(const char* str) {
    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    const char* s = str;
    size_t s_len = utf8_length(str);

    key_val_pair* i = key.data() + curr_scope->index;
    size_t j = curr_scope->count;
    int32_t res = 0;
    for (; j; i++, j--) {
        if (i->length > offset && s_len <= i->length - offset
            && !(res = memcmp(s, i->str + offset, s_len)))
            return true;
        else if (res < 0)
            return false;
    }
    return false;
}

bool key_val::has_key(std::string& str) {
    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    const char* s = str.c_str();
    size_t s_len = str.size();

    key_val_pair* i = key.data() + curr_scope->index;
    size_t j = curr_scope->count;
    int32_t res = 0;
    for (; j; i++, j--) {
        if (i->length > offset && s_len <= i->length - offset
            && !(res = memcmp(s, i->str + offset, s_len)))
            return true;
        else if (res < 0)
            return false;
    }
    return false;
}

bool key_val::open_scope(std::string& str) {
    if (!str.size()) {
        scope.push_back(*curr_scope);
        curr_scope = &scope[scope.size() - 1];
        return true;
    }

    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    key_val_pair* first = 0;
    key_val_pair* last = 0;
    const char* s = str.c_str();
    size_t s_len = str.size();

    key_val_pair* i = key.data() + curr_scope->index;
    size_t j = curr_scope->count;
    int32_t res = 0;
    for (; j; i++, j--) {
        if (i->length > offset && s_len <= i->length - offset
            && !(res = memcmp(s, i->str + offset, s_len))) {
            if (!first)
                first = i;
            last = i + 1;
        }
        else if (first)
            break;
        else if (res < 0)
            return false;
    }

    if (!first)
        return false;

    scope.push_back({});
    curr_scope = &scope[scope.size() - 1];
    curr_scope->key = curr_scope[-1].key.size() ? curr_scope[-1].key + '.' + str : str;
    curr_scope->index = first - key.data();
    curr_scope->count = last - first;

    key_val_sort(this);
    return true;
}

bool key_val::open_scope(std::string&& key) {
    return open_scope(*(std::string*)&key);
}

bool key_val::open_scope(int32_t i) {
    char buf[0x100];
    size_t len = sprintf_s(buf, sizeof(buf), "%d", i);
    return open_scope(std::string(buf, len));
}

bool key_val::open_scope(uint32_t i) {
    char buf[0x100];
    size_t len = sprintf_s(buf, sizeof(buf), "%u", i);
    return open_scope(std::string(buf, len));
}

bool key_val::open_scope(const char* fmt, ...) {
    char buf[0x200];
    va_list args;
    va_start(args, fmt);
    size_t len = vsprintf_s(buf, sizeof(buf), fmt, args);
    va_end(args);
    return open_scope(std::string(buf, len));
}

void key_val::parse(const void* data, size_t size) {
    if (!data || !size)
        return;

    curr_scope = 0;
    scope.clear();

    char** lines;
    size_t count;
    if (!str_utils_text_file_parse(data, size, &buf, &lines, &count))
        return;

    key.reserve(count);
    val.reserve(count);

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

        uint64_t key_hash = hash_fnv1a64m(key_str_data, key_length);

        key.push_back({ key_str_data, key_length });
        val.push_back({ val_str_data, val_length });
        j++;
    }

    scope.resize(1);
    curr_scope = &scope[0];
    curr_scope->key = {};
    curr_scope->index = 0;
    curr_scope->count = key.size();

    key_val_sort(this);
    free(lines)
}

bool key_val::read(bool& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = false;
        return false;
    }
    value = atoi(val[index].str) ? true : false;
    return true;
}

bool key_val::read(float_t& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0.0f;
        return false;
    }
    value = (float_t)atof(val[index].str);
    return true;
}

bool key_val::read(int32_t& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0;
        return false;
    }
    value = atoi(val[index].str);
    return true;
}

bool key_val::read(uint32_t& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0;
        return false;
    }
    value = (uint32_t)atoll(val[index].str);
    return true;
}

bool key_val::read(const char*& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0;
        return false;
    }
    value = val[index].str;
    return true;
}

bool key_val::read(std::string& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = {};
        return false;
    }
    value = std::string(val[index].str, val[index].length);
    return true;
}

bool key_val::read(vec3& value) {
    read("x", value.x);
    read("y", value.y);
    read("z", value.z);
    return true;
}

bool key_val::read(std::string& key, bool& value) {
    int64_t index = key_val_get_key_index(this,
        curr_scope->key.size() ? curr_scope->key + '.' + key : key);
    if (index == -1) {
        value = false;
        return false;
    }
    value = atoi(val[index].str) ? true : false;
    return true;
}

bool key_val::read(std::string&& key, bool& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key, float_t& value) {
    int64_t index = key_val_get_key_index(this,
        curr_scope->key.size() ? curr_scope->key + '.' + key : key);
    if (index == -1) {
        value = 0.0f;
        return false;
    }
    value = (float_t)atof(val[index].str);
    return true;
}

bool key_val::read(std::string&& key, float_t& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key, int32_t& value) {
    int64_t index = key_val_get_key_index(this,
        curr_scope->key.size() ? curr_scope->key + '.' + key : key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = atoi(val[index].str);
    return true;
}

bool key_val::read(std::string&& key, int32_t& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key, uint32_t& value) {
    int64_t index = key_val_get_key_index(this,
        curr_scope->key.size() ? curr_scope->key + '.' + key : key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = (uint32_t)atoll(val[index].str);
    return true;
}

bool key_val::read(std::string&& key, uint32_t& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key, const char*& value) {
    int64_t index = key_val_get_key_index(this,
        curr_scope->key.size() ? curr_scope->key + '.' + key : key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = val[index].str;
    return true;
}

bool key_val::read(std::string&& key, const char*& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key, std::string& value) {
    int64_t index = key_val_get_key_index(this,
        curr_scope->key.size() ? curr_scope->key + '.' + key : key);
    if (index == -1) {
        value = {};
        return false;
    }
    value = std::string(val[index].str, val[index].length);
    return true;
}

bool key_val::read(std::string&& key, std::string& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key, vec3& value) {
    if (open_scope(key)) {
        read("x", value.x);
        read("y", value.y);
        read("z", value.z);
        close_scope();
        return true;
    }
    return false;
}

bool key_val::read(std::string&& key, vec3& value) {
    return read(*(std::string*)&key, value);
}

bool key_val::read(std::string& key0, std::string& key1, bool& value) {
    if (!open_scope(key0)) {
        value = false;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, bool& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::read(std::string& key0, std::string& key1, float_t& value) {
    if (!open_scope(key0)) {
        value = 0.0f;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, float_t& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::read(std::string& key0, std::string& key1, int32_t& value) {
    if (!open_scope(key0)) {
        value = 0;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, int32_t& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::read(std::string& key0, std::string& key1, uint32_t& value) {
    if (!open_scope(key0)) {
        value = 0;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, uint32_t& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::read(std::string& key0, std::string& key1, const char*& value) {
    if (!open_scope(key0)) {
        value = {};
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, const char*& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::read(std::string& key0, std::string& key1, std::string& value) {
    if (!open_scope(key0)) {
        value = {};
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, std::string& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::read(std::string& key0, std::string& key1, vec3& value) {
    if (!open_scope(key0)) {
        value = vec3_null;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string&& key0, std::string&& key1, vec3& value) {
    return read(*(std::string*)&key0, *(std::string*)&key1, value);
}

bool key_val::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    std::string s = path + std::string(file);

    key_val* kv = (key_val*)data;
    kv->file_read(s.c_str());

    return kv->buf ? true : false;
}

key_val_out::key_val_out() : curr_scope() {

}

key_val_out::~key_val_out() {

}

void key_val_out::close_scope() {
    if (scope.size() <= 1)
        return;

    scope.pop_back();
    curr_scope = &scope[scope.size() - 1];
}

void key_val_out::open_scope(std::string& str) {
    if (!str.size()) {
        scope.push_back(*curr_scope);
        curr_scope = &scope[scope.size() - 1];
        return;
    }

    scope.push_back(curr_scope->size() ? *curr_scope + '.' + str : str);
    curr_scope = &scope[scope.size() - 1];
}

void key_val_out::open_scope(std::string&& key) {
    open_scope(*(std::string*)&key);
}

void key_val_out::open_scope(int32_t i) {
    char buf[0x100];
    size_t len = sprintf_s(buf, sizeof(buf), "%d", i);
    open_scope(std::string(buf, len));
}

void key_val_out::open_scope(uint32_t i) {
    char buf[0x100];
    size_t len = sprintf_s(buf, sizeof(buf), "%u", i);
    open_scope(std::string(buf, len));
}

void key_val_out::write(stream& s, bool value) {
    if (!value)
        return;

    s.write_string(*curr_scope);
    s.write("=1\n", 3);
}

void key_val_out::write(stream& s, float_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%g", value);

    s.write_string(*curr_scope);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, int32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%d", value);

    s.write_string(*curr_scope);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, uint32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%u", value);

    s.write_string(*curr_scope);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, const char* value) {
    s.write_string(*curr_scope);
    s.write_char('=');
    s.write_utf8_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string& value) {
    s.write_string(*curr_scope);
    s.write_char('=');
    s.write_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, vec3& value) {
    write(s, "x", value.x);
    write(s, "y", value.y);
    write(s, "z", value.z);
}

void key_val_out::write(stream& s, vec3&& value) {
    write(s, *(vec3*)&value);
}

void key_val_out::write(stream& s, std::string& key, bool value) {
    if (!value)
        return;

    s.write_string(curr_scope->size() ? *curr_scope + '.' + key : key);
    s.write("=1\n", 3);
}

void key_val_out::write(stream& s, std::string&& key, bool value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, float_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%g", value);

    s.write_string(curr_scope->size() ? *curr_scope + '.' + key : key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string&& key, float_t value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, int32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%d", value);

    s.write_string(curr_scope->size() ? *curr_scope + '.' + key : key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string&& key, int32_t value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, uint32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%u", value);

    s.write_string(curr_scope->size() ? *curr_scope + '.' + key : key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string&& key, uint32_t value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, const char* value) {
    s.write_string(curr_scope->size() ? *curr_scope + '.' + key : key);
    s.write_char('=');
    s.write_utf8_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string&& key, const char* value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, std::string& value) {
    s.write_string(curr_scope->size() ? *curr_scope + '.' + key : key);
    s.write_char('=');
    s.write_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string&& key, std::string& value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, vec3& value) {
    write(s, "x", value.x);
    write(s, "y", value.y);
    write(s, "z", value.z);
}

void key_val_out::write(stream& s, std::string&& key, vec3& value) {
    write(s, *(std::string*)&key, value);
}

void key_val_out::write(stream& s, std::string& key, vec3&& value) {
    write(s, key, *(vec3*)&value);
}

void key_val_out::write(stream& s, std::string&& key, vec3&& value) {
    write(s, *(std::string*)&key, *(vec3*)&value);
}

void key_val_out::get_lexicographic_order(std::vector<int32_t>* vec, int32_t length) {
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

static int64_t key_val_get_key_index(key_val* kv, std::string& str) {
    key_val_scope* curr_scope = kv->curr_scope;

    if (curr_scope->key_hash_index.size() < 1)
        return -1;

    uint64_t hash = hash_string_fnv1a64m(str);

    std::vector<key_val_hash_index_pair>::iterator key = curr_scope->key_hash_index.begin();
    size_t len = curr_scope->count;
    size_t temp;
    while (len > 0) {
        if (hash < key[temp = len / 2].hash)
            len = temp;
        else {
            key += temp + 1;
            len -= temp + 1;
        }
    }

    if (key - curr_scope->key_hash_index.begin() == 0)
        if (key[0].hash == hash)
            return key[0].index;
        else
            return -1;
    else if (key[-1].hash == hash)
        return key[-1].index;
    else if (key == curr_scope->key_hash_index.end())
        return -1;
    else if (key[0].hash == hash)
        return key[0].index;
    return -1;
}

static int64_t key_val_get_key_index(key_val* kv, std::string&& str) {
    return key_val_get_key_index(kv, *(std::string*)&str);
}

#define RADIX_BASE 4
#define RADIX (1 << RADIX_BASE)

static void key_val_sort(key_val* kv) {
    if (!kv->key.size())
        return;

    key_val_scope* curr_scope = kv->curr_scope;

    size_t index = curr_scope->index;
    size_t count = curr_scope->count;

    curr_scope->key_hash_index.resize(count);

    key_val_pair* key = kv->key.data() + index;
    key_val_hash_index_pair* key_hash_index = curr_scope->key_hash_index.data();
    for (size_t i = 0; i < count; i++) {
        key_hash_index->index = index + i;
        key_hash_index->hash = hash_fnv1a64m(key->str, key->length);
        key++;
        key_hash_index++;
    }

    if (count < 2)
        return;

    key_val_hash_index_pair* o_key_hash_index = new key_val_hash_index_pair[count];
    size_t* c = (size_t*)force_malloc_s(size_t, (1 << 8));
    key_val_hash_index_pair* arr_key_hash_index = curr_scope->key_hash_index.data();
    key_val_hash_index_pair* org_arr = arr_key_hash_index;

    for (size_t shift = 0, s = 0; shift < sizeof(uint64_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < count; i++)
            c[(arr_key_hash_index[i].hash >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (int64_t i = count - 1; i >= 0; i--) {
            size_t index = --c[(arr_key_hash_index[i].hash >> s) & (RADIX - 1)];
            o_key_hash_index[index] = arr_key_hash_index[i];
        }

        key_val_hash_index_pair* t_key_hash = arr_key_hash_index;
        arr_key_hash_index = o_key_hash_index;
        o_key_hash_index = t_key_hash;
    }

    if (org_arr == o_key_hash_index) {
        key_val_hash_index_pair* t_key_hash_index = arr_key_hash_index;
        arr_key_hash_index = o_key_hash_index;
        o_key_hash_index = t_key_hash_index;

        memmove(arr_key_hash_index, o_key_hash_index, sizeof(key_val_hash_index_pair) * count);
    }

    delete[] o_key_hash_index;
    free(c);
}
