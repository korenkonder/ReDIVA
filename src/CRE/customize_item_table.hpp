/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/prj/time.hpp"
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

struct customize_item_data {
    int32_t id;
    int32_t obj_id;
    int32_t sort_index;
    std::string name;
    chara_index chara_index;
    int32_t parts;
    int32_t spr_cmnitm_thmb_id_itm_img_spr_id;
    bool field_3C;
    bool field_3D;
    int32_t field_40;
    prj::time field_48;
    prj::time field_50;
    int32_t spr_cmnitm_thmb_id_spr_set_id;

    customize_item_data();
    ~customize_item_data();

    void reset();
};

extern void customize_item_table_handler_data_init();
extern const customize_item* customize_item_table_handler_data_get_customize_item(int32_t id);
extern const prj::vector_pair_combine<int32_t, customize_item>&
    customize_item_table_handler_data_get_customize_items();
extern bool customize_item_table_handler_data_load();
extern void customize_item_table_handler_data_read();
extern void customize_item_table_handler_data_free();

extern void customize_item_data_handler_data_init();
extern void customize_item_data_handler_data_add_all_customize_items();
extern void customize_item_table_handler_data_get_chara_item(
    const std::string& name, chara_index& chara_index, int32_t& item_no);
extern bool customize_item_data_handler_data_get_customize_item(int32_t id, customize_item_data& data);
extern int32_t customize_item_data_handler_data_get_customize_item_obj_id(int32_t id);
extern void customize_item_data_handler_data_free();
