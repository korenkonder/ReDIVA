/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "item_table.hpp"
#include "../KKdLib/io/memory_stream.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "mdata_manager.hpp"

struct item_table_handler {
    chara_index chara_index;
    std::list<p_file_handler*> file_handlers;
    bool loaded;
    const char* file;
    item_table table;

    item_table_handler();
    ~item_table_handler();

    void clear();
    const item_table_item* get_item(int32_t item_no);
    bool load();
    void parse(p_file_handler* pfhndl);
    void read();
    void set_path(::chara_index chara_index);
};

static void item_table_load(data_struct* data, item_table& itm_tbl, itm_table& itm_tbl_file);

item_table_handler* item_table_handler_array;

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

const item_table_item* item_table::get_item(int32_t item_no) {
    auto elem = item.find(item_no);
    if (elem != item.end() && elem->second.type != -1)
        return &elem->second;
    return 0;
}

void item_table_handler_array_init() {
    item_table_handler_array = new item_table_handler[CHARA_MAX];
    for (int32_t i = CHARA_MIKU; i < CHARA_MAX; i++)
        item_table_handler_array[i].set_path((chara_index)i);
}

const item_cos_data* item_table_handler_array_get_item_cos_data_by_module_index(
    chara_index chara_index, int32_t module_index) {
    const item_table* table = item_table_handler_array_get_table(chara_index);
    if (!table)
        return 0;

    auto elem = table->cos.find(module_index);
    if (elem != table->cos.end())
        return &elem->second;
    else if (table->cos.size())
        return &table->cos.begin()->second;
    return 0;
}

const item_table_item* item_table_handler_array_get_item(
    chara_index chara_index, int32_t item_no) {
    if (chara_index >= CHARA_MIKU && chara_index < CHARA_MAX)
        return item_table_handler_array[chara_index].get_item(item_no);
    return 0;
}

const std::vector<uint32_t>* item_table_handler_array_get_item_objset(
    chara_index chara_index, int32_t item_no) {
    const item_table_item* item = item_table_handler_array_get_item(chara_index, item_no);
    if (item)
        return &item->objset;
    return 0;
}

const item_table* item_table_handler_array_get_table(chara_index chara_index) {
    if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX)
        return 0;
    return &item_table_handler_array[chara_index].table;
}

bool item_table_handler_array_load() {
    bool ret = false;
    for (int32_t i = CHARA_MIKU; i < CHARA_MAX; i++)
        ret |= item_table_handler_array[i].load();
    return ret;    
}

void item_table_handler_array_read() {
    for (int32_t i = CHARA_MIKU; i < CHARA_MAX; i++)
        item_table_handler_array[i].read();
}

void item_table_handler_array_free() {
    delete[] item_table_handler_array;
}

item_table_handler::item_table_handler() : file(), loaded() {
    chara_index = CHARA_NONE;
}

item_table_handler::~item_table_handler() {

}

void item_table_handler::clear() {
    for (p_file_handler*& i : file_handlers)
        if (i) {
            delete i;
            i = 0;
        }
    file_handlers.clear();
    loaded = false;
}

const item_table_item* item_table_handler::get_item(int32_t item_no) {
    return table.get_item(item_no);
}

bool item_table_handler::load() {
    if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX || loaded || !file)
        return false;

    for (p_file_handler*& i : file_handlers)
        if (i->check_not_ready())
            return true;

    for (p_file_handler*& i : file_handlers) {
        if (!i)
            continue;

        parse(i);

        delete i;
        i = 0;
    }
    file_handlers.clear();
    
    loaded = true;
    return false;
}

void item_table_handler::parse(p_file_handler* pfhndl) {
    itm_table itm;
    itm.read(pfhndl->get_data(), pfhndl->get_size());
    if (itm.ready)
        item_table_load(&data_list[DATA_AFT], table, itm);
}

void item_table_handler::read() {
    if (!file)
        return;

    for (p_file_handler*& i : file_handlers)
        if (i) {
            delete i;
            i = 0;
        }
    file_handlers.clear();

    loaded = false;

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string dir;
        dir.assign("rom/");
        dir.append(i);

        const char* farc_file = "chritm_prop.farc";

        if (!aft_data->check_file_exists(dir.c_str(), farc_file))
            continue;

        p_file_handler* pfhndl = new p_file_handler;
        pfhndl->read_file(aft_data, dir.c_str(), farc_file, file, false);
        file_handlers.push_back(pfhndl);
    }
}

void item_table_handler::set_path(::chara_index chara_index) {
    const char* item_table_paths[] = {
         "mikitm_tbl.txt",
         "rinitm_tbl.txt",
         "lenitm_tbl.txt",
         "lukitm_tbl.txt",
         "neritm_tbl.txt",
         "hakitm_tbl.txt",
         "kaiitm_tbl.txt",
         "meiitm_tbl.txt",
         "sakitm_tbl.txt",
         "tetitm_tbl.txt",
    };

    this->chara_index = chara_index;
    this->file = item_table_paths[chara_index];
}

static void item_table_load(data_struct* data, item_table& itm_tbl, itm_table& itm_tbl_file) {
    object_database* aft_obj_db = &data->data_ft.obj_db;
    texture_database* aft_tex_db = &data->data_ft.tex_db;

    for (itm_table_item& i : itm_tbl_file.item) {
        item_table_item itm;
        itm.flag = i.flag;
        itm.name = i.name;

        itm.objset.reserve(i.objset.size());
        for (std::string& j : i.objset) {
            uint32_t set_id = aft_obj_db->get_object_set_id(j.c_str());
            if (set_id != -1)
                itm.objset.push_back(set_id);
        }

        itm.type = i.type;
        itm.attr = i.attr;
        itm.des_id = i.des_id;
        itm.sub_id = (item_sub_id)i.sub_id;

        for (itm_table_item_data_obj& j : i.data.obj) {
            object_info obj_info;
            if (j.uid.compare("NULL")) {
                obj_info = aft_obj_db->get_object_info(j.uid.c_str());
                if (obj_info.is_null())
                    continue;
            }

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
            uint32_t org = aft_tex_db->get_texture_id(j.org.c_str());
            uint32_t chg = aft_tex_db->get_texture_id(j.chg.c_str());
            if (org == -1 || chg == -1)
                continue;

            item_table_item_data_tex tex;
            tex.org = org;
            tex.chg = chg;
            itm.data.tex.push_back(tex);
        }

        for (itm_table_item_data_col& j : i.data.col) {
            uint32_t tex_id = aft_tex_db->get_texture_id(j.tex.c_str());
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
        itm_tbl.item.insert_or_assign(i.no, itm);
    }

    for (itm_table_dbgset& i : itm_tbl_file.dbgset) {
        uint32_t set_id = aft_obj_db->get_object_set_id(i.name.c_str());
        if (set_id == -1)
            continue;

        item_table_dbgset dbg;
        dbg.item = i.item;
        itm_tbl.dbgset.insert_or_assign(set_id, dbg);
    }

    for (itm_table_cos& i : itm_tbl_file.cos) {
        if (!i.item.size())
            continue;

        item_cos_data cos = {};
        for (int32_t j : i.item) {
            const item_table_item* item = itm_tbl.get_item(j);
            if (item)
                cos.arr[item->sub_id] = j;
        }
        itm_tbl.cos.insert_or_assign(i.id, cos);
    }

    item_cos_data cos = {};
    itm_tbl.cos.insert_or_assign(499, cos);
}
