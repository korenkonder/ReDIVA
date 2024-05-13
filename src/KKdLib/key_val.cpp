/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "key_val.hpp"
#include "io/file_stream.hpp"
#include "io/path.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

static int64_t key_val_find_first_key(key_val* kv, int64_t low, int64_t high,
    const char* s, size_t s_len, size_t offset);
static int64_t key_val_find_last_key(key_val* kv, int64_t low, int64_t high,
    const char* s, size_t s_len, size_t offset);
static int64_t key_val_get_key_index(key_val* kv, const char* key);
static int64_t key_val_get_key_index(key_val* kv, std::string& key);
static int64_t key_val_get_key_index(key_val* kv, const char* str, size_t size);
static void key_val_sort(key_val* kv);

key_val_scope::key_val_scope() : index(), count() {

}

key_val_scope::~key_val_scope() {

}

key_val::key_val() : curr_scope(), buf(), scope_size() {

}

key_val::~key_val() {
    free_def(buf);
}

void key_val::close_scope() {
    if (scope_size <= 0)
        return;

    curr_scope->key.clear();
    curr_scope->index = 0;
    curr_scope->count = 0;
    curr_scope->key_hash_index.clear();

    scope_size--;
    curr_scope = &scope[scope_size];
}

void key_val::file_read(const char* path) {
    if (!path || !path_check_file_exists(path))
        return;

    file_stream s;
    s.open(path, "rb");
    if (s.check_not_null()) {
        char* d = force_malloc<char>(s.length);
        s.read(d, s.length);
        parse((uint8_t*)d, s.length);
        free_def(d);
    }
}

void key_val::file_read(const wchar_t* path) {
    if (!path || !path_check_file_exists(path))
        return;

    file_stream s;
    s.open(path, L"rb");
    if (s.check_not_null()) {
        char* d = force_malloc<char>(s.length);
        s.read(d, s.length);
        parse((uint8_t*)d, s.length);
        free_def(d);
    }
}

bool key_val::has_key(const char* str) {
    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    const char* s = str;
    size_t s_len = utf8_length(str);

    int64_t index = key_val_find_first_key(this, curr_scope->index,
        curr_scope->index + curr_scope->count - 1, s, s_len, offset);
    return index != -1;
}

bool key_val::has_key(std::string& str) {
    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    const char* s = str.c_str();
    size_t s_len = str.size();

    int64_t index = key_val_find_first_key(this, curr_scope->index,
        curr_scope->index + curr_scope->count - 1, s, s_len, offset);
    return index != -1;
}

bool key_val::open_scope(const char* str) {
    if (!str || !*str) {
        scope_size++;
        if ((size_t)scope_size >= scope.size())
            scope.resize(scope_size + (scope_size >> 1) + 1);

        curr_scope = &scope[scope_size];
        key_val_scope* prev_scope = curr_scope - 1;
        curr_scope->key.assign(prev_scope->key);
        curr_scope->index = prev_scope->index;
        curr_scope->count = prev_scope->count;
        curr_scope->key_hash_index.resize(prev_scope->key_hash_index.size());
        memmove(curr_scope->key_hash_index.data(), prev_scope->key_hash_index.data(),
            sizeof(std::pair<uint64_t, size_t>) * prev_scope->key_hash_index.size());
        return true;
    }

    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    const char* s = str;
    size_t s_len = utf8_length(str);

    int64_t first_idx = key_val_find_first_key(this, curr_scope->index,
        curr_scope->index + curr_scope->count - 1, s, s_len, offset);
    if (first_idx == -1)
        return false;

    int64_t last_idx = key_val_find_last_key(this, first_idx,
        curr_scope->index + curr_scope->count - 1, s, s_len, offset);
    if (last_idx == -1)
        return false;

    scope_size++;
    if ((size_t)scope_size >= scope.size())
        scope.resize(scope_size + (scope_size >> 1) + 1);

    curr_scope = &scope[scope_size];
    if (curr_scope[-1].key.size()) {
        std::string& new_key = curr_scope->key;
        new_key.clear();
        new_key.reserve(curr_scope[-1].key.size() + s_len + 1);
        new_key.assign(curr_scope[-1].key);
        new_key.append(1, '.');
        new_key.append(str, s_len);
    }
    else
        curr_scope->key.assign(str, s_len);
    curr_scope->index = first_idx;
    curr_scope->count = last_idx - first_idx + 1;
    curr_scope->key_hash_index.clear();

    key_val_sort(this);
    return true;
}

