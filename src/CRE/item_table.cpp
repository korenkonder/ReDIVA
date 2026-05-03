/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "item_table.hpp"
#include "../KKdLib/io/memory_stream.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/str_utils.hpp"
#include "rob/rob.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "mdata_manager.hpp"

struct item_table_handler {
    CHARA_NUM chara_num;
    std::list<p_file_handler*> file_handlers;
    bool ready;
    const char* file;
    RobItemHeader table;

    item_table_handler();
    ~item_table_handler();

    void clear();
    const RobItemTable* get_item(uint32_t item_no);
    bool load();
    void parse(p_file_handler* pfhndl);
    void read();
    void set_path(CHARA_NUM chara_num);
};

static void item_table_load(data_struct* data, RobItemHeader& itm_tbl, itm_table& itm_tbl_file);

item_table_handler* item_table_handler_array;

RobItemDataObj::RobItemDataObj() : replace_id() {

}

RobItemDataOfs::RobItemDataOfs() : equip_sub_id(), item_no() {

}

RobItemDataTex::RobItemDataTex() {
    org_uid = -1;
    chg_uid = -1;
}

RobItemDataCol::RobItemDataCol() : flag() {
    tex_uid = -1;
}

RobItemData::RobItemData() {

}

RobItemData::~RobItemData() {

}

RobItemTable::RobItemTable() : flag(), type(), attr(), equip_des_id(),
equip_sub_id(), exclusion(), point(), org_itm(), npr_flag(), face_depth() {

}

RobItemTable::~RobItemTable() {

}

RobItemEquip::RobItemEquip() : item_no() {

}

item_table_dbgset::item_table_dbgset() {

}

item_table_dbgset::~item_table_dbgset() {

}

RobItemHeader::RobItemHeader() {

}

RobItemHeader::~RobItemHeader() {

}

// 0x14052BAF0
bool RobItemHeader::check_item(uint32_t item_no) const {
    auto elem = table.find(item_no);
    return elem != table.end();
}

const RobItemTable* RobItemHeader::get_item(uint32_t item_no) const {
    auto elem = table.find(item_no);
    if (elem != table.end() && elem->second.type != ROB_ITEM_TYPE_NONE)
        return &elem->second;
    return 0;
}

void item_table_handler_array_init() {
    item_table_handler_array = new item_table_handler[CN_MAX];
    for (int32_t i = 0; i < CN_MAX; i++)
        item_table_handler_array[i].set_path((CHARA_NUM)i);
}

const RobItemEquip* get_default_costume_data(CHARA_NUM cn, int32_t cos_id) {
    const RobItemHeader* tbl = get_rob_item_header(cn);
    if (!tbl)
        return 0;

    auto elem = tbl->defset.find(cos_id);
    if (elem != tbl->defset.end())
        return &elem->second;
    else if (tbl->defset.size())
        return &tbl->defset.begin()->second;
    return 0;
}

const RobItemTable* get_rob_item_table(CHARA_NUM cn, uint32_t item_no) {
    if (cn >= 0 && cn < CN_MAX)
        return item_table_handler_array[cn].get_item(item_no);
    return 0;
}

std::string get_rob_item_table_name(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl) {
        std::string name = RobItem::s_check_ng_item_name(cn, item_no);
        if (name.size())
            return name;
        return tbl->name;
    }
    return {};
}

const std::vector<uint32_t>* get_rob_item_table_objset(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return &tbl->objset;
    return 0;
}

const RobItemHeader* get_rob_item_header(CHARA_NUM cn) {
    if (cn < 0 || cn >= CN_MAX)
        return 0;
    return &item_table_handler_array[cn].table;
}

bool item_table_handler_array_load() {
    bool ret = false;
    for (int32_t i = 0; i < CN_MAX; i++)
        ret |= item_table_handler_array[i].load();
    return ret;
}

void item_table_handler_array_read() {
    for (int32_t i = 0; i < CN_MAX; i++)
        item_table_handler_array[i].read();
}

void item_table_handler_array_free() {
    delete[] item_table_handler_array;
}

item_table_handler::item_table_handler() : file(), ready() {
    chara_num = CN_NONE;
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
    ready = false;
}

const RobItemTable* item_table_handler::get_item(uint32_t item_no) {
    return table.get_item(item_no);
}

