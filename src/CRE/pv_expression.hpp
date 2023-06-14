/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/pv_exp.hpp"
#include "file_handler.hpp"

extern void pv_expression_array_ctrl(void* rob_chr);
extern void pv_expression_array_reset();
extern void pv_expression_array_reset_data();
extern void pv_expression_array_reset_motion(size_t chara_id);
extern bool pv_expression_array_set(size_t chara_id, void* rob_chr, float_t frame_speed);
extern bool pv_expression_array_set_motion(const char* path, size_t chara_id, uint32_t motion_id);
extern bool pv_expression_array_set_motion(uint32_t hash, size_t chara_id, uint32_t motion_id);

extern bool pv_expression_file_check_not_ready(const char* path);
extern bool pv_expression_file_check_not_ready(uint32_t hash);
extern void pv_expression_file_load(void* data, const char* path);
extern void pv_expression_file_load(void* data, const char* path, uint32_t hash);
extern void pv_expression_file_unload(const char* path);
extern void pv_expression_file_unload(uint32_t hash);

extern void pv_expression_file_storage_init();
extern void pv_expression_file_storage_free();
