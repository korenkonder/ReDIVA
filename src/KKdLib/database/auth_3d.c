/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../key_val.h"
#include "../sort.h"
#include "../str_utils.h"

#define AUTH_3D_DATABASE_TEXT_BUF_SIZE 0x400

static void auth_3d_database_load_categories(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata);
static void auth_3d_database_load_uids(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata);

static void auth_3d_database_file_read_inner(auth_3d_database_file* auth_3d_db_file, stream* s);
static void auth_3d_database_file_write_inner(auth_3d_database_file* auth_3d_db_file, stream* s);
static void auth_3d_database_file_read_text(auth_3d_database_file* auth_3d_db_file, void* data, size_t length);
static void auth_3d_database_file_write_text(auth_3d_database_file* auth_3d_db_file, void** data, size_t* length);

auth_3d_database_file::auth_3d_database_file() : ready(), uid_max(-1) {

}

auth_3d_database_file::~auth_3d_database_file() {

}

void auth_3d_database_file::read(const char* path) {
    if (!path)
        return;

    char* path_bin = str_utils_add(path, ".bin");
    if (path_check_file_exists(path_bin)) {
        stream s;
        io_open(&s, path_bin, "rb");
        if (s.io.stream)
            auth_3d_database_file_read_inner(this, &s);
        io_free(&s);
    }
    free(path_bin);
}

void auth_3d_database_file::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_bin = str_utils_add(path, L".bin");
    if (path_check_file_exists(path_bin)) {
        stream s;
        io_open(&s, path_bin, L"rb");
        if (s.io.stream)
            auth_3d_database_file_read_inner(this, &s);
        io_free(&s);
    }
    free(path_bin);
}

void auth_3d_database_file::read(const void* data, size_t length) {
    if (!data || !length)
        return;

    stream s;
    io_open(&s, data, length);
    auth_3d_database_file_read_inner(this, &s);
    io_free(&s);
}

void auth_3d_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_bin = str_utils_add(path, ".bin");
    stream s;
    io_open(&s, path_bin, "wb");
    if (s.io.stream)
        auth_3d_database_file_write_inner(this, &s);
    io_free(&s);
    free(path_bin);
}

void auth_3d_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_bin = str_utils_add(path, L".bin");
    stream s;
    io_open(&s, path_bin, L"wb");
    if (s.io.stream)
        auth_3d_database_file_write_inner(this, &s);
    io_free(&s);
    free(path_bin);
}

void auth_3d_database_file::write(void** data, size_t* length) {
    if (!data || !length || !ready)
        return;

    stream s;
    io_open(&s);
    auth_3d_database_file_write_inner(this, &s);
    io_copy(&s, data, length);
    io_free(&s);
}

bool auth_3d_database_file::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    auth_3d_database_file* auth_3d_db = (auth_3d_database_file*)data;
    auth_3d_db->read(string_data(&s));

    string_free(&s);
    return auth_3d_db->ready;
}

auth_3d_database::auth_3d_database() : ready() {

}

auth_3d_database::~auth_3d_database() {

}

void auth_3d_database::merge_mdata(auth_3d_database_file* base_auth_3d_db,
    auth_3d_database_file* mdata_auth_3d_db) {
    if (!base_auth_3d_db || !mdata_auth_3d_db
        || !base_auth_3d_db->ready || !mdata_auth_3d_db->ready)
        return;

    auth_3d_database_load_categories(this, base_auth_3d_db, false);
    auth_3d_database_load_uids(this, base_auth_3d_db, false);

    auth_3d_database_load_categories(this, mdata_auth_3d_db, true);
    auth_3d_database_load_uids(this, mdata_auth_3d_db, true);

    ready = true;
}

void auth_3d_database::split_mdata(auth_3d_database_file* base_auth_3d_db,
    auth_3d_database_file* mdata_auth_3d_db) {
    if (!base_auth_3d_db || !mdata_auth_3d_db
        || !ready || !base_auth_3d_db->ready)
        return;

}

auth_3d_database_uid::auth_3d_database_uid() : enabled(), org_uid(), size() {

}

auth_3d_database_uid::~auth_3d_database_uid() {

}

auth_3d_database_uid_file::auth_3d_database_uid_file() : flags(), org_uid(), size() {

}

