/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "rob.hpp"
#include "../../KKdLib/key_val.hpp"

extern const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node);
extern bool osage_setting_data_obj_has_key(object_info key);

extern void skin_param_data_init();
extern void skin_param_data_load();
extern void skin_param_data_free();

extern bool skin_param_manager_array_check_task_ready();

extern bool skin_param_manager_add_task(int32_t chara_id, std::vector<osage_init_data>& vec);
extern bool skin_param_manager_check_task_ready(int32_t chara_id);
extern int32_t skin_param_manager_get_ext_skp_file(
    int32_t chara_id, const osage_init_data& osage_init, const std::string& dir,
    std::string& file, void* data, const motion_database* mot_db);
extern std::vector<skin_param_file_data>* skin_param_manager_get_skin_param_file_data(
    int32_t chara_id, object_info obj_info, uint32_t motion_id, int32_t frame);
extern void skin_param_manager_reset(int32_t chara_id);

extern void skin_param_osage_node_parse(void* kv, const char* name,
    std::vector<skin_param_osage_node>* a3, const skin_param_osage_root& skp_root);

extern void skin_param_osage_root_parse(void* kv, const char* name,
    skin_param_osage_root& skp_root, const bone_database* bone_data);

extern key_val* skin_param_storage_get_key_val(object_info obj_info);
extern void skin_param_storage_load(std::vector<object_info>& obj_infos,
    void* data, const object_database* obj_db);
extern void skin_param_storage_reset();
