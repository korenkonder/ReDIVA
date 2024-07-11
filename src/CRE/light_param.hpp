/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include "../KKdLib/default.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/light_param/face.hpp"
#include "../KKdLib/light_param/fog.hpp"
#include "../KKdLib/light_param/glow.hpp"
#include "../KKdLib/light_param/ibl.hpp"
#include "../KKdLib/light_param/light.hpp"
#include "../KKdLib/light_param/wind.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include <glad/glad.h>

enum light_param_data_storage_flags {
    LIGHT_PARAM_DATA_STORAGE_LIGHT = 0x01,
    LIGHT_PARAM_DATA_STORAGE_FOG   = 0x02,
    LIGHT_PARAM_DATA_STORAGE_GLOW  = 0x04,
    LIGHT_PARAM_DATA_STORAGE_IBL   = 0x08,
    LIGHT_PARAM_DATA_STORAGE_WIND  = 0x10,
    LIGHT_PARAM_DATA_STORAGE_FACE  = 0x20,
    LIGHT_PARAM_DATA_STORAGE_ALL   = 0x3F,
};

extern void light_param_data_storage_data_init();
extern void light_param_data_storage_data_free_file_handlers();
extern std::string light_param_data_storage_data_get_name();
extern int32_t light_param_data_storage_data_get_pv_id();
extern int32_t light_param_data_storage_data_get_stage_index();
extern void light_param_data_storage_data_load();
extern int32_t light_param_data_storage_data_load_file();
extern void light_param_data_storage_data_load_stage(int32_t stage_index);
extern void light_param_data_storage_data_load_stage(uint32_t stage_hash, stage_database* stage_data);
extern void light_param_data_storage_data_load_stages(std::vector<int32_t>& stage_indices);
extern void light_param_data_storage_data_load_stages(
    std::vector<uint32_t>& stage_hashes, stage_database* stage_data);
extern void light_param_data_storage_data_reset();
extern void light_param_data_storage_data_set_default_light_param(
    light_param_data_storage_flags flags = LIGHT_PARAM_DATA_STORAGE_ALL);
extern void light_param_data_storage_data_set_ibl();
extern void light_param_data_storage_data_set_pv_id(int32_t pv_id);
extern void light_param_data_storage_data_set_pv_cut(int32_t cut_id,
    light_param_data_storage_flags flags = LIGHT_PARAM_DATA_STORAGE_ALL);
extern void light_param_data_storage_data_set_stage(int32_t stage_id,
    light_param_data_storage_flags flags = LIGHT_PARAM_DATA_STORAGE_ALL);
extern void light_param_data_storage_data_unload();
extern void light_param_data_storage_data_free();
