/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "rob.hpp"

extern void motion_set_load_motion(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db);
extern void motion_set_unload_motion(uint32_t set);

extern bool motion_storage_check_mot_file_not_ready(uint32_t set_id);
extern const mot_data* motion_storage_get_mot_data(uint32_t motion_id, const motion_database* mot_db);
extern float_t motion_storage_get_mot_data_frame_count(uint32_t motion_id, const motion_database* mot_db);
extern const mot_set* motion_storage_get_motion_set(uint32_t set_id);

extern void motion_init();
extern void motion_free();
