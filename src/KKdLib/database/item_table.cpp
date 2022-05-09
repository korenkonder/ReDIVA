/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "item_table.hpp"
#include "../io/path.h"
#include "../io/stream.h"
#include "../key_val.hpp"
#include "../sort.hpp"
#include "../str_utils.h"

#define ITEM_TABLE_TEXT_BUF_SIZE 0x400

static void itm_table_read_inner(itm_table* itm_tbl, stream* s);
static void itm_table_write_inner(itm_table* itm_tbl, stream* s);
static void itm_table_read_text(itm_table* itm_tbl, void* data, size_t size);
static void itm_table_write_text(itm_table* itm_tbl, void** data, size_t* size);

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

    wchar_t* path_bin = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_bin)) {
        stream s;
        io_open(&s, path_bin, L"rb");
        if (s.io.stream)
            itm_table_read_inner(this, &s);
        io_free(&s);
    }
    free(path_bin);
}

void itm_table::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    stream s;
    io_open(&s, data, size);
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

    wchar_t* path_bin = str_utils_add(path, L".txt");
    stream s;
    io_open(&s, path_bin, L"wb");
    if (s.io.stream)
        itm_table_write_inner(this, &s);
    io_free(&s);
    free(path_bin);
}

void itm_table::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
    io_open(&s);
    itm_table_write_inner(this, &s);
    io_copy(&s, data, size);
    io_free(&s);
}

bool itm_table::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    itm_table* itm_tbl = (itm_table*)data;
    itm_tbl->read(s.c_str());

    return itm_tbl->ready;
}

const char* chara_index_get_name(chara_index chara_index) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return 0;

    const char* chara_names[] = {
        "MIKU",
        "RIN",
        "LEN",
        "LUKA",
        "NERU",
        "HAKU",
        "KAITO",
        "MEIKO",
        "SAKINE",
        "TETO",
        "EXTRA",
    };

    return chara_names[chara_index];
}

const char* chara_index_get_auth_3d_name(chara_index chara_index) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return 0;

    const char* chara_names[] = {
        "MIK",
        "RIN",
        "LEN",
        "LUK",
        "NER",
        "HAK",
        "KAI",
        "MEI",
        "SAK",
        "TET",
        "EXT",
    };

    return chara_names[chara_index];
}

itm_table_item_data_tex::itm_table_item_data_tex() {

}

itm_table_item_data_tex::~itm_table_item_data_tex() {

}

itm_table_item_data_obj::itm_table_item_data_obj() : rpk() {

}

itm_table_item_data_obj::~itm_table_item_data_obj() {

}

itm_table_item_data_col::itm_table_item_data_col() : flag(), col_tone() {

}

itm_table_item_data_col::~itm_table_item_data_col() {

}

itm_table_item_data::itm_table_item_data() {

}

itm_table_item_data::~itm_table_item_data() {

}

