/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/database/item_table.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"

struct hand_item {
    object_info obj_left;
    object_info obj_right;
    std::string item_str;
    std::string item_name;
    uint32_t file_size;
    uint32_t hand_mottbl_index;
    float_t hand_scale;
    int32_t uid;

    hand_item();
    ~hand_item();
};

extern void hand_item_handler_data_init();
extern const hand_item* hand_item_handler_data_get_hand_item(int32_t uid, chara_index chara_index);
extern const std::map<std::pair<int32_t, chara_index>, hand_item>& hand_item_handler_data_get_hand_items();
extern int32_t hand_item_handler_data_get_hand_item_uid(const char* str);
extern bool hand_item_handler_data_load();
extern void hand_item_handler_data_read();
extern void hand_item_handler_data_free();
