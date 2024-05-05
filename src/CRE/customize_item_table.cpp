/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "customize_item_table.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/key_val.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "mdata_manager.hpp"
#include <map>

struct customize_item_table_handler {
    std::map<std::string, int32_t> name_map;
    std::list<p_file_handler*> file_handlers;
    bool ready;
    prj::vector_pair_combine<int32_t, customize_item> customize_items;

    customize_item_table_handler();
    ~customize_item_table_handler();

    void clear();
    void fill_name_map();
    int32_t find_value(std::string& key);
    int32_t find_value(std::string&& key);
    const customize_item* get_customize_item(int32_t id);
    bool load();
    void parse(p_file_handler* pfhndl);
    void read();
};

struct customize_item_data_handler {
    std::vector<customize_item_data> customize_items;

    customize_item_data_handler();
    ~customize_item_data_handler();

    void add_all_customize_items();
    void add_customize_items();
    bool get_customize_item(int32_t id, customize_item_data& data);
    int32_t get_customize_item_obj_id(int32_t id);
};

customize_item_table_handler* customize_item_table_handler_data;
customize_item_data_handler* customize_item_data_handler_data;

customize_item::customize_item() : sort_index(), chara() {
    id = -1;
    obj_id = -1;
    parts = -1;
}

customize_item::~customize_item() {

}

customize_item_data::customize_item_data() : id(), obj_id(), sort_index(), chara_index(), parts(),
spr_cmnitm_thmb_id_itm_img_spr_id(), field_3C(), field_3D(), field_40(), spr_cmnitm_thmb_id_spr_set_id() {
    reset();
}

customize_item_data::~customize_item_data() {

}

void customize_item_data::reset() {
    id = -1;
    obj_id = -1;
    sort_index = 0;
    name.assign("");
    chara_index = CHARA_MIKU;
    parts = -1;
    spr_cmnitm_thmb_id_itm_img_spr_id = -1;
    field_3C = false;
    field_3D = false;
    field_40 = 0;
    field_48 = prj::time::get_default();
    field_50 = prj::time::get_default();
    spr_cmnitm_thmb_id_spr_set_id = -1;
}

void customize_item_table_handler_data_init() {
    if (!customize_item_table_handler_data)
        customize_item_table_handler_data = new customize_item_table_handler;
}

const customize_item* customize_item_table_handler_data_get_customize_item(int32_t id) {
    return customize_item_table_handler_data->get_customize_item(id);
}

const prj::vector_pair_combine<int32_t, customize_item>& customize_item_table_handler_data_get_customize_items() {
    return customize_item_table_handler_data->customize_items;
}

bool customize_item_table_handler_data_load() {
    return customize_item_table_handler_data->load();
}

void customize_item_table_handler_data_read() {
    customize_item_table_handler_data->read();
}

void customize_item_table_handler_data_free() {
    if (customize_item_table_handler_data) {
        delete customize_item_table_handler_data;
        customize_item_table_handler_data = 0;
    }
}

void customize_item_data_handler_data_init() {
    if (!customize_item_data_handler_data)
        customize_item_data_handler_data = new customize_item_data_handler;
}

void customize_item_data_handler_data_add_all_customize_items() {
    customize_item_data_handler_data->add_all_customize_items();
}

bool customize_item_data_handler_data_get_customize_item(int32_t id, customize_item_data& data) {
    return customize_item_data_handler_data->get_customize_item(id, data);
}

int32_t customize_item_data_handler_data_get_customize_item_obj_id(int32_t id) {
    return customize_item_data_handler_data->get_customize_item_obj_id(id);
}

void customize_item_data_handler_data_free() {
    if (customize_item_data_handler_data) {
        delete customize_item_data_handler_data;
        customize_item_data_handler_data = 0;
    }
}

customize_item_table_handler::customize_item_table_handler() : ready() {
    name_map.insert({ });
}

customize_item_table_handler::~customize_item_table_handler() {
    clear();
}

void customize_item_table_handler::clear() {
    ready = false;
    customize_items.clear();

    for (p_file_handler*& i : file_handlers)
        if (i) {
            delete i;
            i = 0;
        }
    file_handlers.clear();
}

void customize_item_table_handler::fill_name_map() {
    std::pair<const char*, int32_t> customize_item_table_names[] = {
        { "MIKU"  ,  0 },
        { "RIN"   ,  1 },
        { "LEN"   ,  2 },
        { "LUKA"  ,  3 },
        { "NERU"  ,  4 },
        { "HAKU"  ,  5 },
        { "KAITO" ,  6 },
        { "MEIKO" ,  7 },
        { "SAKINE",  8 },
        { "TETO"  ,  9 },
        { "ALL"   , 10 },
        { "ZUJO"  , 0 },
        { "FACE"  , 1 },
        { "NECK"  , 2 },
        { "BACK"  , 3 },
        { 0, 0 },
    };

    std::pair<const char*, int32_t>* name = customize_item_table_names;
    while (name->first) {
        name_map.insert_or_assign(name->first, name->second);
        name++;
    }
}

