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

vector_func(auth_3d_database_category)
vector_func(auth_3d_database_uid)
vector_func(auth_3d_database_uid_file)

#define AUTH_3D_DATABASE_TEXT_BUF_SIZE 0x400

static void auth_3d_database_load_categories(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata);
static void auth_3d_database_load_uids(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata);

static void auth_3d_database_file_read_inner(auth_3d_database_file* auth_3d_db_file, stream* s);
static void auth_3d_database_file_write_inner(auth_3d_database_file* auth_3d_db_file, stream* s);
static void auth_3d_database_file_read_text(auth_3d_database_file* auth_3d_db_file, void* data, size_t length);
static void auth_3d_database_file_write_text(auth_3d_database_file* auth_3d_db_file, void** data, size_t* length);

void auth_3d_database_init(auth_3d_database* auth_3d_db) {
    memset(auth_3d_db, 0, sizeof(auth_3d_database));
}

void auth_3d_database_merge_mdata(auth_3d_database* auth_3d_db,
    auth_3d_database_file* base_auth_3d_db, auth_3d_database_file* mdata_auth_3d_db) {
    if (!auth_3d_db || !base_auth_3d_db || !mdata_auth_3d_db
        || !base_auth_3d_db->ready || !mdata_auth_3d_db->ready)
        return;

    vector_auth_3d_database_category* category = &auth_3d_db->category;
    vector_string* base_category = &base_auth_3d_db->category;
    vector_string* mdata_category = &mdata_auth_3d_db->category;

    auth_3d_database_load_categories(auth_3d_db, base_auth_3d_db, false);
    auth_3d_database_load_uids(auth_3d_db, base_auth_3d_db, false);

    auth_3d_database_load_categories(auth_3d_db, mdata_auth_3d_db, true);
    auth_3d_database_load_uids(auth_3d_db, mdata_auth_3d_db, true);

    auth_3d_db->ready = true;
}

void auth_3d_database_split_mdata(auth_3d_database* auth_3d_db,
    auth_3d_database_file* base_auth_3d_db, auth_3d_database_file* mdata_auth_3d_db) {
    if (!auth_3d_db || !base_auth_3d_db || !mdata_auth_3d_db
        || !auth_3d_db->ready || !base_auth_3d_db->ready)
        return;

}

void auth_3d_database_free(auth_3d_database* auth_3d_db) {
    vector_auth_3d_database_category_free(&auth_3d_db->category, auth_3d_database_category_free);
    vector_auth_3d_database_uid_free(&auth_3d_db->uid, auth_3d_database_uid_free);
}

void auth_3d_database_file_init(auth_3d_database_file* auth_3d_db_file) {
    memset(auth_3d_db_file, 0, sizeof(auth_3d_database_file));
    auth_3d_db_file->uid_max = -1;
}

void auth_3d_database_file_read(auth_3d_database_file* auth_3d_db_file, char* path) {
    if (!auth_3d_db_file || !path)
        return;

    char* path_bin = str_utils_add(path, ".bin");
    if (path_check_file_exists(path_bin)) {
        stream s;
        io_open(&s, path_bin, "rb");
        if (s.io.stream)
            auth_3d_database_file_read_inner(auth_3d_db_file, &s);
        io_free(&s);
    }
    free(path_bin);
}

void auth_3d_database_file_wread(auth_3d_database_file* auth_3d_db_file, wchar_t* path) {
    if (!auth_3d_db_file || !path)
        return;

    wchar_t* path_bin = str_utils_wadd(path, L".bin");
    if (path_wcheck_file_exists(path_bin)) {
        stream s;
        io_wopen(&s, path_bin, L"rb");
        if (s.io.stream)
            auth_3d_database_file_read_inner(auth_3d_db_file, &s);
        io_free(&s);
    }
    free(path_bin);
}

void auth_3d_database_file_mread(auth_3d_database_file* auth_3d_db_file, void* data, size_t length) {
    if (!auth_3d_db_file || !data || !length)
        return;

    stream s;
    io_mopen(&s, data, length);
    auth_3d_database_file_read_inner(auth_3d_db_file, &s);
    io_free(&s);
}

void auth_3d_database_file_write(auth_3d_database_file* auth_3d_db_file, char* path) {
    if (!auth_3d_db_file || !path || !auth_3d_db_file->ready)
        return;

    char* path_bin = str_utils_add(path, ".bin");
    stream s;
    io_open(&s, path_bin, "wb");
    if (s.io.stream)
        auth_3d_database_file_write_inner(auth_3d_db_file, &s);
    io_free(&s);
    free(path_bin);
}

