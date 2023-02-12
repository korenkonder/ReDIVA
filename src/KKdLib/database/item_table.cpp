/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "item_table.hpp"
#include "../io/file_stream.hpp"
#include "../io/memory_stream.hpp"
#include "../io/path.hpp"
#include "../key_val.hpp"
#include "../sort.hpp"
#include "../str_utils.hpp"

const char* chara_auth_3d_names[] = {
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

const char* chara_face_mot_names[] = {
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

const char* chara_full_names[] = {
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

static void itm_table_read_inner(itm_table* itm_tbl, stream& s);
static void itm_table_write_inner(itm_table* itm_tbl, stream& s);
static void itm_table_read_text(itm_table* itm_tbl, void* data, size_t size);
static void itm_table_write_text(itm_table* itm_tbl, void** data, size_t* size);

itm_table_item_data_obj::itm_table_item_data_obj() : rpk() {

}

itm_table_item_data_obj::~itm_table_item_data_obj() {

}

itm_table_item_data_ofs::itm_table_item_data_ofs() : sub_id(), no() {

}

itm_table_item_data_tex::itm_table_item_data_tex() {

}

itm_table_item_data_tex::~itm_table_item_data_tex() {

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

itm_table::itm_table() : ready() {

}

itm_table::~itm_table() {

}

void itm_table::read(const char* path) {
    if (!path)
        return;

    char* path_bin = str_utils_add(path, ".txt");
    if (path_check_file_exists(path_bin)) {
        file_stream s;
        s.open(path_bin, "rb");
        if (s.check_not_null())
            itm_table_read_inner(this, s);
    }
    free_def(path_bin);
}

void itm_table::read(const wchar_t* path) {
    if (!path)
        return;

    wchar_t* path_bin = str_utils_add(path, L".txt");
    if (path_check_file_exists(path_bin)) {
        file_stream s;
        s.open(path_bin, L"rb");
        if (s.check_not_null())
            itm_table_read_inner(this, s);
    }
    free_def(path_bin);
}

void itm_table::read(const void* data, size_t size) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    itm_table_read_inner(this, s);
}

void itm_table::write(const char* path) {
    if (!path || !ready)
        return;

    char* path_bin = str_utils_add(path, ".txt");
    file_stream s;
    s.open(path_bin, "wb");
    if (s.check_not_null())
        itm_table_write_inner(this, s);
    free_def(path_bin);
}

void itm_table::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    wchar_t* path_bin = str_utils_add(path, L".txt");
    file_stream s;
    s.open(path_bin, L"wb");
    if (s.check_not_null())
        itm_table_write_inner(this, s);
    free_def(path_bin);
}

void itm_table::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    memory_stream s;
    s.open();
    itm_table_write_inner(this, s);
    s.copy(data, size);
}

bool itm_table::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    itm_table* itm_tbl = (itm_table*)data;
    itm_tbl->read(s.c_str());

    return itm_tbl->ready;
}

const char* chara_index_get_auth_3d_name(chara_index chara_index) {
    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        return chara_auth_3d_names[chara_index];
    return 0;
}

const char* chara_index_get_chara_name(chara_index chara_index) {
    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        return chara_names[chara_index];
    return 0;
}

const char* chara_index_get_face_mot_name(chara_index chara_index) {
    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        return chara_face_mot_names[chara_index];
    return 0;
}

chara_index chara_index_get_from_chara_name(const char* str) {
    if (!str)
        return CHARA_MAX;

    for (int32_t i = CHARA_MIKU; i < CHARA_MAX; i++)
        if (!str_utils_compare(str, chara_names[i]))
            return (chara_index)i;
    return CHARA_MAX;
}

const char* chara_index_get_name(chara_index chara_index) {
    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        return chara_full_names[chara_index];
    return 0;
}

static void itm_table_read_inner(itm_table* itm_tbl, stream& s) {
    void* itm_tbl_data = force_malloc(s.length);
    s.read(itm_tbl_data, s.length);
    itm_table_read_text(itm_tbl, itm_tbl_data, s.length);
    free_def(itm_tbl_data);

    itm_tbl->ready = true;
}

static void itm_table_write_inner(itm_table* itm_tbl, stream& s) {
    void* itm_tbl_data = 0;
    size_t itm_tbl_data_length = 0;
    itm_table_write_text(itm_tbl, &itm_tbl_data, &itm_tbl_data_length);
    s.write(itm_tbl_data, itm_tbl_data_length);
    free_def(itm_tbl_data);
}

