/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "hand_item.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../key_val.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

static void hnd_itm_read_inner(hnd_itm* itm_tbl, stream& s);
static void hnd_itm_write_inner(hnd_itm* itm_tbl, stream& s);
static void hnd_itm_read_text(hnd_itm* itm_tbl, void* data, size_t size);
static void hnd_itm_write_text(hnd_itm* itm_tbl, void** data, size_t* size);

hnd_itm_data::hnd_itm_data() : file_size(), uid() {
    hand_scale = -1.0f;
}

hnd_itm_data::~hnd_itm_data() {

}

hnd_itm::hnd_itm() : ready() {

}

hnd_itm::~hnd_itm() {

}

void hnd_itm::read(const char* path) {
    if (!path)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, "rb");
        if (s.check_not_null())
            hnd_itm_read_inner(this, s);
    }
    free_def(path_txt);
}

void hnd_itm::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    if (path_check_file_exists(path_txt)) {
        file_stream s;
        s.open(path_txt, L"rb");
        if (s.check_not_null())
            hnd_itm_read_inner(this, s);
    }
    free_def(path_txt);
}

void hnd_itm::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    hnd_itm_read_inner(this, s);
}

void hnd_itm::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_txt = str_utils_add(path, ".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, "wb");
    if (s.check_not_null())
        hnd_itm_write_inner(this, s);
    free_def(path_txt);
}

void hnd_itm::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_txt = str_utils_add(path, L".txt");
    if (!path_txt)
        return;

    file_stream s;
    s.open(path_txt, L"wb");
    if (s.check_not_null())
        hnd_itm_write_inner(this, s);
    free_def(path_txt);
}

void hnd_itm::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    hnd_itm_write_inner(this, s);
    s.copy(data, size);
}

bool hnd_itm::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string path(dir);
    path.append(file, file_len);

    hnd_itm* itm_tbl = (hnd_itm*)data;
    itm_tbl->read(path.c_str());

    return itm_tbl->ready;
}

static void hnd_itm_read_inner(hnd_itm* itm_tbl, stream& s) {
    void* itm_tbl_data = force_malloc(s.length);
    s.read(itm_tbl_data, s.length);
    hnd_itm_read_text(itm_tbl, itm_tbl_data, s.length);
    free_def(itm_tbl_data);

    itm_tbl->ready = true;
}

static void hnd_itm_write_inner(hnd_itm* itm_tbl, stream& s) {
    void* itm_tbl_data = 0;
    size_t itm_tbl_data_length = 0;
    hnd_itm_write_text(itm_tbl, &itm_tbl_data, &itm_tbl_data_length);
    s.write(itm_tbl_data, itm_tbl_data_length);
    free_def(itm_tbl_data);
}

static void hnd_itm_read_text(hnd_itm* itm_tbl, void* data, size_t size) {
    key_val kv;
    kv.parse(data, size);

    itm_tbl->data.clear();
    int32_t count;
    if (kv.read("data", "length", count)) {
        std::vector<hnd_itm_data>& vd = itm_tbl->data;

        vd.resize(count);
        for (int32_t i = 0; i < count; i++) {
            if (!kv.open_scope_fmt(i))
                continue;

            hnd_itm_data* d = &vd[i];

            kv.read("objname_left", d->objname_left);
            kv.read("objname_right", d->objname_right);
            kv.read("item_str", d->item_str);
            kv.read("item_name", d->item_name);
            kv.read("file_size", d->file_size);
            kv.read("hand_motion", d->hand_motion);
            kv.read("hand_scale", d->hand_scale);
            kv.read("uid", d->uid);

            kv.close_scope();
        }
        kv.close_scope();
    }
}

static void hnd_itm_write_text(hnd_itm* itm_tbl, void** data, size_t* size) {
    memory_stream s;
    s.open();

    key_val_out kv;
    if (itm_tbl->data.size() > 0) {
        kv.open_scope("cos");

        int32_t count = (int32_t)itm_tbl->data.size();
        hnd_itm_data* vd = itm_tbl->data.data();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, count);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < count; i++) {
            kv.open_scope_fmt(sort_index_data[i]);
            hnd_itm_data* d = &vd[sort_index_data[i]];

            kv.write(s, "objname_left", d->objname_left);
            kv.write(s, "objname_right", d->objname_right);
            kv.write(s, "item_str", d->item_str);
            kv.write(s, "item_name", d->item_name);
            kv.write(s, "file_size", d->file_size);
            kv.write(s, "hand_motion", d->hand_motion);
            kv.write(s, "hand_scale", d->hand_scale);
            kv.write(s, "uid", d->uid);

            kv.close_scope();
        }

        kv.write(s, "length", count);
        kv.close_scope();
    }

    s.copy(data, size);
}