bool item_table_handler::load() {
    if (chara_num < 0 || chara_num >= CN_MAX || ready || !file)
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

    ready = true;
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

    ready = false;

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string farc_file(i);
        farc_file.append("chritm_prop.farc");

        if (aft_data->check_file_exists("rom/", farc_file.c_str())) {
            p_file_handler* pfhndl = new p_file_handler;
            pfhndl->read_file(aft_data, "rom/", farc_file.c_str(), file, prj::MemCTemp, false);
            file_handlers.push_back(pfhndl);
        }
    }
}

void item_table_handler::set_path(CHARA_NUM chara_num) {
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

    this->chara_num = chara_num;
    this->file = item_table_paths[chara_num];
}

static void item_table_load(data_struct* data, RobItemHeader& itm_tbl, itm_table& itm_tbl_file) {
    object_database* aft_obj_db = &data->data_ft.obj_db;
    texture_database* aft_tex_db = &data->data_ft.tex_db;

    for (itm_table_item& i : itm_tbl_file.item) {
        RobItemTable itm;
        itm.flag.w = i.flag.w;
        itm.name.assign(i.name);

        itm.objset.reserve(i.objset.size());
        for (std::string& j : i.objset) {
            uint32_t set_id = aft_obj_db->get_object_set_id(j.c_str());
            if (set_id != -1)
                itm.objset.push_back(set_id);
        }

        itm.type = i.type;
        itm.attr = i.attr;
        itm.equip_des_id = i.equip_des_id;
        itm.equip_sub_id = i.equip_sub_id;

        for (itm_table_item_data_obj& j : i.data.obj) {
            object_info uid;
            if (j.uid.compare("NULL")) {
                uid = aft_obj_db->get_object_info(j.uid.c_str());
                if (uid.is_null())
                    continue;
            }

            RobItemDataObj obj;
            obj.uid = uid;
            obj.replace_id = j.rpk;
            itm.data.obj.push_back(obj);
        }

        for (itm_table_item_data_ofs& j : i.data.ofs) {
            RobItemDataOfs ofs;
            ofs.equip_sub_id = j.sub_id;
            ofs.item_no = j.item_no;
            ofs.trans = j.trans;
            ofs.rot = j.rot;
            ofs.scale = j.scale;
            itm.data.ofs.push_back(ofs);
        }

        for (itm_table_item_data_tex& j : i.data.tex) {
            uint32_t org_uid = aft_tex_db->get_texture_id(j.org.c_str());
            uint32_t chg_uid = aft_tex_db->get_texture_id(j.chg.c_str());
            if (org_uid == -1 || chg_uid == -1)
                continue;

            RobItemDataTex tex;
            tex.org_uid = org_uid;
            tex.chg_uid = chg_uid;
            itm.data.tex.push_back(tex);
        }

        for (itm_table_item_data_col& j : i.data.col) {
            uint32_t tex_uid = aft_tex_db->get_texture_id(j.tex.c_str());
            if (tex_uid == -1)
                continue;

            RobItemDataCol col;
            col.tex_uid = tex_uid;
            col.flag.w = j.flag.w;
            col.color = j.color;
            itm.data.col.push_back(col);
        }

        itm.exclusion = i.exclusion;
        itm.point = i.point;
        itm.org_itm = i.org_itm;
        itm.npr_flag = i.npr_flag;
        itm.face_depth = i.face_depth;
        itm_tbl.table.push_back(i.no, itm);
    }

    itm_tbl.table.combine();

    for (itm_table_dbgset& i : itm_tbl_file.dbgset) {
        if (aft_obj_db->get_object_set_id(i.name.c_str()) == -1)
            continue;

        RobItemEquip equip = {};
        for (int32_t j : i.item) {
            const RobItemTable* tbl = itm_tbl.get_item(j);
            if (tbl)
                equip.item_no[tbl->equip_sub_id] = j;
        }

        itm_tbl.dbgset.push_back(i.name, equip);
    }

    itm_tbl.dbgset.combine();

    for (itm_table_cos& i : itm_tbl_file.cos) {
        if (!i.item.size())
            continue;

        RobItemEquip equip = {};
        for (int32_t j : i.item) {
            const RobItemTable* tbl = itm_tbl.get_item(j);
            if (tbl)
                equip.item_no[tbl->equip_sub_id] = j;
        }
        itm_tbl.defset.push_back(i.id, equip);
    }

    itm_tbl.defset.push_back(499, {});
    itm_tbl.defset.combine();
}