static void itm_table_read_text(itm_table* itm_tbl, void* data, size_t size) {
    key_val kv;
    kv.parse(data, size);

    itm_tbl->item.clear();
    int32_t count;
    if (kv.read("cos", "length", count)) {
        std::vector<itm_table_cos>& vc = itm_tbl->cos;

        vc.reserve(count);
        for (int32_t i = 0; i < count; i++) {
            if (!kv.open_scope_fmt(i))
                continue;

            itm_table_cos c;
            kv.read("id", c.id);

            int32_t count;
            if (kv.read("item", "length", count)) {
                std::vector<int32_t>& vci = c.item;

                vci.resize(count);
                for (int32_t j = 0; j < count; j++) {
                    if (!kv.open_scope_fmt(j))
                        continue;

                    kv.read(vci[j]);
                    kv.close_scope();
                }
                kv.close_scope();
            }

            vc.push_back(c);

            kv.close_scope();
        }
        kv.close_scope();
    }

    itm_tbl->dbgset.clear();
    if (kv.read("dbgset", "length", count)) {
        std::vector<itm_table_dbgset>& vd = itm_tbl->dbgset;

        vd.reserve(count);
        for (int32_t i = 0; i < count; i++) {
            if (!kv.open_scope_fmt(i))
                continue;

            itm_table_dbgset d;
            int32_t count;
            if (kv.read("item", "length", count)) {
                std::vector<int32_t>& vdi = d.item;

                vdi.resize(count);
                for (int32_t j = 0; j < count; j++) {
                    if (!kv.open_scope_fmt(j))
                        continue;

                    kv.read(vdi[j]);
                    kv.close_scope();
                }
                kv.close_scope();
            }

            kv.read("name", d.name);
            vd.push_back(d);

            kv.close_scope();
        }
        kv.close_scope();
    }

    itm_tbl->item.clear();
    if (kv.read("item", "length", count)) {
        std::vector<itm_table_item>& vi = itm_tbl->item;

        vi.reserve(count);
        for (int32_t i = 0; i < count; i++) {
            if (!kv.open_scope_fmt(i))
                continue;

            itm_table_item itm;
            if (!kv.read("no", itm.no)) {
                kv.close_scope();
                continue;
            }

            if (!kv.read("flag", itm.flag)
                || !kv.read("name", itm.name)) {
                kv.close_scope();
                continue;
            }

            int32_t count;
            if (kv.read("objset", "length", count)) {
                std::vector<std::string>& vio = itm.objset;

                vio.resize(count);
                for (int32_t j = 0; j < count; j++) {
                    if (!kv.open_scope_fmt(j))
                        continue;

                    kv.read(vio[j]);
                    kv.close_scope();
                }
                kv.close_scope();
            }

            int32_t sub_id = 0;
            if (!kv.read("type", itm.type)
                || !kv.read("attr", itm.attr)
                || !kv.read("des_id", itm.des_id)
                || !kv.read("sub_id", sub_id)) {
                kv.close_scope();
                continue;
            }
            itm.sub_id = (item_sub_id)sub_id;

            if (kv.open_scope("data")) {
                int32_t count;
                if (kv.read("obj", "length", count)) {
                    std::vector<itm_table_item_data_obj>& vido = itm.data.obj;

                    vido.reserve(count);
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        itm_table_item_data_obj obj;
                        int32_t rpk = 0;
                        if (kv.read("rpk", rpk))
                            obj.rpk = (item_id)rpk;
                        kv.read("uid", obj.uid);
                        vido.push_back(obj);

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                if (kv.read("ofs", "length", count)) {
                    std::vector<itm_table_item_data_ofs>& vido = itm.data.ofs;

                    vido.reserve(count);
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        itm_table_item_data_ofs ofs;
                        int32_t sub_id = 0;
                        if (kv.read("no", ofs.no)
                            && kv.read("sub_id", sub_id)
                            && kv.read("tx", ofs.position.x)
                            && kv.read("ty", ofs.position.y)
                            && kv.read("tz", ofs.position.z)
                            && kv.read("rx", ofs.rotation.x)
                            && kv.read("ry", ofs.rotation.y)
                            && kv.read("rz", ofs.rotation.z)
                            && kv.read("sx", ofs.scale.x)
                            && kv.read("sy", ofs.scale.y)
                            && kv.read("sz", ofs.scale.z)) {
                            ofs.sub_id = (item_sub_id)sub_id;
                            vido.push_back(ofs);
                        }

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                if (kv.read("tex", "length", count)) {
                    std::vector<itm_table_item_data_tex>& vidt = itm.data.tex;

                    vidt.reserve(count);
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        itm_table_item_data_tex tex;
                        kv.read("org", tex.org);
                        kv.read("chg", tex.chg);
                        vidt.push_back(tex);

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                if (kv.read("col", "length", count)) {
                    std::vector<itm_table_item_data_col>& vidc = itm.data.col;

                    vidc.reserve(count);
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        itm_table_item_data_col col;
                        kv.read("tex", col.tex);
                        kv.read("flag", col.flag);

                        if (!(col.flag & 0x01)) {
                            vidc.push_back(col);
                            kv.close_scope();
                            continue;
                        }

                        kv.read("blend.0", col.col_tone.blend.x);
                        kv.read("blend.1", col.col_tone.blend.y);
                        kv.read("blend.2", col.col_tone.blend.z);
                        kv.read("offset.0", col.col_tone.offset.x);
                        kv.read("offset.1", col.col_tone.offset.y);
                        kv.read("offset.2", col.col_tone.offset.z);
                        kv.read("hue", col.col_tone.hue);
                        kv.read("saturation", col.col_tone.saturation);
                        kv.read("value", col.col_tone.value);
                        kv.read("contrast", col.col_tone.contrast);
                        kv.read("inverse", col.col_tone.inverse);
                        vidc.push_back(col);

                        kv.close_scope();
                    }
                    kv.close_scope();
                }
                kv.close_scope();
            }

            if (!kv.read("exclusion", itm.exclusion)
                || !kv.read("point", itm.point)
                || !kv.read("org_itm", itm.org_itm)) {
                kv.close_scope();
                continue;
            }

            kv.read("npr_flag", itm.npr_flag);
            kv.read("face_depth", itm.face_depth);
            vi.push_back(itm);

            kv.close_scope();
        }
        kv.close_scope();
    }
}

static void itm_table_write_text(itm_table* itm_tbl, void** data, size_t* size) {
    memory_stream s;
    s.open();

    key_val_out kv;
    if (itm_tbl->cos.size() > 0) {
        kv.open_scope("cos");

        int32_t count = (int32_t)itm_tbl->cos.size();
        itm_table_cos* vc = itm_tbl->cos.data();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, count);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < count; i++) {
            kv.open_scope_fmt(sort_index_data[i]);
            itm_table_cos* c = &vc[sort_index_data[i]];

            kv.write(s, "id",  c->id);

            kv.open_scope("item");

            int32_t count = (int32_t)c->item.size();
            int32_t* vci = c->item.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t j = 0; j < count; j++) {
                kv.open_scope_fmt(sort_index_data[j]);
                kv.write(s, vci[sort_index_data[j]]);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();

            kv.close_scope();
        }

        kv.write(s, "length", count);
        kv.close_scope();
    }

    if (itm_tbl->dbgset.size() > 0) {
        kv.open_scope("dbgset");

        int32_t count = (int32_t)itm_tbl->dbgset.size();
        itm_table_dbgset* vd = itm_tbl->dbgset.data();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, count);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < count; i++) {
            kv.open_scope_fmt(sort_index_data[i]);

            itm_table_dbgset* d = &vd[sort_index_data[i]];

            kv.open_scope("item");

            int32_t count = (int32_t)d->item.size();
            int32_t* vdi = d->item.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t j = 0; j < count; j++) {
                kv.open_scope_fmt(sort_index_data[j]);
                kv.write(s, vdi[sort_index_data[j]]);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.write(s, "name", d->name);
            kv.close_scope();

            kv.close_scope();
        }

        kv.write(s, "length", count);
        kv.close_scope();
    }

    if (itm_tbl->item.size() > 0) {
        kv.open_scope("item");

        int32_t count = (int32_t)itm_tbl->item.size();
        itm_table_item* vi = itm_tbl->item.data();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, count);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < count; i++) {
            kv.open_scope_fmt(sort_index_data[i]);

            itm_table_item* itm = &vi[sort_index_data[i]];
            kv.write(s, "attr", itm->attr);

            if (itm->data.col.size() || itm->data.obj.size()
                || itm->data.ofs.size() || itm->data.tex.size()) {
                kv.open_scope("data");

                if (itm->data.col.size()) {
                    kv.open_scope("col");

                    int32_t count = (int32_t)itm->data.col.size();
                    itm_table_item_data_col* vidc = itm-> data.col.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        itm_table_item_data_col* col = &vidc[sort_index_data[j]];
                        if (col->flag & 0x01) {
                            kv.write(s, "blend.0", col->col_tone.blend.x);
                            kv.write(s, "blend.1", col->col_tone.blend.y);
                            kv.write(s, "blend.2", col->col_tone.blend.z);
                            kv.write(s, "contrast", col->col_tone.contrast);
                        }

                        kv.write(s, "flag", col->flag);

                        if (col->flag & 0x01) {
                            kv.write(s, "hue", col->col_tone.hue);
                            kv.write(s, "inverse", col->col_tone.inverse ? 1 : 0);
                            kv.write(s, "offset.0", col->col_tone.offset.x);
                            kv.write(s, "offset.1", col->col_tone.offset.y);
                            kv.write(s, "offset.2", col->col_tone.offset.z);
                            kv.write(s, "saturation", col->col_tone.saturation);
                        }

                        kv.write(s, "tex", col->tex);

                        if (col->flag & 0x01)
                            kv.write(s, "value", col->col_tone.value);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                if (itm->data.obj.size()) {
                    kv.open_scope("obj");

                    int32_t count = (int32_t)itm->data.obj.size();
                    itm_table_item_data_obj* vido = itm->data.obj.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        itm_table_item_data_obj* obj = &vido[sort_index_data[j]];
                        kv.write(s, "rpk", obj->rpk);
                        kv.write(s, "uid", obj->uid);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                if (itm->data.ofs.size()) {
                    kv.open_scope("ofs");

                    int32_t count = (int32_t)itm->data.ofs.size();
                    itm_table_item_data_ofs* vido = itm->data.ofs.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        itm_table_item_data_ofs* ofs = &vido[sort_index_data[j]];
                        kv.write(s, "no", ofs->no);
                        kv.write(s, "rx", ofs->rotation.x);
                        kv.write(s, "ry", ofs->rotation.y);
                        kv.write(s, "rz", ofs->rotation.z);
                        kv.write(s, "sub_id", ofs->sub_id);
                        kv.write(s, "sx", ofs->scale.x);
                        kv.write(s, "sy", ofs->scale.y);
                        kv.write(s, "sz", ofs->scale.z);
                        kv.write(s, "tx", ofs->position.x);
                        kv.write(s, "ty", ofs->position.y);
                        kv.write(s, "tz", ofs->position.z);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                if (itm->data.tex.size()) {
                    kv.open_scope("tex");

                    int32_t count = (int32_t)itm->data.tex.size();
                    itm_table_item_data_tex* vidt = itm->data.tex.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        itm_table_item_data_tex* tex = &vidt[sort_index_data[j]];
                        kv.write(s, "chg", tex->chg);
                        kv.write(s, "org", tex->org);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                kv.close_scope();
            }

            kv.write(s, "des_id", itm->des_id);
            kv.write(s, "exclusion", itm->exclusion);
            kv.write(s, "face_depth", itm->face_depth);
            kv.write(s, "flag", itm->flag);
            kv.write(s, "name", itm->name);
            kv.write(s, "no", itm->no);
            kv.write(s, "npr_flag", itm->npr_flag);

            if (itm->objset.size()) {
                kv.open_scope("objset");

                int32_t count = (int32_t)itm->objset.size();
                std::string* vio = itm->objset.data();

                std::vector<int32_t> sort_index;
                key_val_out::get_lexicographic_order(sort_index, count);
                int32_t* sort_index_data = sort_index.data();
                for (int32_t j = 0; j < count; j++) {
                    kv.open_scope_fmt(sort_index_data[j]);
                    kv.write(s, vio[sort_index_data[j]]);
                    kv.close_scope();
                }

                kv.write(s, "length", count);
                kv.close_scope();
            }

            kv.write(s, "org_itm", itm->org_itm);
            kv.write(s, "point", itm->point);
            kv.write(s, "sub_id", itm->sub_id);
            kv.write(s, "type", itm->type);

            kv.close_scope();
        }

        kv.write(s, "length", count);
        kv.close_scope();
    }

    s.copy(data, size);
}