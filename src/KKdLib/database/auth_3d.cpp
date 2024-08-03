/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../prj/algorithm.hpp"
#include "../key_val.hpp"
#include "../hash.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void auth_3d_database_load_categories(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata);
static void auth_3d_database_load_uids(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata);

static void auth_3d_database_file_read_inner(auth_3d_database_file* auth_3d_db_file, stream& s);
static void auth_3d_database_file_write_inner(auth_3d_database_file* auth_3d_db_file, stream& s);
static void auth_3d_database_file_read_text(auth_3d_database_file* auth_3d_db_file, void* data, size_t size);
static void auth_3d_database_file_write_text(auth_3d_database_file* auth_3d_db_file, void** data, size_t* size);

auth_3d_database_uid::auth_3d_database_uid() : enabled(), org_uid(), size() {
    category_hash = hash_murmurhash_empty;
    name_hash = hash_murmurhash_empty;
}

auth_3d_database_uid::~auth_3d_database_uid() {

}

auth_3d_database_uid_file::auth_3d_database_uid_file() : flags(), org_uid(), size() {

}

auth_3d_database_uid_file::~auth_3d_database_uid_file() {

}

auth_3d_database_category::auth_3d_database_category() {
    name_hash = hash_murmurhash_empty;
}

auth_3d_database_category::~auth_3d_database_category() {

}

auth_3d_database_file::auth_3d_database_file() : ready(), uid_max(-1) {

}

auth_3d_database_file::~auth_3d_database_file() {

}

void auth_3d_database_file::read(const char* path) {
    if (!path)
        return;

    char* path_bin = str_utils_add(path, ".bin");
    if (path_check_file_exists(path_bin)) {
        file_stream s;
        s.open(path_bin, "rb");
        if (s.check_not_null())
            auth_3d_database_file_read_inner(this, s);
    }
    free_def(path_bin);
}

void auth_3d_database_file::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_bin = str_utils_add(path, L".bin");
    if (path_check_file_exists(path_bin)) {
        file_stream s;
        s.open(path_bin, L"rb");
        if (s.check_not_null())
            auth_3d_database_file_read_inner(this, s);
    }
    free_def(path_bin);
}

void auth_3d_database_file::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    auth_3d_database_file_read_inner(this, s);
}

void auth_3d_database_file::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_bin = str_utils_add(path, ".bin");
    file_stream s;
    s.open(path_bin, "wb");
    if (s.check_not_null())
        auth_3d_database_file_write_inner(this, s);
    free_def(path_bin);
}

void auth_3d_database_file::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_bin = str_utils_add(path, L".bin");
    file_stream s;
    s.open(path_bin, L"wb");
    if (s.check_not_null())
        auth_3d_database_file_write_inner(this, s);
    free_def(path_bin);
}

void auth_3d_database_file::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    auth_3d_database_file_write_inner(this, s);
    s.copy(data, size);
}

bool auth_3d_database_file::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    auth_3d_database_file* auth_3d_db = (auth_3d_database_file*)data;
    auth_3d_db->read(path.c_str());

    return auth_3d_db->ready;
}

auth_3d_database::auth_3d_database() {

}

auth_3d_database::~auth_3d_database() {

}

void auth_3d_database::add(auth_3d_database_file* auth_3d_db_file, bool mdata) {
    if (!auth_3d_db_file || !auth_3d_db_file->ready )
        return;

    auth_3d_database_load_categories(this, auth_3d_db_file, mdata);
    auth_3d_database_load_uids(this, auth_3d_db_file, mdata);

    update();
}

void auth_3d_database::clear() {
    category.clear();
    category.shrink_to_fit();
    uid.clear();
    uid.shrink_to_fit();
    category_names.clear();
    category_names.shrink_to_fit();
    uid_names.clear();
    uid_names.shrink_to_fit();
}

void auth_3d_database::update() {
    category_names.clear();
    uid_names.clear();

    category_names.reserve(category.size());
    uid_names.reserve(uid.size());

    for (auth_3d_database_category& i : category)
        category_names.push_back(i.name_hash, &i);

    for (auth_3d_database_uid& i : uid)
        uid_names.push_back(i.name_hash, &i);

    category_names.sort_unique();
    uid_names.sort_unique();
}

int32_t auth_3d_database::get_category_index(const char* name) const {
    auto elem = category_names.find(hash_utf8_murmurhash(name));
    if (elem != category_names.end())
        return (int32_t)(elem->second - category.data());
    return -1;
}

void auth_3d_database::get_category_uids(const char* name, std::vector<int32_t>& uid) const {
    uid.clear();

    auto elem = category_names.find(hash_utf8_murmurhash(name));
    if (elem != category_names.end()) {
        uid.assign(elem->second->uid.begin(), elem->second->uid.end());
        return;
    }
}

int32_t auth_3d_database::get_uid(const char* name) const {
    auto elem = uid_names.find(hash_utf8_murmurhash(name));
    if (elem != uid_names.end())
        return (int32_t)(elem->second - uid.data());
    return -1;
}