bool key_val::open_scope(std::string& str) {
    if (!str.size()) {
        scope_size++;
        if ((size_t)scope_size >= scope.size())
            scope.resize(scope_size + (scope_size >> 1) + 1);

        curr_scope = &scope[scope_size];
        key_val_scope* prev_scope = curr_scope - 1;
        curr_scope->key.assign(prev_scope->key);
        curr_scope->index = prev_scope->index;
        curr_scope->count = prev_scope->count;
        curr_scope->key_hash_index.resize(prev_scope->key_hash_index.size());
        memmove(curr_scope->key_hash_index.data(), prev_scope->key_hash_index.data(),
            sizeof(std::pair<uint64_t, size_t>) * prev_scope->key_hash_index.size());
        return true;
    }

    size_t offset = curr_scope->key.size();
    if (offset)
        offset++;

    const char* s = str.c_str();
    size_t s_len = str.size();

    int64_t first_idx = key_val_find_first_key(this, curr_scope->index,
        curr_scope->index + curr_scope->count - 1, s, s_len, offset);
    if (first_idx == -1)
        return false;

    int64_t last_idx = key_val_find_last_key(this, first_idx,
        curr_scope->index + curr_scope->count - 1, s, s_len, offset);
    if (last_idx == -1)
        return false;

    scope_size++;
    if ((size_t)scope_size >= scope.size())
        scope.resize(scope_size + (scope_size >> 1) + 1);

    curr_scope = &scope[scope_size];
    if (curr_scope[-1].key.size()) {
        std::string& new_key = curr_scope->key;
        new_key.clear();
        new_key.reserve(curr_scope[-1].key.size() + str.size() + 1);
        new_key.assign(curr_scope[-1].key);
        new_key.append(1, '.');
        new_key.append(str);
    }
    else
        curr_scope->key.assign(str);
    curr_scope->index = first_idx;
    curr_scope->count = last_idx - first_idx + 1;
    curr_scope->key_hash_index.clear();

    key_val_sort(this);
    return true;
}

bool key_val::open_scope_fmt(int32_t i) {
    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%d", i);
    return open_scope(buf);
}

bool key_val::open_scope_fmt(uint32_t i) {
    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%u", i);
    return open_scope(buf);
}

bool key_val::open_scope_fmt(_In_z_ _Printf_format_string_ const char* const fmt, ...) {
    char buf[0x200];
    va_list args;
    va_start(args, fmt);
    vsprintf_s(buf, sizeof(buf), fmt, args);
    va_end(args);
    return open_scope(buf);
}

void key_val::parse(const void* data, size_t size) {
    if (!data || !size)
        return;

    curr_scope = 0;
    scope.clear();

    char** lines;
    size_t count;
    if (!str_utils_text_file_parse(data, size, buf, lines, count))
        return;

    key.reserve(count);
    val.reserve(count);

    for (size_t i = 0, j = 0; i < count; i++) {
        char* s = lines[i];
        if (*s == '!' || *s == '#')
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

        uint64_t key_hash = hash_xxh3_64bits(key_str_data, key_length);

        key.push_back({ key_str_data, key_length });
        val.push_back({ val_str_data, val_length });
        j++;
    }

    scope.resize(1);
    curr_scope = &scope[0];
    curr_scope->key.clear();
    curr_scope->index = 0;
    curr_scope->count = key.size();

    key_val_sort(this);
    free_def(lines);
}

bool key_val::read(bool& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = false;
        return false;
    }
    value = atoi(val[index].first) ? true : false;
    return true;
}

bool key_val::read(float_t& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0.0f;
        return false;
    }
    value = (float_t)atof(val[index].first);
    return true;
}

bool key_val::read(int32_t& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0;
        return false;
    }
    value = atoi(val[index].first);
    return true;
}

bool key_val::read(uint32_t& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0;
        return false;
    }
    value = (uint32_t)atoll(val[index].first);
    return true;
}

bool key_val::read(const char*& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value = 0;
        return false;
    }
    value = val[index].first;
    return true;
}

bool key_val::read(std::string& value) {
    int64_t index = curr_scope->index;
    if (index == -1) {
        value.clear();
        return false;
    }
    value.assign(val[index].first, val[index].second);
    return true;
}

bool key_val::read(vec3& value) {
    read("x", value.x);
    read("y", value.y);
    read("z", value.z);
    return true;
}

bool key_val::read(const char* key, bool& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = false;
        return false;
    }
    value = atoi(val[index].first) ? true : false;
    return true;
}

bool key_val::read(std::string& key, bool& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = false;
        return false;
    }
    value = atoi(val[index].first) ? true : false;
    return true;
}

bool key_val::read(const char* key, float_t& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0.0f;
        return false;
    }
    value = (float_t)atof(val[index].first);
    return true;
}

