/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"

extern void pv_expression_array_ctrl(void* rob_chr);
extern void pv_expression_array_reset();
extern bool pv_expression_array_reset_data(size_t chara_id, void* rob_chr, float_t frame_speed);
extern void pv_expression_array_reset_motion(size_t chara_id);
extern bool pv_expression_array_set_motion(size_t chara_id, int32_t motion_id);

extern bool pv_expression_file_check_not_ready();
extern void pv_expression_file_load(void* data, const char* path, const char* file);
extern void pv_expression_file_load(void* data, const char* path, uint32_t hash);
extern void pv_expression_file_unload();
