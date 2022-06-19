/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(CLOUD_DEV)
#pragma once

#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "../KKdLib/dsc.hpp"
#include "../KKdLib/pvpp.hpp"
#include "../KKdLib/pvsr.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/task.hpp"
#include "classes.hpp"

struct x_pv_game_glitter {
    std::string name;
    uint32_t hash;

    x_pv_game_glitter(const char* name = 0);
    ~x_pv_game_glitter();
};

struct x_pv_play_data_set_motion {
    float_t frame_speed;
    int32_t motion_id;
    float_t frame;
    float_t duration;
    bool field_10;
    MotionBlendType blend_type;
    bool disable_eye_motion;
    int32_t motion_index;
    int64_t dsc_time;
    float_t dsc_frame;
};

struct x_pv_play_data_motion {
    bool enable;
    int32_t motion_index;
    int64_t time;
};

struct x_dsc_set_item {
    int32_t time;
    int32_t item_index;
    int32_t pv_branch_mode;
};

struct x_dsc_set_motion {
    int32_t time;
    int32_t motion_index;
    int32_t pv_branch_mode;
};

struct x_struc_104 {
    rob_chara* rob_chr;
    float_t current_time;
    float_t duration;
    vec3 start_pos;
    vec3 end_pos;
    float_t start_rot;
    float_t end_rot;
    float_t mot_smooth_len;
    std::vector<x_dsc_set_motion> set_motion;
    std::vector<x_dsc_set_item> set_item;

    x_struc_104();
    ~x_struc_104();

    void reset();
};

struct x_pv_play_data {
    std::vector<x_pv_play_data_motion> motion;
    rob_chara* rob_chr;
    std::list<x_pv_play_data_set_motion> set_motion;
    bool disp;
    x_struc_104 field_38;

    x_pv_play_data();
    ~x_pv_play_data();

    void reset();
};

struct x_pv_game_stage_effect {
    float_t frame;
    float_t last_frame;
    int32_t prev_stage_effect;
    int32_t stage_effect;
    int32_t next_stage_effect;
};

struct x_pv_game_stage_effect_init {
    int32_t frame;
    int32_t effect_id;
    int32_t bar_count;
    int32_t bar;
};

struct x_pv_game_a3da_to_mot_keys {
    std::vector<float_t> x;
    std::vector<float_t> y;
    std::vector<float_t> z;

    x_pv_game_a3da_to_mot_keys();
    ~x_pv_game_a3da_to_mot_keys();
};

struct x_pv_game_a3da_to_mot {
    int32_t auth_3d_id;
    int32_t gblctr;
    int32_t n_hara;
    int32_t n_hara_y;
    int32_t j_hara_wj;
    int32_t n_kosi;
    int32_t j_mune_wj;
    int32_t n_mune_kl;
    int32_t j_mune_b_wj;
    int32_t j_kubi_wj;
    int32_t n_kao;
    int32_t j_kao_wj;
    int32_t j_kao_normal_wj;
    int32_t j_kao_close_wj;
    int32_t j_kao_smile_wj;
    int32_t j_kao_close_half_wj;
    int32_t j_kao_smile_half_wj;
    int32_t j_kuti_l_wj;
    int32_t j_kuti_u_wj;
    int32_t j_kuti_d_wj;
    int32_t j_kuti_r_wj;
    int32_t j_eye_r_wj;
    int32_t j_eye_l_wj;
    int32_t n_waki_l;
    int32_t j_waki_l_wj;
    int32_t n_kata_l;
    int32_t j_kata_l_wj;
    int32_t j_ude_l_wj;
    int32_t j_te_l_wj;
    int32_t j_te_sizen2_l_wj;
    int32_t j_te_close_l_wj;
    int32_t j_te_reset_l_wj;
    int32_t n_waki_r;
    int32_t j_waki_r_wj;
    int32_t n_kata_r;
    int32_t j_kata_r_wj;
    int32_t j_ude_r_wj;
    int32_t j_te_r_wj;
    int32_t j_te_sizen2_r_wj;
    int32_t j_te_close_r_wj;
    int32_t j_te_reset_r_wj;
    int32_t j_te_one_r_wj;
    int32_t j_kosi_wj;
    int32_t n_momo_l;
    int32_t j_momo_l_wj;
    int32_t j_sune_l_wj;
    int32_t j_asi_l_wj;
    int32_t n_momo_r;
    int32_t j_momo_r_wj;
    int32_t j_sune_r_wj;
    int32_t j_asi_r_wj;
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys> bone_keys;
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys> sec_bone_keys;