void auth_3d_database_file_wwrite(auth_3d_database_file* auth_3d_db_file, wchar_t* path) {
    if (!auth_3d_db_file || !path || !auth_3d_db_file->ready)
        return;

    wchar_t* path_bin = str_utils_wadd(path, L".bin");
    stream s;
    io_wopen(&s, path_bin, L"wb");
    if (s.io.stream)
        auth_3d_database_file_write_inner(auth_3d_db_file, &s);
    io_free(&s);
    free(path_bin);
}

void auth_3d_database_file_mwrite(auth_3d_database_file* auth_3d_db_file, void** data, size_t* length) {
    if (!auth_3d_db_file || !data || !auth_3d_db_file->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    auth_3d_database_file_write_inner(auth_3d_db_file, &s);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool auth_3d_database_file_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    auth_3d_database_file* auth_3d_db = data;
    auth_3d_database_file_read(auth_3d_db, string_data(&s));

    string_free(&s);
    return auth_3d_db->ready;
}

void auth_3d_database_file_free(auth_3d_database_file* auth_3d_db_file) {
    vector_string_free(&auth_3d_db_file->category, string_free);
    vector_auth_3d_database_uid_file_free(&auth_3d_db_file->uid, auth_3d_database_uid_file_free);
}

void auth_3d_database_category_free(auth_3d_database_category* cat) {
    string_free(&cat->name);
    vector_int32_t_free(&cat->uid, 0);
}

void auth_3d_database_uid_free(auth_3d_database_uid* uid) {
    string_free(&uid->category);
    string_free(&uid->name);
}

void auth_3d_database_uid_file_free(auth_3d_database_uid_file* uid) {
    string_free(&uid->category);
    string_free(&uid->value);
}

static void auth_3d_database_load_categories(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata) {
    vector_auth_3d_database_category* category = &auth_3d_db->category;
    vector_string* category_file = &auth_3d_db_file->category;
    int32_t count_file = (int32_t)(category_file->end - category_file->begin);

    if (!mdata)
        vector_auth_3d_database_category_reserve(category, count_file);

    for (int32_t i = 0; i < count_file; i++) {
        string* m_cat = &category_file->begin[i];

        char* str = string_data(m_cat);
        size_t len = m_cat->length;
        bool found = false;
        for (auth_3d_database_category* i = category->begin; i != category->end; i++)
            if (!memcmp(str, string_data(&i->name), min(len, i->name.length) + 1)) {
                found = true;
                break;
            }

        if (found)
            continue;

        auth_3d_database_category* cat = vector_auth_3d_database_category_reserve_back(category);
        string_copy(m_cat, &cat->name);
        cat->uid = vector_empty(int32_t);
    }
}

static void auth_3d_database_load_uids(auth_3d_database* auth_3d_db,
    auth_3d_database_file* auth_3d_db_file, bool mdata) {
    size_t uid_file_count = auth_3d_db_file->uid.end - auth_3d_db_file->uid.begin;
    size_t uid_count = auth_3d_db->uid.end - auth_3d_db->uid.begin;

    bool reserve = true;
    size_t uid_reserve = 0;
    if (!mdata)
        uid_reserve = uid_file_count;
    else if (auth_3d_db_file->uid_max == -1)
        uid_reserve = uid_count;
    else if (auth_3d_db_file->uid_max >= uid_count)
        uid_reserve = auth_3d_db_file->uid_max + 1ULL - uid_count;
    else
        reserve = false;

    if (reserve) {
        vector_auth_3d_database_uid_reserve(&auth_3d_db->uid, uid_reserve);
        auth_3d_db->uid.end += uid_reserve;
    }

    vector_auth_3d_database_uid* uids = &auth_3d_db->uid;
    vector_auth_3d_database_uid_file* uids_file = &auth_3d_db_file->uid;
    for (size_t i = 0; i < uid_file_count; i++) {
        auth_3d_database_uid_file* uid_file = &uids_file->begin[i];
        int32_t org_uid;
        if (!mdata)
            org_uid = (int32_t)i;
        else if (uid_file->flags & AUTH_3D_DATABASE_UID_ORG_UID)
            org_uid = uid_file->org_uid;
        else
            org_uid = (int32_t)(uid_count + i);

        auth_3d_database_uid* uid = &uids->begin[org_uid];
        uid->org_uid = org_uid;
        uid->enabled = false;

        if (uid_file->value.length >= 3) {
            if (string_data(&uid_file->value)[0] == 'A')
                uid->enabled = true;

            if (mdata)
                string_free(&uid->name);
            string_init_length(&uid->name, string_data(&uid_file->value) + 2, uid_file->value.length - 2);
        }

        if (uid->enabled) {
            if (mdata)
                string_replace(&uid_file->category, &uid->category);
            else
                string_copy(&uid_file->category, &uid->category);

            if (uid->category.length > 0) {
                vector_auth_3d_database_category* category = &auth_3d_db->category;
                for (auth_3d_database_category* j = category->begin; j != category->end; j++) {
                    if (!string_compare(&uid->category, &j->name))
                        continue;

                    bool found = false;
                    for (int32_t* k = j->uid.begin; k != j->uid.end; k++)
                        if (*k == org_uid) {
                            found = true;
                            break;
                        }

                    if (!found)
                        vector_int32_t_push_back(&j->uid, &org_uid);
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
    key_val_init(&kv, data, length);
    key_val lkv;

    len = 8;
    memcpy(buf, "category", 8);
    off = len;

    auth_3d_db_file->category = vector_empty(string);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "category", &lkv)) {
        vector_string* vc = &auth_3d_db_file->category;

        vector_string_reserve(vc, count);
        vc->end += count;
        for (int32_t i = 0; i < count; i++) {
            string* c = &vc->begin[i];
            *c = string_empty;
            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, ".value", 7, c);
        }
        key_val_free(&lkv);
    }

    len = 3;
    memcpy(buf, "uid", 3);
    off = len;

    auth_3d_db_file->uid = vector_empty(auth_3d_database_uid_file);
    if (!key_val_read_int32_t(&kv, buf, off, ".max", 5, &auth_3d_db_file->uid_max))
        auth_3d_db_file->uid_max = -1;

    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "uid", &lkv)) {
        vector_auth_3d_database_uid_file* vu = &auth_3d_db_file->uid;

        vector_auth_3d_database_uid_file_reserve(vu, count);
        vu->end += count;
        for (int32_t i = 0; i < count; i++) {
            auth_3d_database_uid_file* u = &vu->begin[i];
            memset(u, 0, sizeof(auth_3d_database_uid_file));
            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, ".category", 10, &u->category);
            if (key_val_read_int32_t(&lkv,
                buf, off, ".org_uid", 9, &u->org_uid))
                u->flags |= AUTH_3D_DATABASE_UID_ORG_UID;
            if (key_val_read_float_t(&lkv,
                buf, off, ".size", 6, &u->size))
                u->flags |= AUTH_3D_DATABASE_UID_SIZE;
            key_val_read_string(&lkv,
                buf, off, ".value", 7, &u->value);
        }
        key_val_free(&lkv);
    }

    key_val_free(&kv);
}