bool key_val::read(std::string& key, float_t& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0.0f;
        return false;
    }
    value = (float_t)atof(val[index].first);
    return true;
}

bool key_val::read(const char* key, int32_t& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = atoi(val[index].first);
    return true;
}

bool key_val::read(std::string& key, int32_t& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = atoi(val[index].first);
    return true;
}

bool key_val::read(const char* key, uint32_t& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = (uint32_t)atoll(val[index].first);
    return true;
}

bool key_val::read(std::string& key, uint32_t& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = (uint32_t)atoll(val[index].first);
    return true;
}

bool key_val::read(const char* key, const char*& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = val[index].first;
    return true;
}

bool key_val::read(std::string& key, const char*& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value = 0;
        return false;
    }
    value = val[index].first;
    return true;
}

bool key_val::read(const char* key, std::string& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value.clear();
        return false;
    }
    value.assign(val[index].first, val[index].second);
    return true;
}

bool key_val::read(std::string& key, std::string& value) {
    int64_t index = key_val_get_key_index(this, key);
    if (index == -1) {
        value.clear();
        return false;
    }
    value.assign(val[index].first, val[index].second);
    return true;
}

bool key_val::read(const char* key, vec3& value) {
    if (open_scope(key)) {
        read("x", value.x);
        read("y", value.y);
        read("z", value.z);
        close_scope();
        return true;
    }
    value = 0.0f;
    return false;
}