    x_pv_game_a3da_to_mot(int32_t auth_3d_id);
    ~x_pv_game_a3da_to_mot();

    void get_bone_indices(auth_3d_object_hrc* oh);
};

class x_pv_game : public app::TaskWindow {
public:
    int32_t pv_id;
    int32_t stage_id;
    pvpp* pp;
    pvsr* sr;

    uint32_t effpv_objset;
    uint32_t stgpv_objset;
    uint32_t stgpvhrc_objset;

    std::vector<uint32_t> objset_load;

    int32_t state;
    int32_t frame;
    double_t frame_float;
    int64_t time;
    int32_t rob_chara_ids[ROB_CHARA_COUNT];

    x_pv_game_glitter* pv_glitter;
    x_pv_game_glitter* stage_glitter;

    object_database obj_db;
    texture_database tex_db;
    stage_database stage_data;

    std::vector<uint32_t> stage_hashes;
    std::vector<stage_data_modern*> stages_data;

    std::string pv_category;
    std::string stage_category;
    std::string light_category;
    std::string camera_category;

    uint32_t pv_category_hash;
    uint32_t stage_category_hash;
    uint32_t camera_category_hash;

    std::vector<std::pair<std::string, uint32_t>> category_load;
    std::vector<std::pair<std::string, uint32_t>> effchrpv_category_load;

    std::set<std::string> pv_auth_3d_names;
    std::set<std::string> stage_auth_3d_names;
    std::unordered_map<std::string, std::pair<uint32_t, std::string>> effchrpv_auth_3d_names;
    std::unordered_map<std::string, std::pair<uint32_t, std::string>> effchrpv_auth_3d_mot_names;

    std::map<uint32_t, int32_t> pv_auth_3d_ids;
    std::map<uint32_t, int32_t> stage_auth_3d_ids;
    int32_t light_auth_3d_id;
    int32_t camera_auth_3d_id;
    std::map<uint32_t, int32_t> effchrpv_auth_3d_ids;
    std::map<uint32_t, int32_t> effchrpv_auth_3d_mot_ids;

    std::vector<int32_t> effchrpv_rob_mot_ids;
    std::map<int32_t, x_pv_game_a3da_to_mot> effchrpv_auth_3d_rob_mot_ids;

    std::map<uint32_t, uint32_t> effchrpv_auth_3d_hashes;

    dsc dsc_m;
    dsc_data* dsc_data_ptr;
    dsc_data* dsc_data_ptr_end;

    std::vector<x_pv_game_stage_effect_init> stage_effects;
    x_pv_game_stage_effect_init* stage_effects_ptr;
    x_pv_game_stage_effect_init* stage_effects_ptr_end;

    std::map<int32_t, std::set<int32_t>> pv_auth_3d_chara_count;

    bool play;
    bool success;
    int32_t chara_id;
    float_t target_anim_fps;
    float_t anim_frame_speed;
    int64_t dsc_time;
    bool pv_end;
    x_pv_play_data playdata[ROB_CHARA_COUNT];
    float_t scene_rot_y;
    mat4u scene_rot_mat;
    int32_t branch_mode;

    int64_t prev_bar_point_time;
    int32_t prev_bpm;

    bool pause;
    bool step_frame;

    x_pv_game_stage_effect stage_effect;

    x_pv_game();
    virtual ~x_pv_game() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Window() override;

    void Load(int32_t pv_id, int32_t stage_id, chara_index charas[6], int32_t modules[6]);
    void Unload();
};

extern x_pv_game x_pv_game_data;

#endif