static void auth_3d_database_file_write_text(auth_3d_database_file* auth_3d_db_file, void** data, size_t* length) {
    char buf[AUTH_3D_DATABASE_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t off;

    stream s;
    io_mopen(&s, 0, 0);
    
    io_write(&s, "#A3DA__________\n", 16);
    io_write(&s, "# date time was eliminated.\n", 28);

    vector_int32_t sort_index = vector_empty(int32_t);
    if (auth_3d_db_file->category.end - auth_3d_db_file->category.begin) {
        len = 8;
        memcpy(buf, "category", 8);
        off = len;

        vector_string* vc = &auth_3d_db_file->category;
        count = (int32_t)(vc->end - vc->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            string* c = &vc->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, ".value", 7, c);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (auth_3d_db_file->uid.end - auth_3d_db_file->uid.begin) {
        len = 3;
        memcpy(buf, "uid", 3);
        off = len;

        int32_t uid_max = -1;
        vector_auth_3d_database_uid_file* vu = &auth_3d_db_file->uid;
        count = (int32_t)(vu->end - vu->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            auth_3d_database_uid_file* u = &vu->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, AUTH_3D_DATABASE_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, ".category", 10, &u->category);
            if (u->flags & AUTH_3D_DATABASE_UID_ORG_UID)
                key_val_write_int32_t(&s, buf, off, ".org_uid", 9, u->org_uid);
            if (u->flags & AUTH_3D_DATABASE_UID_SIZE)
                key_val_write_float_t(&s, buf, off, ".size", 6, u->size);
            key_val_write_string(&s, buf, off, ".value", 7, &u->value);

            if (u->flags & AUTH_3D_DATABASE_UID_ORG_UID && u->org_uid > uid_max)
                uid_max = u->org_uid;
        }

        if (uid_max == 0)
            uid_max = count;

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
        key_val_write_int32_t(&s, buf, off, ".max", 5, uid_max);
    }

    vector_int32_t_free(&sort_index, 0);

    io_mcopy(&s, data, length);
    io_free(&s);
}
