/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "rob.hpp"

extern const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node);
extern bool osage_setting_data_obj_has_key(object_info key);

extern void skin_param_data_init();
extern void skin_param_data_load();
extern void skin_param_data_free();

extern bool skin_param_manager_array_check_task_ready();

extern bool skin_param_manager_add_task(int32_t chara_id, std::vector<osage_init_data>& vec);
extern bool skin_param_manager_check_task_ready(int32_t chara_id);
extern std::vector<skin_param_file_data>* skin_param_manager_get_skin_param_file_data(
    int32_t chara_id, object_info obj_info, int32_t motion_id, int32_t frame);
extern void skin_param_manager_reset(int32_t chara_id);
