/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/io/memory_stream.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data.hpp"
#include "item_table.hpp"

static bool item_table_array_read(data_struct* data, const char* path);
static item_table_item* item_table_get_item(item_table* itm_tbl, int32_t item_no);
static void item_table_load(data_struct* data, item_table* itm_tbl, itm_table* itm_tbl_file);

item_table* item_table_array;

void item_table_array_init() {
    item_table_array = new item_table[CHARA_MAX];
}

item_cos_data* item_table_array_get_item_cos_data_by_module_index(
    chara_index chara_index, int32_t module_index) {
    item_table* table = item_table_array_get_table(chara_index);
    if (!table)
        return 0;

    auto elem = table->cos.find(module_index);
    if (elem != table->cos.end())
        return &elem->second;
    else if (table->cos.size())
        return &table->cos.begin()->second;
    return 0;
}

item_table_item* item_table_array_get_item(chara_index chara_index, int32_t item_no) {
    if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX)
        return 0;
    return item_table_get_item(&item_table_array[chara_index], item_no);
}

void item_table_array_get_item_objset(chara_index chara_index,
    int32_t item_no, std::vector<uint32_t>** objset) {
    *objset = 0;
    item_table_item* item = item_table_array_get_item(chara_index, item_no);
    if (item)
        *objset = &item->objset;
}

item_table* item_table_array_get_table(chara_index chara_index) {
    if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX)
        return 0;
    return &item_table_array[chara_index];
}

bool item_table_array_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s;
    s.assign(path);
    s.append(file, file_len);

    return item_table_array_read((data_struct*)data, s.c_str());
}

void item_table_array_free() {
    delete[] item_table_array;
}

item_table_item_data_obj::item_table_item_data_obj() : rpk() {

}

item_table_item_data_ofs::item_table_item_data_ofs() : sub_id(), no() {

}

item_table_item_data_tex::item_table_item_data_tex() {
    org = -1;
    chg = -1;
}

item_table_item_data_col::item_table_item_data_col() : flag() {
    tex_id = -1;
}

item_table_item_data::item_table_item_data() {

}

item_table_item_data::~item_table_item_data() {

}

item_table_item::item_table_item() : flag(), type(), attr(), des_id(),
sub_id(), exclusion(), point(), org_itm(), npr_flag(), face_depth() {

}

item_table_item::~item_table_item() {

}

item_table_dbgset::item_table_dbgset() {

}

item_table_dbgset::~item_table_dbgset() {

}

item_table::item_table() {

}

item_table::~item_table() {

}

static bool item_table_array_read(data_struct* data, const char* path) {
    if (!path)
        return false;

    char* path_farc = str_utils_add(path, ".farc");
    if (!path_check_file_exists(path_farc)) {
        free_def(path_farc);
        return false;
    }

    farc f;
    f.read(path_farc, true, false);

    for (int32_t i = CHARA_MIKU; i < CHARA_MAX; i++) {
        const char* file = "";
        switch (i) {
        case CHARA_MIKU:
            file = "mikitm_tbl.txt";
            break;
        case CHARA_RIN:
            file = "rinitm_tbl.txt";
            break;
        case CHARA_LEN:
            file = "lenitm_tbl.txt";
            break;
        case CHARA_LUKA:
            file = "lukitm_tbl.txt";
            break;
        case CHARA_NERU:
            file = "neritm_tbl.txt";
            break;
        case CHARA_HAKU:
            file = "hakitm_tbl.txt";
            break;
        case CHARA_KAITO:
            file = "kaiitm_tbl.txt";
            break;
        case CHARA_MEIKO:
            file = "meiitm_tbl.txt";
            break;
        case CHARA_SAKINE:
            file = "sakitm_tbl.txt";
            break;
        case CHARA_TETO:
            file = "tetitm_tbl.txt";
            break;
        }

        farc_file* ff = f.read_file(file);
        if (ff) {
            memory_stream s;
            s.open(ff->data, ff->size);
            itm_table itm;
            itm.read(ff->data, ff->size);
            if (itm.ready)
                item_table_load(data, &item_table_array[i], &itm);
        }
    }
    free_def(path_farc);
    return true;
}

