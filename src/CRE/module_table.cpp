/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "module_table.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/prj/vector_pair_combine.hpp"
#include "../KKdLib/key_val.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "mdata_manager.hpp"
#include <map>

struct module_table_handler {
    std::map<std::string, int32_t> name_map;
    std::list<p_file_handler*> file_handlers;
    bool ready;
    prj::vector_pair_combine<int32_t, module> modules;

    module_table_handler();
    ~module_table_handler();

    void clear();
    void fill_name_map();
    int32_t find_value(std::string& key);
    int32_t find_value(std::string&& key);
    const module* get_module(int32_t id);
    bool load();
    void parse(p_file_handler* pfhndl);
    void read();
};

struct module_data_handler {
    std::vector<module_data> modules;
    std::vector<module_data> random_modules;

    module_data_handler();
    ~module_data_handler();

    void add_all_modules();
    void add_modules();
    void add_random_modules();
    bool get_module(chara_index chara_index, int32_t cos, module_data& data);
    bool get_module(int32_t id, module_data& data);
};

struct module_data_random {
    int32_t id;
    int32_t sort_index;
    chara_index chara_index;
    int32_t spr_sel_mdrchrcmn_spr_set_id;
    int32_t spr_sel_mdrchrcmn_md_img_spr_id;
    const char* name;
};

module_data_random module_data_random_data[] = {
    {  -2,  0, CHARA_MIKU  , 1935, 35031, "オール ランダム" },
    {  -3,  1, CHARA_MIKU  , 1947, 35043, "初音ミク ランダム" },
    {  -4,  2, CHARA_RIN   , 1951, 35047, "鏡音リン ランダム" },
    {  -5,  3, CHARA_LEN   , 1941, 35037, "鏡音レン ランダム" },
    {  -6,  4, CHARA_LUKA  , 1943, 35039, "巡音ルカ ランダム" },
    {  -7,  8, CHARA_NERU  , 1949, 35045, "亞北ネル ランダム" },
    {  -8,  7, CHARA_HAKU  , 1937, 35033, "弱音ハク ランダム" },
    {  -9,  6, CHARA_KAITO , 1939, 35035, "カイト ランダム" },
    { -10,  5, CHARA_MEIKO , 1945, 35041, "メイコ ランダム" },
    { -11,  9, CHARA_SAKINE, 1953, 35049, "咲音メイコ ランダム" },
    { -12, 10, CHARA_TETO  , 2400, 35051, "重音テト ランダム" },
};

module_table_handler* module_table_handler_data;
module_data_handler* module_data_handler_data;

module::module() : sort_index(), chara(), cos() {
    id = -1;
}

module::~module() {

}

module_data::module_data() : id(), sort_index(), chara_index(), cos(), sleeve_l(), sleeve_r(),
spr_sel_md_id_spr_set_id(), spr_sel_md_id_cmn_spr_set_id(), spr_sel_md_id_md_img_id_spr_id(),
spr_sel_md_id_cmn_md_img_spr_id(), field_78(), field_79(), field_A0() {
    reset();
}

module_data::~module_data() {

}

void module_data::reset() {
    id = -1;
    sort_index = 0;
    chara_index = CHARA_MIKU;
    cos = 0;
    sleeve_l = {};
    sleeve_r = {};
    spr_sel_md_id_spr_set_id = -1;
    spr_sel_md_id_cmn_spr_set_id = -1;
    spr_sel_md_id_md_img_id_spr_id = -1;
    spr_sel_md_id_cmn_md_img_spr_id = -1;
    field_78 = false;
    field_79 = false;
    name.assign("");
    field_A0 = 0;
    field_A8 = prj::time::get_default();
    field_B0 = prj::time::get_default();
}

void module_table_handler_data_init() {
    if (!module_table_handler_data)
        module_table_handler_data = new module_table_handler;
}

const module* module_table_handler_data_get_module(int32_t id) {
    return module_table_handler_data->get_module(id);
}

