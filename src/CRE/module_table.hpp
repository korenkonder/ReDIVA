/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/prj/vector_pair_combine.hpp"
#include "../KKdLib/prj/time.hpp"
#include "rob/rob.hpp"

struct module {
    int32_t id;
    int32_t sort_index;
    std::string name;
    int32_t chara;
    int32_t cos;

    module();
    ~module();
};

struct module_data {
    int32_t id;
    int32_t sort_index;
    chara_index chara_index;
    int32_t cos;
    rob_sleeve_data sleeve_l;
    rob_sleeve_data sleeve_r;
    int32_t spr_sel_md_id_spr_set_id;
    int32_t spr_sel_md_id_cmn_spr_set_id;
    int32_t spr_sel_md_id_md_img_id_spr_id;
    int32_t spr_sel_md_id_cmn_md_img_spr_id;
    bool field_78;
    bool field_79;
    std::string name;
    int32_t field_A0;
    prj::time field_A8;
    prj::time field_B0;

    module_data();
    ~module_data();

    void reset();
};

extern void module_table_handler_data_init();
extern const module* module_table_handler_data_get_module(int32_t id);
extern const prj::vector_pair_combine<int32_t, module>& module_table_handler_data_get_modules();
extern bool module_table_handler_data_load();
extern void module_table_handler_data_read();
extern void module_table_handler_data_free();

extern void module_data_handler_data_init();
extern void module_data_handler_data_add_all_modules();
extern bool module_data_handler_data_get_module(chara_index chara_index, int32_t cos, module_data& data);
extern bool module_data_handler_data_get_module(int32_t id, module_data& data);
extern const std::vector<module_data>& module_data_handler_data_get_modules();
extern void module_data_handler_data_free();
