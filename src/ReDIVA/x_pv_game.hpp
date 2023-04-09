/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(ReDIVA_DEV)
#pragma once

#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "config.hpp"
#include "../CRE/Glitter/glitter.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/frame_rate_control.hpp"
#include "../CRE/stage_modern.hpp"
#include "../CRE/task.hpp"
#include "../KKdLib/post_process_table/dof.hpp"
#include "../KKdLib/dsc.hpp"
#include "../KKdLib/pvpp.hpp"
#include "../KKdLib/pvsr.hpp"
#include "task_window.hpp"

struct x_pv_bar_beat_data {
    int32_t bar;
    float_t bar_time;
    int32_t beat_count;
    int32_t beat_counter;
    float_t beat_time[16];

    bool compare_bar_time_less(float_t time);
};

struct x_pv_bar_beat {
    float_t curr_time;
    float_t delta_time;
    float_t next_bar_time;
    float_t curr_bar_time;
    float_t next_beat_time;
    float_t curr_beat_time;
    float_t divisor;
    int32_t bar;
    int32_t counter;
    std::vector<x_pv_bar_beat_data> data;

    x_pv_bar_beat();
    ~x_pv_bar_beat();

    float_t get_bar_current_frame();
    float_t get_delta_frame();
    int32_t get_bar_beat_from_time(int32_t* beat, float_t time);
    float_t get_next_beat_time(float_t time);
    float_t get_time_from_bar_beat(int32_t bar, int32_t beat);
    void reset();
    void reset_time();
    void set_frame(float_t curr_frame, float_t delta_frame);
    void set_time(float_t curr_time, float_t delta_time);
};

class BPMFrameRateControl : public FrameRateControl {
public:
    x_pv_bar_beat* bar_beat;

    BPMFrameRateControl();
    virtual ~BPMFrameRateControl() override;

    virtual float_t GetDeltaFrame() override;

    void Reset();
};


class XPVFrameRateControl : public FrameRateControl {
public:
    XPVFrameRateControl();
    virtual ~XPVFrameRateControl() override;

    virtual float_t GetDeltaFrame() override;

    void Reset();
};

struct x_pv_play_data_set_motion {
    float_t frame_speed;
    int32_t motion_id;
    float_t frame;
    float_t blend_duration;
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

struct x_pv_play_data_motion_data {
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

    x_pv_play_data_motion_data();
    ~x_pv_play_data_motion_data();

    void reset();
};

struct x_pv_play_data {
    std::vector<x_pv_play_data_motion> motion;
    rob_chara* rob_chr;
    std::list<x_pv_play_data_set_motion> set_motion;
    bool disp;
    x_pv_play_data_motion_data motion_data;

    x_pv_play_data();
    ~x_pv_play_data();

    void reset();
};

#if BAKE_PV826
struct x_pv_game_a3da_to_mot_keys {
    std::vector<float_t> x;
    std::vector<float_t> y;
    std::vector<float_t> z;

    x_pv_game_a3da_to_mot_keys();
    ~x_pv_game_a3da_to_mot_keys();
};

struct x_pv_game_a3da_to_mot {
    auth_3d_id id;
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

    x_pv_game_a3da_to_mot(auth_3d_id id);
    ~x_pv_game_a3da_to_mot();

    void get_bone_indices(auth_3d_object_hrc* oh);
};
#endif

struct x_pv_game_song_effect_auth_3d {
    auth_3d_id id;

    x_pv_game_song_effect_auth_3d();
    x_pv_game_song_effect_auth_3d(auth_3d_id id);

    void reset();
};

struct x_pv_game_song_effect_glitter {
    string_hash name;
    Glitter::SceneCounter scene_counter;
    bool field_8;
    bool field_9[ROB_CHARA_COUNT];

    x_pv_game_song_effect_glitter();

    void reset();
};

struct x_pv_game_song_effect {
    bool enable;
    int32_t chara_id;
    int64_t time;
    std::vector<x_pv_game_song_effect_auth_3d> auth_3d;
    std::vector<x_pv_game_song_effect_glitter> glitter;

    x_pv_game_song_effect();
    ~x_pv_game_song_effect();

    void reset();
};

struct x_pv_game_camera {
    int32_t state;
    string_hash category;
    string_hash file;
    auth_3d_id id;
    FrameRateControl* frame_rate_control;

    x_pv_game_camera();
    ~x_pv_game_camera();