static item_table_item* item_table_get_item(item_table* itm_tbl_hndl, int32_t item_no) {
    auto elem = itm_tbl_hndl->item.find(item_no);
    if (elem != itm_tbl_hndl->item.end() && elem->second.type != -1)
        return &elem->second;
    return 0;
}

static void item_table_load(data_struct* data, item_table* itm_tbl, itm_table* itm_tbl_file) {
    for (itm_table_item& i : itm_tbl_file->item) {
        item_table_item itm;
        itm.flag = i.flag;
        itm.name = i.name;

        itm.objset.reserve(i.objset.size());
        for (std::string& j : i.objset) {
            uint32_t set_id = data->data_ft.obj_db.get_object_set_id(j.c_str());
            if (set_id != -1)
                itm.objset.push_back(set_id);
        }

        itm.type = i.type;
        itm.attr = i.attr;
        itm.des_id = i.des_id;
        itm.sub_id = (item_sub_id)i.sub_id;

        for (itm_table_item_data_obj& j : i.data.obj) {
            object_info obj_info = data->data_ft.obj_db.get_object_info(j.uid.c_str());
            if (obj_info.is_null())
                continue;

            item_table_item_data_obj obj;
            obj.obj_info = obj_info;
            obj.rpk = j.rpk;
            itm.data.obj.push_back(obj);
        }

        for (itm_table_item_data_ofs& j : i.data.ofs) {
            item_table_item_data_ofs ofs;
            ofs.sub_id = j.sub_id;
            ofs.no = j.no;
            ofs.position = j.position;
            ofs.rotation = j.rotation;
            ofs.scale = j.scale;
            itm.data.ofs.push_back(ofs);
        }

        for (itm_table_item_data_tex& j : i.data.tex) {
            uint32_t org = data->data_ft.tex_db.get_texture_id(j.org.c_str());
            uint32_t chg = data->data_ft.tex_db.get_texture_id(j.chg.c_str());
            if (org == -1 || chg == -1)
                continue;

            item_table_item_data_tex tex;
            tex.org = org;
            tex.chg = chg;
            itm.data.tex.push_back(tex);
        }

        for (itm_table_item_data_col& j : i.data.col) {
            uint32_t tex_id = data->data_ft.tex_db.get_texture_id(j.tex.c_str());
            if (tex_id == -1)
                continue;

            item_table_item_data_col col;
            col.tex_id = tex_id;
            col.flag = j.flag;
            col.col_tone = j.col_tone;
            itm.data.col.push_back(col);
        }

        itm.exclusion = i.exclusion;
        itm.point = i.point;
        itm.org_itm = i.org_itm;
        itm.npr_flag = i.npr_flag;
        itm.face_depth = i.face_depth;
        itm_tbl->item.insert_or_assign(i.no, itm);
    }

    for (itm_table_dbgset& i : itm_tbl_file->dbgset) {
        uint32_t set_id = data->data_ft.obj_db.get_object_set_id(i.name.c_str());
        if (set_id == -1)
            continue;

        item_table_dbgset dbg;
        dbg.item = i.item;
        itm_tbl->dbgset.insert_or_assign(set_id, dbg);
    }

    for (itm_table_cos& i : itm_tbl_file->cos) {
        if (!i.item.size())
            continue;

        item_cos_data cos = {};
        for (int32_t j : i.item) {
            item_table_item* item = item_table_get_item(itm_tbl, j);
            if (item)
                cos.arr[item->sub_id] = j;
        }
        itm_tbl->cos.insert_or_assign(i.id, cos);
    }

    item_cos_data cos = {};
    itm_tbl->cos.insert_or_assign(499, cos);
}
