/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"

struct pv_game_camera_dsc_data {
    vec3 view_point;
    vec3 interest;
    vec3 up_vec;

    pv_game_camera_dsc_data();
};

extern void pv_game_camera_ctrl(float_t delta_time);
extern void pv_game_camera_reset();
extern void pv_game_camera_set_dsc_data(float_t duration, pv_game_camera_dsc_data& start,
    pv_game_camera_dsc_data& end, float_t acceleration_1, float_t acceleration_2,
    int32_t follow_chara, int32_t chara_id, int32_t chara_follow_point, bool edit_camera);
extern void pv_game_camera_set_fov_min_dist(float_t fov, float_t min_dist);
