/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "item_table.h"
#include "../io/path.h"
#include "../io/stream.h"
#include "../key_val.h"
#include "../sort.h"
#include "../str_utils.h"

#define ITEM_TABLE_TEXT_BUF_SIZE 0x400

static void itm_table_read_inner(itm_table* itm_tbl, stream* s);
static void itm_table_write_inner(itm_table* itm_tbl, stream* s);
static void itm_table_read_text(itm_table* itm_tbl, void* data, size_t length);
static void itm_table_write_text(itm_table* itm_tbl, void** data, size_t* length);

itm_table::itm_table() : ready() {

}

itm_table::~itm_table() {

}

void itm_table::read(const char* path) {
    if (!path)
        return;

    char* path_bin = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_bin)) {
        stream s;
        io_open(&s, path_bin, "rb");
        if (s.io.stream)
            itm_table_read_inner(this, &s);
        io_free(&s);
    }
    free(path_bin);
}

void itm_table::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_bin = str_utils_wadd(path, L".txt");
    if (path_check_file_exists(path_bin)) {
        stream s;
        io_open(&s, path_bin, L"rb");
        if (s.io.stream)
            itm_table_read_inner(this, &s);
        io_free(&s);
    }
    free(path_bin);
}

void itm_table::read(const void* data, size_t length) {
    if (!data || !length)
        return;

    stream s;
    io_open(&s, data, length);
    itm_table_read_inner(this, &s);
    io_free(&s);
}

void itm_table::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_bin = str_utils_add(path, ".txt");
    stream s;
    io_open(&s, path_bin, "wb");
    if (s.io.stream)
        itm_table_write_inner(this, &s);
    io_free(&s);
    free(path_bin);
}

void itm_table::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_bin = str_utils_wadd(path, L".txt");
    stream s;
    io_open(&s, path_bin, L"wb");
    if (s.io.stream)
        itm_table_write_inner(this, &s);
    io_free(&s);
    free(path_bin);
}

void itm_table::write(void** data, size_t* length) {
    if (!data || !length || !ready)
        return;

    stream s;
    io_open(&s);
    itm_table_write_inner(this, &s);
    io_copy(&s, data, length);
    io_free(&s);
}

bool itm_table::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    itm_table* itm_tbl = (itm_table*)data;
    itm_tbl->read(string_data(&s));

    string_free(&s);
    return itm_tbl->ready;
}

itm_table_itm_data::itm_table_itm_data() {

}

itm_table_itm_data::~itm_table_itm_data() {

}

itm_table_item::itm_table_item() : flag(), type(), attr(), des_id(),
sub_id(), exclusion_point(), field_AC(), org_itm(), npr_flag(), face_depth() {

}

itm_table_item::~itm_table_item() {

}

itm_table_cos::itm_table_cos() : id() {

}

itm_table_cos::~itm_table_cos() {

}

itm_table_dbgset::itm_table_dbgset() {

}

itm_table_dbgset::~itm_table_dbgset() {

}

static void itm_table_read_inner(itm_table* itm_tbl, stream* s) {
    void* itm_tbl_data = force_malloc(s->length);
    io_read(s, itm_tbl_data, s->length);
    itm_table_read_text(itm_tbl, itm_tbl_data, s->length);
    free(itm_tbl_data);

    itm_tbl->ready = true;
}

static void itm_table_write_inner(itm_table* itm_tbl, stream* s) {
    void* itm_tbl_data = 0;
    size_t itm_tbl_data_length = 0;
    itm_table_write_text(itm_tbl, &itm_tbl_data, &itm_tbl_data_length);
    io_write(s, itm_tbl_data, itm_tbl_data_length);
    free(itm_tbl_data);
}