bool key_val::read(std::string& key, vec3& value) {
    if (open_scope(key)) {
        read("x", value.x);
        read("y", value.y);
        read("z", value.z);
        close_scope();
        return true;
    }
    value = 0.0f;
    return false;
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

bool key_val::read(const char* key0, const char* key1, bool& value) {
    if (!open_scope(key0)) {
        value = false;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
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

bool key_val::read(const char* key0, const char* key1, float_t& value) {
    if (!open_scope(key0)) {
        value = 0.0f;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
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

bool key_val::read(const char* key0, const char* key1, int32_t& value) {
    if (!open_scope(key0)) {
        value = 0;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
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

bool key_val::read(const char* key0, const char* key1, uint32_t& value) {
    if (!open_scope(key0)) {
        value = 0;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string& key0, std::string& key1, const char*& value) {
    if (!open_scope(key0)) {
        value = 0;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(const char* key0, const char* key1, const char*& value) {
    if (!open_scope(key0)) {
        value = 0;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string& key0, std::string& key1, std::string& value) {
    if (!open_scope(key0)) {
        value.clear();
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(const char* key0, const char* key1, std::string& value) {
    if (!open_scope(key0)) {
        value.clear();
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(std::string& key0, std::string& key1, vec3& value) {
    if (!open_scope(key0)) {
        value = 0.0f;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::read(const char* key0, const char* key1, vec3& value) {
    if (!open_scope(key0)) {
        value = 0.0f;
        return false;
    }

    if (read(key1, value))
        return true;

    close_scope();
    return false;
}

bool key_val::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    std::string path(dir);
    path.append(file);

    key_val* kv = (key_val*)data;
    kv->file_read(path.c_str());

    return kv->buf ? true : false;
}

key_val_out::key_val_out() : curr_scope() {
    scope.push_back("");
    curr_scope = &scope[scope.size() - 1];
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

    if (curr_scope->size()) {
        temp_key.clear();
        temp_key.reserve(curr_scope->size() + str.size() + 1);
        temp_key.assign(*curr_scope);
        temp_key.push_back('.');
        temp_key.append(str);
        scope.push_back(temp_key);
    }
    else
        scope.push_back(str);
    curr_scope = &scope[scope.size() - 1];
}

void key_val_out::open_scope(const char* str) {
    if (!str || !*str) {
        scope.push_back(*curr_scope);
        curr_scope = &scope[scope.size() - 1];
        return;
    }

    if (curr_scope->size()) {
        size_t str_len = utf8_length(str);
        temp_key.clear();
        temp_key.reserve(curr_scope->size() + str_len + 1);
        temp_key.assign(*curr_scope);
        temp_key.push_back('.');
        temp_key.append(str, str_len);
        scope.push_back(temp_key);
    }
    else
        scope.push_back(str);
    curr_scope = &scope[scope.size() - 1];
}

void key_val_out::open_scope_fmt(int32_t i) {
    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "%d", i);
    open_scope(buf);
}

void key_val_out::open_scope_fmt(uint32_t i) {
    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "%u", i);
    open_scope(buf);
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

void key_val_out::write(stream& s, const char* key, bool value) {
    if (!value)
        return;

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_utf8_string(key);
    s.write("=1\n", 3);
}

void key_val_out::write(stream& s, std::string& key, bool value) {
    if (!value)
        return;

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_string(key);
    s.write("=1\n", 3);
}

void key_val_out::write(stream& s, const char* key, float_t value, const char* fmt) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, fmt, value);

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_utf8_string(key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string& key, float_t value, const char* fmt) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, fmt, value);

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_string(key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, const char* key, int32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%d", value);

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_utf8_string(key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string& key, int32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%d", value);

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_string(key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, const char* key, uint32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%u", value);

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_utf8_string(key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string& key, uint32_t value) {
    char val_buf[0x100];
    sprintf_s(val_buf, 0x100, "%u", value);

    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_string(key);
    s.write_char('=');
    s.write_utf8_string(val_buf);
    s.write_char('\n');
}

void key_val_out::write(stream& s, const char* key, const char* value) {
    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_utf8_string(key);
    s.write_char('=');
    s.write_utf8_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string& key, const char* value) {
    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_string(key);
    s.write_char('=');
    s.write_utf8_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, const char* key, std::string& value) {
    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_utf8_string(key);
    s.write_char('=');
    s.write_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, std::string& key, std::string& value) {
    if (curr_scope->size()) {
        s.write_string(*curr_scope);
        s.write_char('.');
    }
    s.write_string(key);
    s.write_char('=');
    s.write_string(value);
    s.write_char('\n');
}

void key_val_out::write(stream& s, const char* key, vec3& value) {
    open_scope(key);
    write(s, "x", value.x);
    write(s, "y", value.y);
    write(s, "z", value.z);
    close_scope();
}

void key_val_out::write(stream& s, std::string& key, vec3& value) {
    open_scope(key);
    write(s, "x", value.x);
    write(s, "y", value.y);
    write(s, "z", value.z);
    close_scope();
}

void key_val_out::write(stream& s, const char* key, vec3&& value) {
    write(s, key, *(vec3*)&value);
}

void key_val_out::write(stream& s, std::string& key, vec3&& value) {
    write(s, key, *(vec3*)&value);
}

void key_val_out::get_lexicographic_order(std::vector<int32_t>& vec, int32_t length) {
    vec.clear();

    int32_t i, j, m;

    if (length < 1)
        return;

    vec.reserve(length);

    j = 0;
    vec.push_back(j);
    i = 1;
    j = 1;

    m = 1;
    while (m < length)
        m *= 10;

    while (i < length) {
        if (j * 10 < m) {
            vec.push_back(j);
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
            vec.push_back(j);
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
                vec.push_back(j);
                i++;
                while (j % 10 == 9)
                    j /= 10;
                j++;
            }
    }
}

static int64_t key_val_find_first_key(key_val* kv, int64_t low, int64_t high,
    const char* s, size_t s_len, size_t offset) {
    auto* k = kv->key.data();

    int64_t first_idx = -1;
    int64_t mid;
    int32_t res;
    bool full;
    while (low <= high) {
        auto* l = &k[mid = (low + high) / 2];
        if (l->second > offset && s_len <= l->second - offset) {
            res = memcmp(s, l->first + offset, s_len);
            if (!res && l->first[offset + s_len] && l->first[offset + s_len] != '.')
                res = -1;
            full = true;
        }
        else {
            if (l->second >= offset) {
                res = str_utils_compare_length(s, s_len, l->first + offset, l->second - offset);
                if (!res && s[l->second - offset])
                    res = 1;
            }
            else
                res = 1;
            full = false;
        }

        if (res > 0)
            low = mid + 1;
        else if (res < 0)
            high = mid - 1;
        else {
            if (full)
                first_idx = mid;
            high = mid - 1;
        }
    }
    return first_idx;
}

static int64_t key_val_find_last_key(key_val* kv, int64_t low, int64_t high,
    const char* s, size_t s_len, size_t offset) {
    auto* k = kv->key.data();

    int64_t last_idx = -1;
    int64_t mid;
    int32_t res;
    bool full;
    while (low <= high) {
        auto* l = &k[mid = (low + high) / 2];
        if (l->second > offset && s_len <= l->second - offset) {
            res = memcmp(s, l->first + offset, s_len);
            if (!res && l->first[offset + s_len] && l->first[offset + s_len] != '.')
                res = -1;
            full = true;
        }
        else {
            if (l->second >= offset) {
                res = str_utils_compare_length(s, s_len, l->first + offset, l->second - offset);
                if (!res && s[l->second - offset])
                    res = 1;
            }
            else
                res = 1;
            full = false;
        }

        if (res > 0)
            low = mid + 1;
        else if (res < 0)
            high = mid - 1;
        else {
            if (full)
                last_idx = mid;
            low = mid + 1;
        }
    }
    return last_idx;
}

static int64_t key_val_get_key_index(key_val* kv, const char* key) {
    size_t str_len = utf8_length(key);
    if (kv->curr_scope->key.size()) {
        kv->temp_key.clear();
        kv->temp_key.reserve(kv->curr_scope->key.size() + str_len + 1);
        kv->temp_key.assign(kv->curr_scope->key);
        kv->temp_key.push_back('.');
        kv->temp_key.append(key, str_len);
        return key_val_get_key_index(kv, kv->temp_key.c_str(), kv->temp_key.size());
    }
    else
        return key_val_get_key_index(kv, key, str_len);
}

static int64_t key_val_get_key_index(key_val* kv, std::string& key) {
    if (kv->curr_scope->key.size()) {
        kv->temp_key.clear();
        kv->temp_key.reserve(kv->curr_scope->key.size() + key.size() + 1);
        kv->temp_key.assign(kv->curr_scope->key);
        kv->temp_key.push_back('.');
        kv->temp_key.append(key);
        return key_val_get_key_index(kv, kv->temp_key.c_str(), kv->temp_key.size());
    }
    else
        return key_val_get_key_index(kv, key.c_str(), key.size());
}

static int64_t key_val_get_key_index(key_val* kv, const char* str, size_t size) {
    key_val_scope* curr_scope = kv->curr_scope;

    if (curr_scope->key_hash_index.size() < 1)
        return -1;

    uint64_t hash = hash_xxh3_64bits(str, size);

    auto* key = curr_scope->key_hash_index.data();
    size_t len = curr_scope->count;
    size_t temp;
    while (len > 0) {
        if (hash < key[temp = len / 2].first)
            len = temp;
        else {
            key += temp + 1;
            len -= temp + 1;
        }
    }

    if (key == curr_scope->key_hash_index.data())
        if (key[0].first == hash)
            return key[0].second;
        else
            return -1;
    else if (key[-1].first == hash)
        return key[-1].second;
    else if (key == curr_scope->key_hash_index.data() + curr_scope->key_hash_index.size())
        return -1;
    else if (key[0].first == hash)
        return key[0].second;
    return -1;
}

#define RADIX_BASE 4
#define RADIX (1 << RADIX_BASE)

static void key_val_sort(key_val* kv) {
    if (!kv->key.size())
        return;

    key_val_scope* curr_scope = kv->curr_scope;

    size_t index = curr_scope->index;
    size_t count = curr_scope->count;

    curr_scope->key_hash_index.clear();
    curr_scope->key_hash_index.resize(count);

    auto* key = kv->key.data() + index;
    auto* key_hash_index = curr_scope->key_hash_index.data();
    for (size_t i = 0; i < count; i++) {
        key_hash_index->second = index + i;
        key_hash_index->first = hash_xxh3_64bits(key->first, key->second);
        key++;
        key_hash_index++;
    }

    if (count < 2)
        return;

    size_t c[RADIX];
    kv->temp_key_hash_index.clear();
    kv->temp_key_hash_index.reserve(count);
    auto* o_key_hash_index = kv->temp_key_hash_index.data();
    auto* arr_key_hash_index = curr_scope->key_hash_index.data();
    auto* org_arr = arr_key_hash_index;

    for (size_t shift = 0, s = 0; shift < sizeof(uint64_t) * 8 / RADIX_BASE; shift++, s += RADIX_BASE) {
        memset(c, 0, sizeof(size_t) * RADIX);

        for (size_t i = 0; i < count; i++)
            c[(arr_key_hash_index[i].first >> s) & (RADIX - 1)]++;

        for (size_t i = 1; i < RADIX; i++)
            c[i] += c[i - 1];

        for (int64_t i = count - 1; i >= 0; i--) {
            size_t index = --c[(arr_key_hash_index[i].first >> s) & (RADIX - 1)];
            o_key_hash_index[index] = arr_key_hash_index[i];
        }

        auto* t_key_hash = arr_key_hash_index;
        arr_key_hash_index = o_key_hash_index;
        o_key_hash_index = t_key_hash;
    }

    if (org_arr == o_key_hash_index) {
        auto* t_key_hash_index = arr_key_hash_index;
        arr_key_hash_index = o_key_hash_index;
        o_key_hash_index = t_key_hash_index;

        memmove(arr_key_hash_index, o_key_hash_index, sizeof(*o_key_hash_index) * count);
    }
}
