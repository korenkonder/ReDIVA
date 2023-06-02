/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/prj/vector_pair_combine.hpp"

struct customize_item {
    int32_t id;
    int32_t obj_id;
    int32_t sort_index;
    std::string name;
    int32_t chara;
    int32_t parts;

    customize_item();
    ~customize_item();
};

extern void customize_item_table_handler_data_init();
extern const customize_item* customize_item_table_handler_data_get_customize_item(int32_t id);
extern const prj::vector_pair_combine<int32_t, customize_item>&
    customize_item_table_handler_data_get_customize_items();
extern bool customize_item_table_handler_data_load();
extern void customize_item_table_handler_data_read();
extern void customize_item_table_handler_data_free();