auth_3d_database_uid_file::~auth_3d_database_uid_file() {

}

auth_3d_database_category::auth_3d_database_category() {

}

auth_3d_database_category::~auth_3d_database_category() {

}

static void auth_3d_database_load_categories(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata) {
    std::vector<auth_3d_database_category>& category = auth_3d_db->category;
    std::vector<std::string>& category_file = auth_3d_db_file->category;
    int32_t count_file = (int32_t)category_file.size();

    if (!mdata)
        category.reserve(count_file);

    for (int32_t i = 0; i < count_file; i++) {
        bool found = false;
        for (auth_3d_database_category& j : category)
            if (category_file[i] == j.name) {
                found = true;
                break;
            }

        if (found)
            continue;

        auth_3d_database_category cat;
        cat.name = category_file[i];
        category.push_back(cat);
    }
}

static void auth_3d_database_load_uids(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata) {
    size_t uid_file_count = auth_3d_db_file->uid.size();
    size_t uid_count = auth_3d_db->uid.size();

    bool reserve = true;
    size_t uid_resize = 0;
    if (!mdata)
        uid_resize = uid_file_count;
    else if (auth_3d_db_file->uid_max < 0)
        uid_resize = uid_file_count + uid_count;
    else if (auth_3d_db_file->uid_max >= uid_count)
        uid_resize = auth_3d_db_file->uid_max + 1ULL;
    else
        reserve = false;

    if (reserve)
        auth_3d_db->uid.resize(uid_resize);

    std::vector<auth_3d_database_uid>& uids = auth_3d_db->uid;
    std::vector<auth_3d_database_uid_file>& uids_file = auth_3d_db_file->uid;
    for (size_t i = 0; i < uid_file_count; i++) {
        auth_3d_database_uid_file* uid_file = &uids_file[i];
        int32_t org_uid;
        if (!mdata)
            org_uid = (int32_t)i;
        else if (uid_file->flags & AUTH_3D_DATABASE_UID_ORG_UID)
            org_uid = uid_file->org_uid;
        else
            org_uid = (int32_t)(uid_count + i);

        auth_3d_database_uid* uid = &uids[org_uid];
        uid->org_uid = org_uid;
        uid->enabled = false;

        if (uid_file->value.size() >= 3) {
            if (uid_file->value.c_str()[0] == 'A')
                uid->enabled = true;

            uid->name = std::string(uid_file->value.c_str() + 2, uid_file->value.size() - 2);
            uid->category = uid_file->category;
        }

        if (uid->enabled) {
            uid_file->category = uid->category;

            if (uid->category.size() > 0) {
                for (auth_3d_database_category& j : auth_3d_db->category) {
                    if (uid->category != j.name)
                        continue;

                    bool found = false;
                    for (int32_t& k : j.uid)
                        if (k == org_uid) {
                            found = true;
                            break;
                        }

                    if (!found)
                        j.uid.push_back(org_uid);
                }
            }
        }

        if (uid_file->flags & AUTH_3D_DATABASE_UID_SIZE)
            uid->size = uid_file->size;
        else
            uid->size = -1.0f;
    }
}

static void auth_3d_database_file_read_inner(auth_3d_database_file* auth_3d_db_file, stream* s) {
    uint32_t signature = io_read_uint32_t(s);
    if (signature != reverse_endianness_int32_t('#A3D'))
        return;

    signature = io_read_uint32_t(s);
    if ((signature & 0xFF) != 'A')
        return;
    
    size_t string_offset = 0x10;
    size_t string_length = (int32_t)(s->length - 0x10);

    io_set_position(s, string_offset, SEEK_SET);
    void* a3da_data = force_malloc(string_length);
    io_read(s, a3da_data, string_length);
    auth_3d_database_file_read_text(auth_3d_db_file, a3da_data, string_length);
    free(a3da_data);

    auth_3d_db_file->ready = true;
}

static void auth_3d_database_file_write_inner(auth_3d_database_file* auth_3d_db_file, stream* s) {
    void* a3da_data = 0;
    size_t a3da_data_length = 0;
    auth_3d_database_file_write_text(auth_3d_db_file, &a3da_data, &a3da_data_length);
    io_write(s, a3da_data, a3da_data_length);
    free(a3da_data);
}