int32_t customize_item_table_handler::find_value(std::string& key) {
    auto elem = name_map.find(key);
    if (elem != name_map.end())
        return elem->second;
    return 9999;
}

int32_t customize_item_table_handler::find_value(std::string&& key) {
    auto elem = name_map.find(key);
    if (elem != name_map.end())
        return elem->second;
    return 9999;
}

const customize_item* customize_item_table_handler::get_customize_item(int32_t id) {
    auto elem = customize_items.find(id);
    if (elem != customize_items.end())
        return &elem->second;
    return 0;
}

bool customize_item_table_handler::load() {
    if (ready)
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

void customize_item_table_handler::parse(p_file_handler* pfhndl) {
    key_val kv;
    kv.parse(pfhndl->get_data(), pfhndl->get_size());

    int32_t count;
    if (!kv.read("cstm_item", "data_list.length", count))
        return;

    for (int32_t i = 0; i < count; i++) {
        if (!kv.open_scope_fmt(i))
            continue;

        customize_item customize_item;

        if (kv.has_key("id"))
            kv.read("id", customize_item.id);

        if (kv.has_key("obj_id"))
            kv.read("obj_id", customize_item.obj_id);

        if (kv.has_key("sort_index"))
            kv.read("sort_index", customize_item.sort_index);

        if (kv.has_key("name"))
            kv.read("name", customize_item.name);

        if (kv.has_key("chara")) {
            const char* chara_str;
            kv.read("chara", chara_str);

            int32_t chara = find_value(chara_str);
            if (chara != 9999)
                customize_item.chara = chara;
        }

        if (kv.has_key("parts")) {
            const char* parts_str;
            kv.read("parts", parts_str);

            int32_t parts = find_value(parts_str);
            if (parts != 9999)
                customize_item.parts = parts;
        }

        customize_items.push_back(customize_item.id, customize_item);

        kv.close_scope();
    }

    kv.close_scope();

    customize_items.combine();
}

void customize_item_table_handler::read() {
    ready = false;

    fill_name_map();

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string farc_file(i);
        farc_file.append("gm_customize_item_tbl.farc");

        if (aft_data->check_file_exists("rom/", farc_file.c_str())) {
            p_file_handler* pfhndl = new p_file_handler;
            pfhndl->read_file(aft_data, "rom/", farc_file.c_str(), "gm_customize_item_id.bin", false);
            file_handlers.push_back(pfhndl);
        }
    }
}

customize_item_data_handler::customize_item_data_handler() {

}

customize_item_data_handler::~customize_item_data_handler() {

}

void customize_item_data_handler::add_all_customize_items() {
    add_customize_items();
}

void customize_item_data_handler::add_customize_items() {
    customize_items.clear();

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    char buf[0x80];

    for (const auto& i : customize_item_table_handler_data_get_customize_items()) {
        customize_item_data cstm_itm;
        cstm_itm.id = i.second.id;
        cstm_itm.obj_id = i.second.obj_id;
        cstm_itm.sort_index = i.second.sort_index;
        cstm_itm.name.assign(i.second.name);
        cstm_itm.chara_index = (chara_index)i.second.chara;
        cstm_itm.parts = i.second.parts;

        /*struc_781 v24;
        if (sub_1402B7880()->sub_1402B7420(cstm_itm.id, &v24)) {
            cstm_itm.field_3C = true;
            cstm_itm.field_3D = v24.field_4;
            cstm_itm.name.assign(v24.field_8);
            cstm_itm.field_40 = v24.field_28;
            cstm_itm.field_48.value = v24.field_30.value;
            cstm_itm.field_50.value = v24.field_38.value;
            cstm_itm.sort_index = v24.sort_index;
        }

        int32_t v23 = -1;
        if (sub_1402B7880->sub_1402B7510(cstm_itm.id, &v23))
            cstm_itm.field_3D = true;*/

        sprintf_s(buf, sizeof(buf), "SPR_CMNITM_THMB%03d", cstm_itm.id);
        cstm_itm.spr_cmnitm_thmb_id_spr_set_id = aft_spr_db->get_spr_set_by_name(buf)->id;

        sprintf_s(buf, sizeof(buf), "SPR_CMNITM_THMB%03d_ITM_IMG", cstm_itm.id);
        cstm_itm.spr_cmnitm_thmb_id_itm_img_spr_id = aft_spr_db->get_spr_by_name(buf)->id;

        customize_items.push_back(cstm_itm);
    }
}

bool customize_item_data_handler::get_customize_item(int32_t id, customize_item_data& data) {
    for (customize_item_data& i : customize_items)
        if (i.id == id) {
            data = i;
            return true;
        }
    return false;
}

int32_t customize_item_data_handler::get_customize_item_obj_id(int32_t id) {
    for (customize_item_data& i : customize_items)
        if (i.id == id)
            return i.obj_id;
    return -1;
}
