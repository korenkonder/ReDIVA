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
#include <vector>
#include "../KKdLib/dsc.h"
#include "../KKdLib/pvpp.h"
#include "../KKdLib/pvsr.h"
#include "../CRE/auth_3d.h"
#include "../CRE/rob.h"
#include "../CRE/task.h"
#include "classes.h"

struct x_pv_game_glitter {
    std::string name;
    uint32_t hash;

    x_pv_game_glitter(const char* name = 0);
    ~x_pv_game_glitter();
};

struct pv_play_data_set_motion {
    float_t frame_speed;
    int32_t motion_id;
    float_t frame;
    float_t duration;
    bool field_10;
    MotionBlendType blend_type;
    bool field_18;
    int32_t motion_index;
    int64_t dsc_time;
    float_t dsc_frame;
};

struct pv_play_data_motion {
    bool enable;
    int32_t motion_index;
    int64_t time;
};

struct dsc_set_item {
    int32_t time;
    int32_t item_index;
    int32_t pv_branch_mode;
};

struct dsc_set_motion {
    int32_t time;
    int32_t motion_index;
    int32_t pv_branch_mode;
};

struct struc_104 {
    rob_chara* rob_chr;
    float_t current_time;
    float_t duration;
    vec3 start_pos;
    vec3 end_pos;
    float_t start_rot;
    float_t end_rot;
    float_t mot_smooth_len;
    std::vector<dsc_set_motion> set_motion;
    std::vector<dsc_set_item> set_item;

    struc_104();
    ~struc_104();

    void reset();
};

struct pv_play_data {
    std::vector<pv_play_data_motion> motion;
    rob_chara* rob_chr;
    std::list<pv_play_data_set_motion> set_motion;
    bool disp;
    struc_104 field_38;

    pv_play_data();
    ~pv_play_data();

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

class x_pv_game : public TaskWindow {
public:
    int32_t pv_id;
    int32_t stage_id;
    pvpp* pp;
    pvsr* sr;

    uint32_t effpv_objset;
    uint32_t stgpv_objset;
    uint32_t stgpvhrc_objset;

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

    std::set<std::string> pv_auth_3d_names;
    std::set<std::string> stage_auth_3d_names;

    std::map<uint32_t, int32_t> pv_auth_3d_ids;
    std::map<uint32_t, int32_t> stage_auth_3d_ids;
    int32_t light_auth_3d_id;
    int32_t camera_auth_3d_id;

    dsc dsc_m;
    dsc_data* dsc_data_ptr;
    dsc_data* dsc_data_ptr_end;

    std::vector<x_pv_game_stage_effect_init> stage_effects;
    x_pv_game_stage_effect_init* stage_effects_ptr;
    x_pv_game_stage_effect_init* stage_effects_ptr_end;

    bool play;
    bool success;
    int32_t chara_id;
    float_t target_anim_fps;
    float_t anim_frame_speed;
    int64_t dsc_time;
    bool pv_end;
    pv_play_data playdata[ROB_CHARA_COUNT];
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

    void Load(int32_t pv_id, int32_t stage_id);
    void Unload();
};

extern x_pv_game x_pv_game_data;

#endif