const prj::vector_pair_combine<int32_t, module>& module_table_handler_data_get_modules() {
    return module_table_handler_data->modules;
}

bool module_table_handler_data_load() {
    return module_table_handler_data->load();
}

void module_table_handler_data_read() {
    module_table_handler_data->read();
}

void module_table_handler_data_free() {
    if (module_table_handler_data) {
        delete module_table_handler_data;
        module_table_handler_data = 0;
    }
}

void module_data_handler_data_init() {
    if (!module_data_handler_data)
        module_data_handler_data = new module_data_handler;
}

void module_data_handler_data_add_all_modules() {
    module_data_handler_data->add_all_modules();
}

bool module_data_handler_data_get_module(chara_index chara_index, int32_t cos, module_data& data) {
    return module_data_handler_data->get_module(chara_index, cos, data);
}

bool module_data_handler_data_get_module(int32_t id, module_data& data) {
    return module_data_handler_data->get_module(id, data);
}

const std::vector<module_data>& module_data_handler_data_get_modules() {
    return module_data_handler_data->modules;
}

void module_data_handler_data_free() {
    if (module_data_handler_data) {
        delete module_data_handler_data;
        module_data_handler_data = 0;
    }
}

module_table_handler::module_table_handler() : ready() {
    name_map.insert({ });
}

module_table_handler::~module_table_handler() {
    clear();
}

void module_table_handler::clear() {
    ready = false;
    modules.clear();

    for (p_file_handler*& i : file_handlers)
        if (i) {
            delete i;
            i = 0;
        }
    file_handlers.clear();
}

void module_table_handler::fill_name_map() {
    std::pair<const char*, int32_t> module_table_names[] = {
        { "MIKU"  , 0 },
        { "RIN"   , 1 },
        { "LEN"   , 2 },
        { "LUKA"  , 3 },
        { "NERU"  , 4 },
        { "HAKU"  , 5 },
        { "KAITO" , 6 },
        { "MEIKO" , 7 },
        { "SAKINE", 8 },
        { "TETO"  , 9 },
        { 0, 0 },
    };

    std::pair<const char*, int32_t>* name = module_table_names;
    while (name->first) {
        name_map.insert_or_assign(name->first, name->second);
        name++;
    }
}

int32_t module_table_handler::find_value(std::string& key) {
    auto elem = name_map.find(key);
    if (elem != name_map.end())
        return elem->second;
    return 9999;
}

int32_t module_table_handler::find_value(std::string&& key) {
    auto elem = name_map.find(key);
    if (elem != name_map.end())
        return elem->second;
    return 9999;
}

const module* module_table_handler::get_module(int32_t id) {
    auto elem = modules.find(id);
    if (elem != modules.end())
        return &elem->second;
    return 0;
}

bool module_table_handler::load() {
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

void module_table_handler::parse(p_file_handler* pfhndl) {
    key_val kv;
    kv.parse(pfhndl->get_data(), pfhndl->get_size());

    int32_t count;
    if (!kv.read("module", "data_list.length", count))
        return;

    for (int32_t i = 0; i < count; i++) {
        if (!kv.open_scope_fmt(i))
            continue;

        module module;

        if (kv.has_key("id"))
            kv.read("id", module.id);

        if (kv.has_key("sort_index"))
            kv.read("sort_index", module.sort_index);

        if (kv.has_key("name"))
            kv.read("name", module.name);

        if (kv.has_key("chara")) {
            const char* chara_str;
            kv.read("chara", chara_str);

            int32_t chara = find_value(chara_str);
            if (chara != 9999)
                module.chara = chara;
        }

        if (kv.has_key("cos")) {
            const char* cos_str;
            kv.read("cos", cos_str);

            int32_t cos = 0;
            if (sscanf_s(cos_str, "COS_%03d", &cos) == 1) {
                cos--;
                if (cos != 9999)
                    module.cos = cos;
            }
        }

        modules.push_back(module.id, module);

        kv.close_scope();
    }

    kv.close_scope();

    modules.combine();
}

void module_table_handler::read() {
    ready = false;

    fill_name_map();

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string farc_file;
        farc_file.assign(i);
        farc_file.append("gm_module_tbl.farc");

        if (aft_data->check_file_exists("rom/", farc_file.c_str())) {
            p_file_handler* pfhndl = new p_file_handler;
            pfhndl->read_file(aft_data, "rom/", farc_file.c_str(), "gm_module_id.bin", false);
            file_handlers.push_back(pfhndl);
        }
    }
}