static void auth_3d_database_load_categories(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata) {
    std::vector<auth_3d_database_category>& category = auth_3d_db->category;
    std::vector<std::string>& category_file = auth_3d_db_file->category;
    int32_t count_file = (int32_t)category_file.size();

    if (!mdata)
        category.reserve(count_file);

    for (int32_t i = 0; i < count_file; i++) {
        std::string& cat_file = category_file[i];

        auth_3d_database_category* cat = 0;
        for (auth_3d_database_category& j : category)
            if (!cat_file.compare(j.name)) {
                cat = &j;
                break;
            }

        if (!cat) {
            category.push_back({});
            cat = &category.back();
        }

        cat->name = category_file[i];
        cat->name_hash = hash_string_murmurhash(cat->name);
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

    std::vector<auth_3d_database_category>& category = auth_3d_db->category;
    auth_3d_database_uid* uids = auth_3d_db->uid.data();
    auth_3d_database_uid_file* uids_file = auth_3d_db_file->uid.data();
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

            uid->name.assign(uid_file->value.substr(2, uid_file->value.size() - 2));
            uid->name_hash = hash_string_murmurhash(uid->name);
            uid->category.assign(uid_file->category);
            uid->category_hash = hash_string_murmurhash(uid->category);
        }

        if (uid->enabled) {
            uid_file->category.assign(uid->category);

            if (uid->category.size() > 0)
                for (auth_3d_database_category& j : category)
                    if (!uid->category.compare(j.name)) {
                        j.uid.push_back(org_uid);
                        break;
                    }
        }

        if (uid_file->flags & AUTH_3D_DATABASE_UID_SIZE)
            uid->size = uid_file->size;
        else
            uid->size = -1.0f;
    }

    for (auth_3d_database_category& i : category)
        prj::sort_unique(i.uid);
}

static void auth_3d_database_file_read_inner(auth_3d_database_file* auth_3d_db_file, stream& s) {
    uint32_t signature = s.read_uint32_t();
    if (signature != reverse_endianness_int32_t('#A3D'))
        return;

    signature = s.read_uint32_t();
    if ((signature & 0xFF) != 'A')
        return;

    size_t string_offset = 0x10;
    size_t string_length = (int32_t)(s.length - 0x10);

    s.set_position(string_offset, SEEK_SET);
    void* a3da_data = force_malloc(string_length);
    s.read(a3da_data, string_length);
    auth_3d_database_file_read_text(auth_3d_db_file, a3da_data, string_length);
    free_def(a3da_data);

    auth_3d_db_file->ready = true;
}

static void auth_3d_database_file_write_inner(auth_3d_database_file* auth_3d_db_file, stream& s) {
    void* a3da_data = 0;
    size_t a3da_data_length = 0;
    auth_3d_database_file_write_text(auth_3d_db_file, &a3da_data, &a3da_data_length);
    s.write(a3da_data, a3da_data_length);
    free_def(a3da_data);
}

static void auth_3d_database_file_read_text(auth_3d_database_file* auth_3d_db_file, void* data, size_t size) {
    key_val kv;
    kv.parse(data, size);

    auth_3d_db_file->category.clear();
    int32_t count;
    if (kv.read("category", "length", count)) {
        auth_3d_db_file->category.resize(count);
        std::string* vc = auth_3d_db_file->category.data();
        for (int32_t i = 0; i < count; i++) {
            if (!kv.open_scope_fmt(i))
                continue;

            kv.read("value", vc[i]);
            kv.close_scope();
        }
        kv.close_scope();
    }

    if (kv.open_scope("uid")) {
        auth_3d_db_file->uid.clear();
        if (!kv.read("max", auth_3d_db_file->uid_max))
            auth_3d_db_file->uid_max = -1;

        int32_t count;
        if (kv.read("length", count)) {
            auth_3d_db_file->uid.resize(count);
            auth_3d_database_uid_file* vu = auth_3d_db_file->uid.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                auth_3d_database_uid_file* u = &vu[i];
                kv.read("category", u->category);
                if (kv.read("org_uid", u->org_uid))
                    enum_or(u->flags, AUTH_3D_DATABASE_UID_ORG_UID);
                if (kv.read("size", u->size))
                    enum_or(u->flags, AUTH_3D_DATABASE_UID_SIZE);
                kv.read("value", u->value);

                kv.close_scope();
            }
        }
        kv.close_scope();
    }
}

static void auth_3d_database_file_write_text(auth_3d_database_file* auth_3d_db_file, void** data, size_t* size) {
    memory_stream s;
    s.open();

    s.write("#A3DA__________\n", 16);
    s.write("# date time was eliminated.\n", 28);

    key_val_out kv;
    if (auth_3d_db_file->category.size() > 0) {
        kv.open_scope("category");

        int32_t count = (int32_t)auth_3d_db_file->category.size();
        std::string* vc = auth_3d_db_file->category.data();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, count);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < count; i++) {
            kv.open_scope_fmt(sort_index_data[i]);
            kv.write(s, "value", vc[sort_index_data[i]]);
            kv.close_scope();
        }

        kv.write(s, "length", count);
        kv.close_scope();
    }

    if (auth_3d_db_file->uid.size() > 0) {
        kv.open_scope("uid");

        int32_t uid_max = -1;
        int32_t count = (int32_t)auth_3d_db_file->uid.size();
        auth_3d_database_uid_file* vu = auth_3d_db_file->uid.data();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, count);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < count; i++) {
            kv.open_scope_fmt(sort_index_data[i]);

            auth_3d_database_uid_file* u = &vu[sort_index_data[i]];

            kv.write(s, "category", u->category);
            if ((int32_t)(u->flags & AUTH_3D_DATABASE_UID_ORG_UID))
                kv.write(s, "org_uid", u->org_uid);
            if ((int32_t)(u->flags & AUTH_3D_DATABASE_UID_SIZE))
                kv.write(s, "size", u->size);
            kv.write(s, "value", u->value);

            if ((int32_t)(u->flags & AUTH_3D_DATABASE_UID_ORG_UID) && u->org_uid > uid_max)
                uid_max = u->org_uid;

            kv.close_scope();
        }

        kv.write(s, "length", count);
        if (uid_max != -1)
            kv.write(s, "max", uid_max);
        kv.close_scope();
    }

    s.copy(data, size);
}