static void auth_3d_database_file_read_text(auth_3d_database_file* auth_3d_db_file, void* data, size_t length) {
    char buf[AUTH_3D_DATABASE_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t off;

    key_val kv;
    kv.parse((uint8_t*)data, length);
    key_val lkv;

    len = 8;
    memcpy(buf, "category", 8);
    off = len;

    auth_3d_db_file->category.clear();
    if (kv.read_int32_t(buf, off, ".length", 8, &count)
        && kv.get_local_key_val("category", &lkv)) {
        std::vector<std::string>& vc = auth_3d_db_file->category;

        vc.resize(count);
        for (int32_t i = 0; i < count; i++) {
            std::string* c = &vc[i];
            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            lkv.read_string(
                buf, off, ".value", 7, c);
        }
    }

    len = 3;
    memcpy(buf, "uid", 3);
    off = len;

    auth_3d_db_file->uid.clear();
    if (!kv.read_int32_t(buf, off, ".max", 5, &auth_3d_db_file->uid_max))
        auth_3d_db_file->uid_max = -1;

    if (kv.read_int32_t(buf, off, ".length", 8, &count)
        && kv.get_local_key_val("uid", &lkv)) {
        std::vector<auth_3d_database_uid_file>& vu = auth_3d_db_file->uid;

        vu.resize(count);
        for (int32_t i = 0; i < count; i++) {
            auth_3d_database_uid_file* u = &vu[i];
            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            lkv.read_string(
                buf, off, ".category", 10, &u->category);
            if (lkv.read_int32_t(
                buf, off, ".org_uid", 9, &u->org_uid))
                enum_or(u->flags, AUTH_3D_DATABASE_UID_ORG_UID);
            if (lkv.read_float_t(
                buf, off, ".size", 6, &u->size))
                enum_or(u->flags, AUTH_3D_DATABASE_UID_SIZE);
            lkv.read_string(
                buf, off, ".value", 7, &u->value);
        }
    }
}

static void auth_3d_database_file_write_text(auth_3d_database_file* auth_3d_db_file, void** data, size_t* length) {
    char buf[AUTH_3D_DATABASE_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t off;

    stream s;
    io_open(&s);
    
    io_write(&s, "#A3DA__________\n", 16);
    io_write(&s, "# date time was eliminated.\n", 28);

    if (auth_3d_db_file->category.size() > 0) {
        len = 8;
        memcpy(buf, "category", 8);
        off = len;

        std::vector<std::string>& vc = auth_3d_db_file->category;
        count = (int32_t)vc.size();
        std::vector<int32_t> sort_index;
        key_val::get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            std::string& c = vc[sort_index[i]];

            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", sort_index[i]);
            off = len + len1;

            key_val::write_string(&s, buf, off, ".value", 7, c);
        }

        off = len;
        key_val::write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (auth_3d_db_file->uid.size() > 0) {
        len = 3;
        memcpy(buf, "uid", 3);
        off = len;

        int32_t uid_max = -1;
        std::vector<auth_3d_database_uid_file>& vu = auth_3d_db_file->uid;
        count = (int32_t)vu.size();
        std::vector<int32_t> sort_index;
        key_val::get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            auth_3d_database_uid_file* u = &vu[sort_index[i]];

            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", sort_index[i]);
            off = len + len1;

            key_val::write_string(&s, buf, off, ".category", 10, u->category);
            if ((int32_t)(u->flags & AUTH_3D_DATABASE_UID_ORG_UID))
                key_val::write_int32_t(&s, buf, off, ".org_uid", 9, u->org_uid);
            if ((int32_t)(u->flags & AUTH_3D_DATABASE_UID_SIZE))
                key_val::write_float_t(&s, buf, off, ".size", 6, u->size);
            key_val::write_string(&s, buf, off, ".value", 7, u->value);

            if ((int32_t)(u->flags & AUTH_3D_DATABASE_UID_ORG_UID) && u->org_uid > uid_max)
                uid_max = u->org_uid;
        }

        if (uid_max == 0)
            uid_max = count;

        off = len;
        key_val::write_int32_t(&s, buf, off, ".length", 8, count);
        key_val::write_int32_t(&s, buf, off, ".max", 5, uid_max);
    }

    io_copy(&s, data, length);
    io_free(&s);
}