    void ctrl(float_t curr_time);
    void load(int32_t pv_id, int32_t stage_id, FrameRateControl* frame_rate_control);
    void load_data();
    void reset();
    void stop();
    void unload();
};

struct x_pv_game_effect {
    int32_t flags;
    int32_t state;
    pvpp* play_param;
    std::vector<int64_t>* change_fields;
    std::vector<string_hash> pv_obj_set;
    std::vector<string_hash> pv_auth_3d;
    std::vector<string_hash> pv_glitter;
    std::vector<x_pv_game_song_effect> song_effect;
    FrameRateControl* frame_rate_control;

    x_pv_game_effect();
    ~x_pv_game_effect();

    void ctrl(object_database* obj_db, texture_database* tex_db);
    void load(int32_t pv_id, pvpp* play_param, FrameRateControl* frame_rate_control);
    void load_data(int32_t pv_id);
    void reset();
    void set_chara_id(int32_t index, int32_t chara_id, bool chara_scale);
    void set_song_effect(int32_t index, int64_t time);
    void set_song_effect_time(int32_t index, int64_t time, bool glitter);
    void set_song_effect_time_inner(int32_t index, int64_t time, bool glitter);
    void set_time(int64_t time, bool glitter);
    void stop();
    void stop_song_effect(int32_t index, bool free_glitter);
    void unload();
};

struct x_pv_game_chara_effect_auth_3d {
    bool field_0;
    chara_index src_chara;
    chara_index dst_chara;
    string_hash file;
    string_hash category;
    string_hash object_set;
    auth_3d_id id;
    int64_t time;

    x_pv_game_chara_effect_auth_3d();
    ~x_pv_game_chara_effect_auth_3d();

    void reset();
};

struct x_pv_game_chara_effect {
    int32_t state;
    pvpp* play_param;
    std::vector<int64_t>* change_fields;
    std::vector<x_pv_game_chara_effect_auth_3d> auth_3d[ROB_CHARA_COUNT];
    FrameRateControl* frame_rate_control;

    x_pv_game_chara_effect();
    ~x_pv_game_chara_effect();

#if BAKE_PV826
    void ctrl(object_database* obj_db, texture_database* tex_db,
        int32_t pv_id = -1, std::map<uint32_t, auth_3d_id>* effchrpv_auth_3d_mot_ids = 0);
    void load(int32_t pv_id, pvpp* play_param, FrameRateControl* frame_rate_control, chara_index charas[6],
        std::unordered_map<std::string, string_hash>* effchrpv_auth_3d_mot_names = 0);
#else
    void ctrl(object_database* obj_db, texture_database* tex_db);
    void load(int32_t pv_id, pvpp* play_param, FrameRateControl* frame_rate_control, chara_index charas[6]);
#endif
    void load_data();
    void reset();
    void set_chara_effect(int32_t chara_id, int32_t index, int64_t time);
    void set_chara_effect_time(int32_t chara_id, int32_t index, int64_t time);
    void set_time(int64_t time);
    void stop();
    void stop_chara_effect(int32_t chara_id, int32_t index);
    void unload();
};

struct x_pv_game_dsc_data {
    dsc dsc;
    dsc_data* dsc_data_ptr;
    dsc_data* dsc_data_ptr_end;
    int64_t time;

    x_pv_game_dsc_data();
    ~x_pv_game_dsc_data();

    void ctrl(float_t curr_time, float_t delta_time);
    dsc_data* find(int32_t func_name, int32_t* time,
        int32_t* pv_branch_mode, dsc_data* start, dsc_data* end);
    void find_bar_beat(x_pv_bar_beat& bar_beat);
    void find_change_fields(std::vector<int64_t>& change_fields);
    int64_t find_pv_end();
    void find_stage_effects(std::vector<std::pair<int64_t, int32_t>>& stage_effects);
    void reset();
    void unload();
};

struct x_pv_game_stage;

struct x_pv_game_data {
    int32_t pv_id;
    p_file_handler play_param_file_handler;
    pvpp* play_param;
    float_t curr_time;
    float_t delta_time;
    float_t pv_end_time;
    float_t time_float;
    int32_t frame;
    uint16_t field_1C;
    int32_t state;
    std::vector<int64_t> change_fields;
    x_pv_bar_beat bar_beat;
    int32_t stage_effect_index;
    int32_t next_stage_effect_bar;
    std::vector<std::pair<int64_t, int32_t>> stage_effects;
    x_pv_game_camera camera;
    x_pv_game_effect effect;
    x_pv_game_chara_effect chara_effect;
    x_pv_game_dsc_data dsc_data;
    string_hash exp_file;
    //dof dof;
    x_pv_game_stage* stage;

    object_database obj_db;
    texture_database tex_db;

