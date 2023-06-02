/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/prj/vector_pair_combine.hpp"

struct module {
    int32_t id;
    int32_t sort_index;
    std::string name;
    int32_t chara;
    int32_t cos;

    module();
    ~module();
};

extern void module_table_handler_data_init();
extern const module* module_table_handler_data_get_module(int32_t id);
extern const prj::vector_pair_combine<int32_t, module>& module_table_handler_data_get_modules();
extern bool module_table_handler_data_load();
extern void module_table_handler_data_read();
extern void module_table_handler_data_free();