itm_table_item::itm_table_item() : no(), flag(), type(), attr(), des_id(),
sub_id(), exclusion(), point(), org_itm(), npr_flag(), face_depth() {

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

static void itm_table_read_text(itm_table* itm_tbl, void* data, size_t size) {
    char buf[ITEM_TABLE_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t len4;
    size_t off;

    key_val kv;
    kv.parse((uint8_t*)data, size);
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

    itm_tbl->dbgset.clear();
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

    len = 4;
    memcpy(buf, "item", 4);
    off = len;

    itm_tbl->item.clear();
    if (kv.read_int32_t(buf, off, ".length", 8, &count)
        && kv.get_local_key_val("item", &lkv)) {
        std::vector<itm_table_item>& vi = itm_tbl->item;

        vi.reserve(count);
        for (int32_t i = 0; i < count; i++) {

            len1 = sprintf_s(buf + len, ITEM_TABLE_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            itm_table_item itm;
            if (!lkv.read_int32_t(buf, off, ".no", 4, &itm.no))
                continue;

            if (!lkv.read_int32_t(
                buf, off, ".flag", 6, &itm.flag)
                || !lkv.read_string(
                    buf, off, ".name", 6, &itm.name))
                continue;

            len2 = 7;
            memcpy(buf + len + len1, ".objset", 7);
            off = len + len1 + len2;

            if (off >= 0 && off < ITEM_TABLE_TEXT_BUF_SIZE)
                buf[off] = 0;
            key_val sub_local_key_val;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<std::string>& vio = itm.objset;

                vio.resize(count1);
                for (int32_t j = 0; j < count1; j++) {
                    len3 = sprintf_s(buf + len + len1 + len2,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    sub_local_key_val.read_string(
                        buf, off, "", 1, &vio[j]);
                }
            }

            off = len + len1;
            if (!lkv.read_int32_t(
                buf, off, ".type", 6, &itm.type)
                || !lkv.read_int32_t(
                    buf, off, ".attr", 6, &itm.attr)
                || !lkv.read_int32_t(
                    buf, off, ".des_id", 8, &itm.des_id)
                || !lkv.read_int32_t(
                    buf, off, ".sub_id", 8, (int32_t*)&itm.sub_id))
                continue;

            len2 = 5;
            memcpy(buf + len + len1, ".data", 5);
            off = len + len1 + len2;

            len3 = 4;
            memcpy(buf + len + len1 + len2, ".obj", 4);
            off = len + len1 + len2 + len3;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<itm_table_item_data_obj>& vido = itm.data.obj;

                vido.reserve(count1);
                for (int32_t j = 0; j < count1; j++) {

                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", j);
                    off = len + len1 + len2 + len3 + len4;

                    itm_table_item_data_obj obj;
                    sub_local_key_val.read_int32_t(
                        buf, off, ".rpk", 5, (int32_t*)&obj.rpk);
                    sub_local_key_val.read_string(
                        buf, off, ".uid", 5, &obj.uid);
                    vido.push_back(obj);
                }
            }

            len3 = 4;
            memcpy(buf + len + len1 + len2, ".ofs", 4);
            off = len + len1 + len2 + len3;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<itm_table_item_data_ofs>& vido = itm.data.ofs;

                vido.reserve(count1);
                for (int32_t j = 0; j < count1; j++) {
                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", j);
                    off = len + len1 + len2 + len3 + len4;

                    itm_table_item_data_ofs ofs;
                    if (sub_local_key_val.read_int32_t(
                        buf, off, ".no", 4, &ofs.no)
                        && sub_local_key_val.read_int32_t(
                            buf, off, ".sub_id", 8, (int32_t*)&ofs.sub_id)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".tx", 4, &ofs.position.x)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".ty", 4, &ofs.position.y)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".tz", 4, &ofs.position.z)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".rx", 4, &ofs.rotation.x)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".ry", 4, &ofs.rotation.y)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".rz", 4, &ofs.rotation.z)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".sx", 4, &ofs.scale.x)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".sy", 4, &ofs.scale.y)
                        && sub_local_key_val.read_float_t(
                            buf, off, ".sz", 4, &ofs.scale.z))
                        vido.push_back(ofs);
                }
            }

            len3 = 4;
            memcpy(buf + len + len1 + len2, ".tex", 4);
            off = len + len1 + len2 + len3;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<itm_table_item_data_tex>& vidt = itm.data.tex;

                vidt.reserve(count1);
                for (int32_t j = 0; j < count1; j++) {
                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", j);
                    off = len + len1 + len2 + len3 + len4;

                    itm_table_item_data_tex tex;
                    sub_local_key_val.read_string(
                        buf, off, ".org", 5, &tex.org);
                    sub_local_key_val.read_string(
                        buf, off, ".chg", 5, &tex.chg);
                    vidt.push_back(tex);
                }
            }

            len3 = 4;
            memcpy(buf + len + len1 + len2, ".col", 4);
            off = len + len1 + len2 + len3;
            if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                && lkv.get_local_key_val(buf, &sub_local_key_val)) {
                std::vector<itm_table_item_data_col>& vidc = itm.data.col;

                vidc.reserve(count1);
                for (int32_t j = 0; j < count1; j++) {
                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", j);
                    off = len + len1 + len2 + len3 + len4;

                    itm_table_item_data_col col;
                    sub_local_key_val.read_string(
                        buf, off, ".tex", 5, &col.tex);
                    sub_local_key_val.read_int32_t(
                        buf, off, ".flag", 6, &col.flag);

                    if (~col.flag & 0x01) {
                        vidc.push_back(col);
                        continue;
                    }

                    sub_local_key_val.read_float_t(
                        buf, off, ".blend.0", 9, &col.col_tone.blend.x);
                    sub_local_key_val.read_float_t(
                        buf, off, ".blend.1", 9, &col.col_tone.blend.y);
                    sub_local_key_val.read_float_t(
                        buf, off, ".blend.2", 9, &col.col_tone.blend.z);
                    sub_local_key_val.read_float_t(
                        buf, off, ".offset.0", 10, &col.col_tone.offset.x);
                    sub_local_key_val.read_float_t(
                        buf, off, ".offset.1", 10, &col.col_tone.offset.y);
                    sub_local_key_val.read_float_t(
                        buf, off, ".offset.2", 10, &col.col_tone.offset.z);
                    sub_local_key_val.read_float_t(
                        buf, off, ".hue", 5, &col.col_tone.hue);
                    sub_local_key_val.read_float_t(
                        buf, off, ".saturation", 12, &col.col_tone.saturation);
                    sub_local_key_val.read_float_t(
                        buf, off, ".value", 7, &col.col_tone.value);
                    sub_local_key_val.read_float_t(
                        buf, off, ".contrast", 10, &col.col_tone.contrast);
                    sub_local_key_val.read_bool(
                        buf, off, ".inverse", 9, &col.col_tone.inverse);
                    vidc.push_back(col);
                }
            }

            off = len + len1;
            if (!lkv.read_int32_t(
                buf, off, ".exclusion", 11, &itm.exclusion)
                || !lkv.read_int32_t(
                    buf, off, ".point", 7, &itm.point)
                || !lkv.read_int32_t(
                    buf, off, ".org_itm", 9, &itm.org_itm))
                continue;

            lkv.read_bool(
                buf, off, ".npr_flag", 10, &itm.npr_flag);
            lkv.read_float_t(
                buf, off, ".face_depth", 12, &itm.face_depth);
            vi.push_back(itm);
        }
    }
}