    x_pv_game_data();
    ~x_pv_game_data();

#if BAKE_PV826
    void ctrl(float_t curr_time, float_t delta_time,
        std::map<uint32_t, auth_3d_id>* effchrpv_auth_3d_mot_ids = 0);
#else
    void ctrl(float_t curr_time, float_t delta_time);
#endif
    void ctrl_stage_effect_index();
#if BAKE_PV826
    void load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6],
        std::unordered_map<std::string, string_hash>* effchrpv_auth_3d_mot_names = 0);
#else
    void load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6]);
#endif
    void reset();
    void stop();
    void unload();
    void unload_if_state_is_0();
};

class PVStageFrameRateControl : public FrameRateControl {
public:
    float_t delta_frame;

    PVStageFrameRateControl();
    virtual ~PVStageFrameRateControl() override;

    virtual float_t GetDeltaFrame() override;
};

struct aet_obj_data {
    uint32_t hash;
    std::string layer_name;
    void* field_20;
    uint32_t id;
    bool loop;
    bool hidden;
    bool field_2A;
    FrameRateControl* frame_rate_control;

    aet_obj_data();
    ~aet_obj_data();

    bool check_disp();
    uint32_t init(AetArgs& args, const aet_database* aet_db);
    void reset();
};

struct x_pv_game_stage_env_data {
    auth_3d_id light_auth_3d_id;
    aet_obj_data data[5][8];

    x_pv_game_stage_env_data();
};

struct x_pv_game_stage_env_aet {
    int32_t state;
    float_t duration;
    aet_obj_data* prev[5][8];
    aet_obj_data* next[5][8];

    x_pv_game_stage_env_aet();
};

struct x_pv_game_stage_env {
    int32_t flags;
    int32_t state;
    pvsr* stage_resource;
    int32_t env_index;
    x_pv_game_stage_env_data data[64];
    uint32_t aet_gam_stgpv_id_hash;
    uint32_t spr_gam_stgpv_id_hash;
    uint32_t aet_gam_stgpv_id_main_hash;
    PVStageFrameRateControl frame_rate_control;
    float_t trans_duration;
    float_t trans_remain;
    x_pv_game_stage_env_aet aet;

    aet_database aet_db;
    sprite_database spr_db;

    x_pv_game_stage_env();
    ~x_pv_game_stage_env();

    void ctrl(float_t delta_time);
    pvsr_auth_2d* get_aet(int32_t env_index, int32_t type, int32_t index);
    void load(int32_t stage_id, pvsr* stage_resource);
    void reset();
    void reset_env();
    void set(int32_t env_index, float_t trans_time, float_t a4);
    void unload();

    void sub_810EE03E();
    bool sub_810EE198(float_t delta_time);
};

struct x_pv_game_stage_effect_auth_3d {
    bool repeat;
    bool field_1;
    auth_3d_id id;

    x_pv_game_stage_effect_auth_3d();

    void reset();
};

struct x_pv_game_stage_effect_glitter {
    string_hash name;
    Glitter::SceneCounter scene_counter;
    float_t fade_time;
    float_t fade_time_left;
    bool force_disp;

    x_pv_game_stage_effect_glitter();

    void reset();
};

struct x_pv_game_stage_change_effect {
    bool enable;
    std::vector<x_pv_game_stage_effect_auth_3d> auth_3d;
    std::vector<x_pv_game_stage_effect_glitter> glitter;
    int16_t bars_left;
    int16_t bar_count;

    x_pv_game_stage_change_effect();
    ~x_pv_game_stage_change_effect();

    void reset();
};

struct x_pv_game_stage_effect {
    pvsr_stage_effect* stage_effect;
    std::vector<x_pv_game_stage_effect_auth_3d> auth_3d;
    std::vector<x_pv_game_stage_effect_glitter> glitter;
    int32_t main_auth_3d_index;
    bool set;

    x_pv_game_stage_effect();
    ~x_pv_game_stage_effect();

    void reset();
};

#define X_PV_GAME_STAGE_EFFECT_COUNT 12

struct x_pv_game_stage_data {
    int32_t flags;
    int32_t state;
    p_file_handler file_handler;
    stage_database stg_db;
    FrameRateControl* frame_rate_control;
    std::vector<uint32_t> obj_hash;
    std::vector<task_stage_modern_info> stage_info;
    std::vector<uint32_t> objhrc_hash;

    x_pv_game_stage_data();
    ~x_pv_game_stage_data();

    bool check_not_loaded();
    void ctrl(object_database* obj_db, texture_database* tex_db);
    void load(int32_t stage_id, FrameRateControl* frame_rate_control);
    void load_objects(object_database* obj_db, texture_database* tex_db);
    void reset();
    void set_default_stage();
    void set_stage(uint32_t hash);
    void unload();
};

