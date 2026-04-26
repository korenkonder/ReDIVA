/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <string>

extern void effchrpv_auth_3d_to_mot_init(int32_t pv_id, const enum ROB_ID* rob_ids);
extern void effchrpv_auth_3d_to_mot_add_chara_effect_auth_3d(uint32_t hash, int32_t id);
extern void effchrpv_auth_3d_to_mot_add_file_name(const std::string& file, const std::string& category);
extern void effchrpv_auth_3d_to_mot_disp();
extern void effchrpv_auth_3d_to_mot_get_body_anim(int32_t frame, bool add_keys);
extern void effchrpv_auth_3d_to_mot_get_hand_anim(int32_t frame);
extern void effchrpv_auth_3d_to_mot_load(int32_t frame);
extern void effchrpv_auth_3d_to_mot_modify_play_param(void* play_param);
extern void effchrpv_auth_3d_to_mot_post_modify_play_param();
extern void effchrpv_auth_3d_to_mot_write_mot_set();
extern void effchrpv_auth_3d_to_mot_free();