static void itm_table_read_text(itm_table* itm_tbl, void* data, size_t length) {
    char buf[ITEM_TABLE_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    key_val kv;
    kv.parse((uint8_t*)data, length);
    key_val lkv;

    len = 3;
    memcpy(buf, "cos", 3);
    off = len;

    itm_tbl->item.clear();
    if (kv.read_int32_t(buf, off, ".length", 8, &count)
        && kv.get_local_key_val("cos", &lkv)) {
        std::vector<itm_table_cos>& vc = itm_tbl->cos;

        vc.resize(count);
        for (int32_t i = 0; i < count; i++) {
            itm_table_cos* c = &vc[i];

            len1 = sprintf_s(buf + len, ITEM_TABLE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            lkv.read_int32_t(
                buf, off, ".id", 4, &c->id);

            len2 = 5;
            memcpy(buf + len + len1, ".item", 5);
            off = len + len1 + len2;

            if (off >= 0 && off < ITEM_TABLE_TEXT_BUF_SIZE)
                buf[off] = 0;
            key_val sub_local_key_val;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<int32_t>& vci = c->item;

                vci.resize(count1);
                for (int32_t j = 0; j < count1; j++) {
                    len3 = sprintf_s(buf + len + len1 + len2,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    sub_local_key_val.read_int32_t(
                        buf, off, "", 1, &vci[j]);
                }
            }
        }
    }

    len = 6;
    memcpy(buf, "dbgset", 6);
    off = len;

    itm_tbl->item.clear();
    if (kv.read_int32_t(buf, off, ".length", 8, &count)
        && kv.get_local_key_val("dbgset", &lkv)) {
        std::vector<itm_table_dbgset>& vd = itm_tbl->dbgset;

        vd.resize(count);
        for (int32_t i = 0; i < count; i++) {
            itm_table_dbgset* d = &vd[i];

            len1 = sprintf_s(buf + len, ITEM_TABLE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            len2 = 5;
            memcpy(buf + len + len1, ".item", 5);
            off = len + len1 + len2;

            if (off >= 0 && off < ITEM_TABLE_TEXT_BUF_SIZE)
                buf[off] = 0;
            key_val sub_local_key_val;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<int32_t>& vdi = d->item;

                vdi.resize(count1);
                for (int32_t j = 0; j < count1; j++) {
                    len3 = sprintf_s(buf + len + len1 + len2,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    sub_local_key_val.read_int32_t(
                        buf, off, "", 1, &vdi[j]);
                }
            }

            off = len + len1;
            lkv.read_string(
                buf, off, ".name", 6, &d->name);

        }
    }
}

static void itm_table_write_text(itm_table* itm_tbl, void** data, size_t* length) {
    char buf[ITEM_TABLE_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    stream s;
    io_open(&s);

    if (itm_tbl->cos.size() > 0) {
        len = 3;
        memcpy(buf, "cos", 3);
        off = len;

        std::vector<itm_table_cos>& vc = itm_tbl->cos;
        count = (int32_t)vc.size();
        std::vector<int32_t> sort_index;
        key_val::get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            itm_table_cos* c = &vc[sort_index[i]];

            len1 = sprintf_s(buf + len, ITEM_TABLE_TEXT_BUF_SIZE - len, ".%d", sort_index[i]);
            off = len + len1;

            key_val::write_int32_t(&s, buf, off, ".id", 4, c->id);

            len2 = 5;
            memcpy(buf + len + len1, ".item", 5);
            off = len + len1 + len2;

            std::vector<int32_t>& vci = c->item;
            count1 = (int32_t)vci.size();
            std::vector<int32_t> sort_index1;
            key_val::get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count1; j++) {
                len3 = sprintf_s(buf + len + len1 + len2,
                    ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index1[j]);
                off = len + len1 + len2 + len3;

                key_val::write_int32_t(&s,
                    buf, off, "", 1, vci[sort_index1[j]]);
            }

            off = len + len1 + len2;
            key_val::write_int32_t(&s, buf, off, ".length", 8, count1);
        }

        off = len;
        key_val::write_int32_t(&s, buf, off, ".length", 8, count);
    }
    
    if (itm_tbl->dbgset.size() > 0) {
        len = 6;
        memcpy(buf, "dbgset", 6);
        off = len;

        std::vector<itm_table_dbgset>& vc = itm_tbl->dbgset;
        count = (int32_t)vc.size();
        std::vector<int32_t> sort_index;
        key_val::get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            itm_table_dbgset* d = &vc[sort_index[i]];

            len1 = sprintf_s(buf + len, ITEM_TABLE_TEXT_BUF_SIZE - len, ".%d", sort_index[i]);
            off = len + len1;

            len2 = 5;
            memcpy(buf + len + len1, ".item", 5);
            off = len + len1 + len2;

            std::vector<int32_t>& vdi = d->item;
            count1 = (int32_t)vdi.size();
            std::vector<int32_t> sort_index1;
            key_val::get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count1; j++) {
                len3 = sprintf_s(buf + len + len1 + len2,
                    ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index1[j]);
                off = len + len1 + len2 + len3;

                key_val::write_int32_t(&s,
                    buf, off, "", 1, vdi[sort_index1[j]]);
            }

            off = len + len1 + len2;
            key_val::write_int32_t(&s, buf, off, ".length", 8, count1);

            off = len + len1;
            key_val::write_string(&s, buf, off, ".name", 6, &d->name);
        }

        off = len;
        key_val::write_int32_t(&s, buf, off, ".length", 8, count);
    }

    io_copy(&s, data, length);
    io_free(&s);
}