struct x_pv_game_stage {
    int32_t flags;
    int32_t stage_id;
    int32_t field_8;
    int32_t state;
    //int32_t field_10;
    p_file_handler stage_resource_file_handler;
    pvsr* stage_resource;
    x_pv_game_stage_env env;
    BPMFrameRateControl bpm_frame_rate_control;
    x_pv_game_stage_data stage_data;
    std::vector<string_hash> stage_auth_3d;
    std::vector<uint32_t> stage_glitter;
    int32_t curr_stage_effect;
    int32_t next_stage_effect;
    int32_t stage_effect_transition_state;
    x_pv_game_stage_effect effect[X_PV_GAME_STAGE_EFFECT_COUNT];
    x_pv_game_stage_change_effect change_effect
        [X_PV_GAME_STAGE_EFFECT_COUNT][X_PV_GAME_STAGE_EFFECT_COUNT];
    //struc_179 field_69CC0;
    //int32_t field_69D1C;
    //int32_t field_69D20;
    //x_pv_game_stage* field_69D24;

    object_database obj_db;
    texture_database tex_db;
    std::map<uint32_t, auth_3d_id> auth_3d_ids;

    x_pv_game_stage();
    ~x_pv_game_stage();

    bool check_stage_effect_has_change_effect(int32_t curr_stage_effect, int32_t next_stage_effect);
    void ctrl(float_t delta_time);
    void ctrl_inner();
    void load(int32_t stage_id, FrameRateControl* frame_rate_control, bool a4);
    void load_change_effect(int32_t curr_stage_effect, int32_t next_stage_effect);
    void reset();
    void reset_stage_effect();
    void reset_stage_env();
    bool set_change_effect_frame_part_1();
    void set_change_effect_frame_part_2(float_t frame);
    void set_env(int32_t env_index, float_t end_time, float_t start_time);
    void set_stage_effect(int32_t stage_effect);
    void set_stage_effect_auth_3d_frame(int32_t stage_effect, float_t frame);
    void set_stage_effect_glitter_frame(int32_t stage_effect, float_t frame);
    void stop_stage_change_effect();
    void stop_stage_effect(bool reset_stage);
    void stop_stage_effect_auth_3d(int32_t stage_effect);
    void stop_stage_effect_glitter(int32_t stage_effect);
    void unload();
};

class x_pv_game : public app::TaskWindow {
public:
    int32_t state;
    int32_t pv_count;
    x_pv_game_data pv_data[3];
    int32_t pv_index;
    x_pv_game_stage stage_data;
    XPVFrameRateControl field_7198C;
    XPVFrameRateControl field_71994;

    int32_t state_old;
    int32_t frame;
    double_t frame_float;
    int64_t time;
    int32_t rob_chara_ids[ROB_CHARA_COUNT];

    std::string light_category;

#if BAKE_PV826
    std::unordered_map<std::string, string_hash> effchrpv_auth_3d_mot_names;
#endif

    auth_3d_id light_auth_3d_id;

#if BAKE_PV826
    std::map<uint32_t, auth_3d_id> effchrpv_auth_3d_mot_ids;

    std::vector<int32_t> effchrpv_rob_mot_ids;
    std::map<int32_t, x_pv_game_a3da_to_mot> effchrpv_auth_3d_rob_mot_ids;
#endif

    std::vector<pv_data_set_motion> set_motion[ROB_CHARA_COUNT];

    bool play;
    bool success;
    int32_t chara_id;
    float_t target_anim_fps;
    float_t anim_frame_speed;
    bool pv_end;
    x_pv_play_data playdata[ROB_CHARA_COUNT];
    float_t scene_rot_y;
    mat4 scene_rot_mat;
    int32_t branch_mode;
    bool task_effect_init;

    bool pause;
    bool step_frame;

    int32_t pv_id;
    int32_t stage_id;
    chara_index charas[6];
    int32_t modules[6];

    x_pv_game();
    virtual ~x_pv_game() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;
    virtual void Window() override;

    void Load(int32_t pv_id, int32_t stage_id, chara_index charas[6], int32_t modules[6]);
    bool Unload();

    void ctrl(float_t curr_time, float_t delta_time);
    void stop_current_pv();
    void unload();
};

class XPVGameSelector : public app::TaskWindow {
public:
    int32_t pv_id;
    int32_t stage_id;
    chara_index charas[ROB_CHARA_COUNT];
    int32_t modules[ROB_CHARA_COUNT];
    bool start;
    bool exit;

    XPVGameSelector();
    virtual ~XPVGameSelector() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Window() override;
};

extern x_pv_game* x_pv_game_ptr;
extern XPVGameSelector x_pv_game_selector;

extern bool x_pv_game_init();
extern bool x_pv_game_free();
#endif
