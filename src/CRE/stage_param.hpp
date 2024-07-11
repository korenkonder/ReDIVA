/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include "../KKdLib/stage_param/fog_ring.hpp"
#include "../KKdLib/stage_param/leaf.hpp"
#include "../KKdLib/stage_param/litproj.hpp"
#include "../KKdLib/stage_param/rain.hpp"
#include "../KKdLib/stage_param/ripple.hpp"
#include "../KKdLib/stage_param/snow.hpp"
#include "../KKdLib/stage_param/splash.hpp"
#include "../KKdLib/stage_param/star.hpp"

extern void stage_param_data_init();
extern void stage_param_data_free();

extern void stage_param_data_coli_data_get_mhd_id_data(
    const int32_t& mhd_id, void* ring); // osage_ring_data
extern int32_t stage_param_data_coli_data_get_pv_id();
extern void stage_param_data_coli_data_get_stage_index_data(
    const int32_t& stage_index, void* ring); // osage_ring_data
extern void stage_param_data_coli_data_get_stage_wind_file_path(std::string& path, const int32_t& stage_index);
extern void stage_param_data_coli_data_load(const int32_t& pv_id);
extern void stage_param_data_coli_data_reset();

extern void stage_param_data_fog_ring_storage_clear();
extern stage_param_fog_ring* stage_param_data_fog_ring_storage_get_value(int32_t stage_index);
extern void stage_param_data_fog_ring_storage_set_stage_data(int32_t stage_index, stage_param_fog_ring* value);

extern void stage_param_data_leaf_storage_clear();
extern stage_param_leaf* stage_param_data_leaf_storage_get_value(int32_t stage_index);
extern void stage_param_data_leaf_storage_set_stage_data(int32_t stage_index, stage_param_leaf* value);

extern void stage_param_data_litproj_storage_clear();
extern stage_param_litproj* stage_param_data_litproj_storage_get_value(int32_t stage_index);
extern void stage_param_data_litproj_storage_set_stage_data(int32_t stage_index, stage_param_litproj* value);

extern void stage_param_data_rain_storage_clear();
extern stage_param_rain* stage_param_data_rain_storage_get_value(int32_t stage_index);
extern void stage_param_data_rain_storage_set_stage_data(int32_t stage_index, stage_param_rain* value);

extern void stage_param_data_ripple_storage_clear();
extern stage_param_ripple* stage_param_data_ripple_storage_get_value(int32_t stage_index);
extern void stage_param_data_ripple_storage_set_stage_data(int32_t stage_index, stage_param_ripple* value);

extern void stage_param_data_snow_storage_clear();
extern stage_param_snow* stage_param_data_snow_storage_get_value(int32_t stage_index);
extern void stage_param_data_snow_storage_set_stage_data(int32_t stage_index, stage_param_snow* value);

extern void stage_param_data_splash_storage_clear();
extern stage_param_splash* stage_param_data_splash_storage_get_value(int32_t stage_index);
extern void stage_param_data_splash_storage_set_stage_data(int32_t stage_index, stage_param_splash* value);

extern void stage_param_data_star_storage_clear();
extern stage_param_star* stage_param_data_star_storage_get_value(int32_t stage_index);
extern void stage_param_data_star_storage_set_stage_data(int32_t stage_index, stage_param_star* value);