static void itm_table_write_text(itm_table* itm_tbl, void** data, size_t* size) {
    char buf[ITEM_TABLE_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t len4;
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

        std::vector<itm_table_dbgset>& vd = itm_tbl->dbgset;
        count = (int32_t)vd.size();
        std::vector<int32_t> sort_index;
        key_val::get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            itm_table_dbgset* d = &vd[sort_index[i]];

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
            key_val::write_string(&s, buf, off, ".name", 6, d->name);
        }

        off = len;
        key_val::write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (itm_tbl->item.size() > 0) {
        len = 4;
        memcpy(buf, "item", 4);
        off = len;

        std::vector<itm_table_item>& vi = itm_tbl->item;
        count = (int32_t)vi.size();
        std::vector<int32_t> sort_index;
        key_val::get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            itm_table_item* itm = &vi[sort_index[i]];

            len1 = sprintf_s(buf + len, ITEM_TABLE_TEXT_BUF_SIZE - len, ".%d", sort_index[i]);
            off = len + len1;

            key_val::write_int32_t(&s, buf, off, ".attr", 6, itm->attr);

            len2 = 5;
            memcpy(buf + len + len1, ".data", 5);
            off = len + len1 + len2;

            if (itm->data.col.size()) {
                len3 = 4;
                memcpy(buf + len + len1 + len2, ".col", 4);
                off = len + len1 + len2 + len3;

                std::vector<itm_table_item_data_col>& vidc = itm->data.col;
                count1 = (int32_t)vidc.size();
                std::vector<int32_t> sort_index;
                key_val::get_lexicographic_order(&sort_index, count1);
                for (int32_t j = 0; j < count1; j++) {
                    itm_table_item_data_col* col = &vidc[sort_index[j]];

                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", sort_index[j]);
                    off = len + len1 + len2 + len3 + len4;

                    if (col->flag & 0x01) {
                        key_val::write_float_t(&s, buf, off, ".blend.0", 9, col->col_tone.blend.x);
                        key_val::write_float_t(&s, buf, off, ".blend.1", 9, col->col_tone.blend.y);
                        key_val::write_float_t(&s, buf, off, ".blend.2", 9, col->col_tone.blend.z);
                        key_val::write_float_t(&s, buf, off, ".contrast", 10, col->col_tone.contrast);
                    }

                    key_val::write_int32_t(&s, buf, off, ".flag", 6, col->flag);

                    if (col->flag & 0x01) {
                        key_val::write_float_t(&s, buf, off, ".hue", 5, col->col_tone.hue);
                        key_val::write_int32_t(&s, buf, off, ".inverse", 9, col->col_tone.inverse ? 1 : 0);
                        key_val::write_float_t(&s, buf, off, ".offset.0", 10, col->col_tone.offset.x);
                        key_val::write_float_t(&s, buf, off, ".offset.1", 10, col->col_tone.offset.y);
                        key_val::write_float_t(&s, buf, off, ".offset.2", 10, col->col_tone.offset.z);
                        key_val::write_float_t(&s, buf, off, ".saturation", 12, col->col_tone.saturation);
                    }

                    key_val::write_string(&s, buf, off, ".tex", 5, col->tex);

                    if (col->flag & 0x01)
                        key_val::write_float_t(&s, buf, off, ".value", 7, col->col_tone.value);
                }

                off = len + len1 + len2 + len3;
                key_val::write_int32_t(&s, buf, off, ".length", 8, count1);
            }

            if (itm->data.obj.size()) {
                len3 = 4;
                memcpy(buf + len + len1 + len2, ".obj", 4);
                off = len + len1 + len2 + len3;

                std::vector<itm_table_item_data_obj>& vido = itm->data.obj;
                count1 = (int32_t)vido.size();
                std::vector<int32_t> sort_index;
                key_val::get_lexicographic_order(&sort_index, count1);
                for (int32_t j = 0; j < count1; j++) {
                    itm_table_item_data_obj* obj = &vido[sort_index[j]];

                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", sort_index[j]);
                    off = len + len1 + len2 + len3 + len4;

                    key_val::write_int32_t(&s, buf, off, ".rpk", 5, obj->rpk);
                    key_val::write_string(&s, buf, off, ".uid", 5, obj->uid);
                }

                off = len + len1 + len2 + len3;
                key_val::write_int32_t(&s, buf, off, ".length", 8, count1);
            }

            if (itm->data.ofs.size()) {
                len3 = 4;
                memcpy(buf + len + len1 + len2, ".ofs", 4);
                off = len + len1 + len2 + len3;

                std::vector<itm_table_item_data_ofs>& vidof = itm->data.ofs;
                count1 = (int32_t)vidof.size();
                std::vector<int32_t> sort_index;
                key_val::get_lexicographic_order(&sort_index, count1);
                for (int32_t j = 0; j < count1; j++) {
                    itm_table_item_data_ofs* ofs = &vidof[sort_index[j]];

                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", sort_index[j]);
                    off = len + len1 + len2 + len3 + len4;

                    key_val::write_int32_t(&s, buf, off, ".no", 4, ofs->no);
                    key_val::write_float_t(&s, buf, off, ".rx", 4, ofs->rotation.x);
                    key_val::write_float_t(&s, buf, off, ".ry", 4, ofs->rotation.y);
                    key_val::write_float_t(&s, buf, off, ".rz", 4, ofs->rotation.z);
                    key_val::write_int32_t(&s, buf, off, ".sub_id", 8, ofs->sub_id);
                    key_val::write_float_t(&s, buf, off, ".sx", 4, ofs->scale.x);
                    key_val::write_float_t(&s, buf, off, ".sy", 4, ofs->scale.y);
                    key_val::write_float_t(&s, buf, off, ".sz", 4, ofs->scale.z);
                    key_val::write_float_t(&s, buf, off, ".tx", 4, ofs->position.x);
                    key_val::write_float_t(&s, buf, off, ".ty", 4, ofs->position.y);
                    key_val::write_float_t(&s, buf, off, ".tz", 4, ofs->position.z);
                }

                off = len + len1 + len2 + len3;
                key_val::write_int32_t(&s, buf, off, ".length", 8, count1);
            }

            if (itm->data.tex.size()) {
                len3 = 4;
                memcpy(buf + len + len1 + len2, ".tex", 4);
                off = len + len1 + len2 + len3;

                std::vector<itm_table_item_data_tex>& vidt = itm->data.tex;
                count1 = (int32_t)vidt.size();
                std::vector<int32_t> sort_index;
                key_val::get_lexicographic_order(&sort_index, count1);
                for (int32_t j = 0; j < count1; j++) {
                    itm_table_item_data_tex* tex = &vidt[sort_index[j]];

                    len4 = sprintf_s(buf + len + len1 + len2 + len3,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2 - len3, ".%d", sort_index[j]);
                    off = len + len1 + len2 + len3 + len4;

                    key_val::write_string(&s, buf, off, ".chg", 5, tex->chg);
                    key_val::write_string(&s, buf, off, ".org", 5, tex->org);
                }

                off = len + len1 + len2 + len3;
                key_val::write_int32_t(&s, buf, off, ".length", 8, count1);
            }

            off = len + len1;
            key_val::write_int32_t(&s, buf, off, ".des_id", 8, itm->des_id);
            key_val::write_int32_t(&s, buf, off, ".exclusion", 11, itm->exclusion);
            key_val::write_float_t(&s, buf, off, ".face_depth", 12, itm->face_depth);
            key_val::write_int32_t(&s, buf, off, ".flag", 6, itm->flag);
            key_val::write_string(&s, buf, off, ".name", 6, itm->name);
            key_val::write_int32_t(&s, buf, off, ".no", 4, itm->no);
            if (itm->npr_flag)
                key_val::write_int32_t(&s, buf, off, ".npr_flag", 10, itm->npr_flag);

            if (itm->objset.size()) {
                len2 = 7;
                memcpy(buf + len + len1, ".objset", 7);
                off = len + len1 + len2;

                if (off >= 0 && off < ITEM_TABLE_TEXT_BUF_SIZE)
                    buf[off] = 0;

                std::vector<std::string>& vio = itm->objset;
                count1 = (int32_t)vio.size();
                std::vector<int32_t> sort_index;
                key_val::get_lexicographic_order(&sort_index, count1);
                for (int32_t j = 0; j < count1; j++) {
                    len3 = sprintf_s(buf + len + len1 + len2,
                        ITEM_TABLE_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index[j]);
                    off = len + len1 + len2 + len3;

                    key_val::write_string(&s, buf, off, "", 1, vio[sort_index[j]]);
                }

                off = len + len1 + len2;
                key_val::write_int32_t(&s, buf, off, ".length", 8, count1);
            }

            off = len + len1;
            key_val::write_int32_t(&s, buf, off, ".org_itm", 9, itm->org_itm);
            key_val::write_int32_t(&s, buf, off, ".point", 7, itm->point);
            key_val::write_int32_t(&s, buf, off, ".sub_id", 8, itm->sub_id);
            key_val::write_int32_t(&s, buf, off, ".type", 6, itm->type);
        }

        off = len;
        key_val::write_int32_t(&s, buf, off, ".length", 8, count);
    }

    io_copy(&s, data, size);
    io_free(&s);
}