module_data_handler::module_data_handler() {

}

module_data_handler::~module_data_handler() {

}

void module_data_handler::add_all_modules() {
    add_modules();
    add_random_modules();
}

void module_data_handler::add_modules() {
    modules.clear();

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    char buf[0x80];

    for (const auto& i : module_table_handler_data_get_modules()) {
        module_data mdl;
        mdl.id = i.second.id;
        mdl.sort_index = i.second.sort_index;
        mdl.name.assign(i.second.name);
        mdl.chara_index = (chara_index)i.second.chara;
        mdl.cos = i.second.cos;
        mdl.field_79 = false;

        rob_sleeve_handler_data_get_sleeve_data(mdl.chara_index, mdl.cos, mdl.sleeve_l, mdl.sleeve_r);

        sprintf_s(buf, sizeof(buf), "SPR_SEL_MD%03d", mdl.id);
        mdl.spr_sel_md_id_spr_set_id = aft_spr_db->get_spr_set_by_name(buf)->id;

        sprintf_s(buf, sizeof(buf), "SPR_SEL_MD%03dCMN", mdl.id);
        mdl.spr_sel_md_id_cmn_spr_set_id = aft_spr_db->get_spr_set_by_name(buf)->id;

        sprintf_s(buf, sizeof(buf), "SPR_SEL_MD%03d_MD_IMG_%03d", mdl.id, mdl.id);
        mdl.spr_sel_md_id_md_img_id_spr_id = aft_spr_db->get_spr_by_name(buf)->id;

        sprintf_s(buf, sizeof(buf), "SPR_SEL_MD%03dCMN_MD_IMG", mdl.id);
        mdl.spr_sel_md_id_cmn_md_img_spr_id = aft_spr_db->get_spr_by_name(buf)->id;

        /*struc_684 v17;
        if (sub_1402B7880()->sub_1402B7550(i.second.id, v17)) {
            mdl.field_78 = true;
            mdl.field_79 = v17.field_4;
            mdl.name.assign(v17.name);
            mdl.field_A0 = v17.field_28;
            mdl.field_A8 = v17.field_30;
            mdl.field_B0 = v17.field_38;
            mdl.sort_index = v17.sort_index;
        }*/

        modules.push_back(mdl);
    }
}

void module_data_handler::add_random_modules() {
    random_modules.clear();

    for (module_data_random& i : module_data_random_data) {
        module_data mdl;
        mdl.id = i.id;
        mdl.sort_index = i.sort_index;
        mdl.name.assign(i.name);
        mdl.chara_index = i.chara_index;
        mdl.cos = -1;
        mdl.field_79 = false;
        mdl.spr_sel_md_id_cmn_spr_set_id = i.spr_sel_mdrchrcmn_spr_set_id;
        mdl.spr_sel_md_id_cmn_md_img_spr_id = i.spr_sel_mdrchrcmn_md_img_spr_id;
        random_modules.push_back(mdl);
    }
}

bool module_data_handler::get_module(chara_index chara_index, int32_t cos, module_data& data) {
    for (module_data& i : modules)
        if (i.chara_index == chara_index && i.cos == cos) {
            data = i;
            return true;

        }
    return false;
}

bool module_data_handler::get_module(int32_t id, module_data& data) {
    for (module_data& i : modules)
        if (i.id == id) {
            data = i;
            return true;

        }

    for (module_data& i : random_modules)
        if (i.id == id) {
            data = i;
            return true;

        }

    return false;
}
