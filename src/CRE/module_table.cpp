/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "module_table.hpp"
#include "../KKdLib/io/path.hpp"
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

module_table_handler* module_table_handler_data;

module::module() : sort_index(), chara(), cos() {
    id = -1;
}

module::~module() {

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
            int32_t cos;
            kv.read("cos", cos);
            cos--;
            if (cos != 9999)
                module.cos = cos;
        }

        modules.push_back({ module.id, module });

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
