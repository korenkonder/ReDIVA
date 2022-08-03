/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/pv_exp.hpp"
#include "file_handler.hpp"

struct pv_expression_file {
    p_file_handler file_handler;
    pv_exp* data;
    int32_t load_count;
    uint32_t hash;

    pv_expression_file();
    ~pv_expression_file();

    static void load_file(pv_expression_file* handler, const void* data, size_t size);
};

extern void pv_expression_array_ctrl(void* rob_chr);
extern void pv_expression_array_reset();
extern bool pv_expression_array_reset_data(size_t chara_id, void* rob_chr, float_t frame_speed);
extern void pv_expression_array_reset_motion(size_t chara_id);
extern bool pv_expression_array_set_motion(const char* file, size_t chara_id, int32_t motion_id);
extern bool pv_expression_array_set_motion(uint32_t hash, size_t chara_id, int32_t motion_id);

extern bool pv_expression_file_check_not_ready(const char* file);
extern bool pv_expression_file_check_not_ready(uint32_t hash);
extern void pv_expression_file_load(void* data, const char* path, const char* file);
extern void pv_expression_file_load(void* data, const char* path, uint32_t hash);
extern void pv_expression_file_unload(const char* file);
extern void pv_expression_file_unload(uint32_t hash);

extern void pv_expression_file_storage_init();
extern void pv_expression_file_storage_free();
