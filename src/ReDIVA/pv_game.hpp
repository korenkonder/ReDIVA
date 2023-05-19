/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "../KKdLib/dsc.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/auth_2d.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/ogg_vorbis.hpp"
#include "../CRE/pv_db.hpp"
#include "../CRE/stage.hpp"
#include "../CRE/task.hpp"
#include "task_window.hpp"

enum pv_game_aet {
    PV_GAME_AET_0 = 0,
    PV_GAME_AET_1,
    PV_GAME_AET_VALUE_TEXT,
    PV_GAME_AET_COMBO,
    PV_GAME_AET_CHANCE_TXT,
    PV_GAME_AET_CHANCE_POINT,
    PV_GAME_AET_SLIDE_POINT,
    PV_GAME_AET_MAX_SLIDE_POINT,
    PV_GAME_AET_MAX_SLIDE_NUM_PLUS,
    PV_GAME_AET_MAX_SLIDE_NUM_0,
    PV_GAME_AET_MAX_SLIDE_NUM_1,
    PV_GAME_AET_MAX_SLIDE_NUM_2,
    PV_GAME_AET_MAX_SLIDE_NUM_3,
    PV_GAME_AET_MAX_SLIDE_NUM_4,
    PV_GAME_AET_MAX_SLIDE_NUM_5,
    PV_GAME_AET_MAX_SLIDE_NUM_6,
    PV_GAME_AET_FRAME_BOTTOM,
    PV_GAME_AET_FRAME_UP,
    PV_GAME_AET_CHANCE_FRAME_BOTTOM,
    PV_GAME_AET_CHANCE_FRAME_TOP,
    PV_GAME_AET_20,
    PV_GAME_AET_STAGE_INFO,
    PV_GAME_AET_SONG_ENERGY_BASE,
    PV_GAME_AET_SONG_ENERGY_BASE_REACH,
    PV_GAME_AET_SONG_ENERGY_NORMAL,
    PV_GAME_AET_SONG_ENERGY_REACH,
    PV_GAME_AET_SONG_ENERGY_FULL,
    PV_GAME_AET_SONG_ENERGY_EFF_NORMAL,
    PV_GAME_AET_SONG_ENERGY_EFF_REACH,
    PV_GAME_AET_SONG_ENERGY_CLEAR_TXT,
    PV_GAME_AET_SONG_ENERGY_BORDER_GREAT,
    PV_GAME_AET_SONG_ENERGY_BORDER_EXCELLENT,
    PV_GAME_AET_SONG_ENERGY_BORDER_RIVAL,
    PV_GAME_AET_SONG_ENERGY_EDGE_LINE02,
    PV_GAME_AET_SONG_ENERGY_BORDER,
    PV_GAME_AET_SONG_ENERGY_NOT_CLEAR,
    PV_GAME_AET_SONG_ENERGY_NOT_CLEAR_TXT,
    PV_GAME_AET_NEXT_INFO,
    PV_GAME_AET_LEVEL_INFO,
    PV_GAME_AET_OPTION_INFO_BASE,
    PV_GAME_AET_OPTION_INFO_HISPEED,
    PV_GAME_AET_OPTION_INFO_HIDDEN,
    PV_GAME_AET_OPTION_INFO_SUDDEN,
    PV_GAME_AET_SONG_ICON,
    PV_GAME_AET_SUCCESS_INFO,
    PV_GAME_AET_DEMO_LOGO,
    PV_GAME_AET_46,
    PV_GAME_AET_DEMO_FONT,
    PV_GAME_AET_FRONT_0,
    PV_GAME_AET_FRONT_1,
    PV_GAME_AET_FRONT_2,
    PV_GAME_AET_FRONT_3,
    PV_GAME_AET_FRONT_4,
    PV_GAME_AET_FRONT_5,
    PV_GAME_AET_FRONT_6,
    PV_GAME_AET_FRONT_7,
    PV_GAME_AET_FRONT_LOW_0,
    PV_GAME_AET_FRONT_LOW_1,
    PV_GAME_AET_FRONT_LOW_2,
    PV_GAME_AET_FRONT_LOW_3,
    PV_GAME_AET_FRONT_LOW_4,
    PV_GAME_AET_FRONT_LOW_5,
    PV_GAME_AET_FRONT_LOW_6,
    PV_GAME_AET_FRONT_LOW_7,
    PV_GAME_AET_FRONT_3D_SURF_0,
    PV_GAME_AET_FRONT_3D_SURF_1,
    PV_GAME_AET_FRONT_3D_SURF_2,
    PV_GAME_AET_FRONT_3D_SURF_3,
    PV_GAME_AET_FRONT_3D_SURF_4,
    PV_GAME_AET_FRONT_3D_SURF_5,
    PV_GAME_AET_FRONT_3D_SURF_6,
    PV_GAME_AET_FRONT_3D_SURF_7,
    PV_GAME_AET_BACK_0,
    PV_GAME_AET_BACK_1,
    PV_GAME_AET_BACK_2,
    PV_GAME_AET_BACK_3,
    PV_GAME_AET_BACK_4,
    PV_GAME_AET_BACK_5,
    PV_GAME_AET_BACK_6,
    PV_GAME_AET_BACK_7,
    PV_GAME_AET_TITLE_IMAGE,
    PV_GAME_AET_WHITE_FADE,
    PV_GAME_AET_BLACK_FADE,
    PV_GAME_AET_83,
    PV_GAME_AET_84,
    PV_GAME_AET_TOP_FRONT,
    PV_GAME_AET_BOTTOM_FRONT,
    PV_GAME_AET_TOP_BACK,
    PV_GAME_AET_BOTTOM_BACK,
    PV_GAME_AET_EDIT_EFFECT,
    PV_GAME_AET_MAX,
};

enum pv_game_music_action {
    PV_GAME_MUSIC_ACTION_NONE = 0,
    PV_GAME_MUSIC_ACTION_STOP,
    PV_GAME_MUSIC_ACTION_PAUSE,
    PV_GAME_MUSIC_ACTION_PLAY,
    PV_GAME_MUSIC_ACTION_MAX,
};

enum pv_game_music_flags : uint8_t {
    PV_GAME_MUSIC_FLAG_1  = 0x01,
    PV_GAME_MUSIC_AIX     = 0x02,
    PV_GAME_MUSIC_OGG     = 0x04,
    PV_GAME_MUSIC_FLAG_8  = 0x08,
    PV_GAME_MUSIC_FLAG_10 = 0x10,
    PV_GAME_MUSIC_FLAG_20 = 0x20,
    PV_GAME_MUSIC_FLAG_40 = 0x40,
    PV_GAME_MUSIC_FLAG_80 = 0x80,

    PV_GAME_MUSIC_ALL     = 0xFF,
};

struct pv_game_music_fade {
    int32_t start;
    int32_t value;
    float_t time;
    float_t remain;
    pv_game_music_action action;
    bool enable;
};

struct pv_game_music_args {
    int32_t type;
    std::string file_path;
    float_t start;
    bool field_2C;

    pv_game_music_args();
    ~pv_game_music_args();
};

struct pv_game_music_ogg {
    OggPlayback* playback;
    std::string file_path;

    pv_game_music_ogg();
    ~pv_game_music_ogg();
};

struct pv_game_music {
    pv_game_music_flags flags;
    bool pause;
    int32_t volume;
    int32_t master_volume;
    int32_t channel_pair_volume[4];
    pv_game_music_fade fade_in;
    pv_game_music_fade fade_out;
    bool no_fade;
    float_t no_fade_remain;
    float_t fade_out_time_req;
    pv_game_music_action fade_out_action_req;
    int32_t type;
    std::string file_path;
    float_t start;
    float_t end;
    bool play_on_end;
    float_t fade_in_time;
    float_t fade_out_time;
    bool field_9C;
    pv_game_music_args args;
    bool loaded;
    pv_game_music_ogg* ogg;

    pv_game_music();
    ~pv_game_music();

    bool check_args(int32_t type, std::string&& file_path, float_t start);
    void ctrl(float_t delta_time);
    void exclude_flags(pv_game_music_flags flags);
    void fade_in_end();
    void fade_out_end();
    void file_load(int32_t type, std::string&& file_path, bool play_on_end,
        float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9);
    int32_t get_master_volume(int32_t index);
    int32_t get_volume(int32_t index);
    int32_t include_flags(pv_game_music_flags flags);
    int32_t load(int32_t type, std::string&& file_path, bool wait_load, float_t time, bool a6);
    void ogg_free();
    int32_t ogg_init();
    int32_t ogg_load(std::string&& file_path, float_t start);
    int32_t ogg_reset();
    int32_t ogg_stop();
    int32_t play();
    int32_t play(int32_t type, std::string&& file_path, bool play_on_end,
        float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9);
    void play_fade_in(int32_t type, std::string&& file_path, float_t start,
        float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10);
    int32_t play_or_stop();
    void reset();
    void reset_args();
    void set_args(int32_t type, std::string&& file_path, float_t start, bool a5);
    int32_t set_channel_pair_volume(int32_t channel_pair, int32_t value);
    int32_t set_channel_pair_volume_map(int32_t channel_pair, int32_t value);
    int32_t set_fade_out(float_t time, bool stop);
    int32_t set_master_volume(int32_t value);
    int32_t set_ogg_args(std::string&& file_path, float_t start, bool wait_load);
    int32_t set_ogg_pause_state(uint8_t pause_state);
    int32_t set_pause(int32_t pause);
    void set_volume_map(int32_t index, int32_t value);
    int32_t stop();
    void stop_reset_flags();
};

struct pv_play_data_set_motion {
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

struct pv_play_data_motion_data {
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

    pv_play_data_motion_data();
    ~pv_play_data_motion_data();

    void clear();
    void reset();
};

struct pv_play_data {
    std::vector<pv_play_data_motion> motion;
    rob_chara* rob_chr;
    std::list<pv_play_data_set_motion> set_motion;
    bool disp;
    int8_t field_31;
    int32_t field_34;
    pv_play_data_motion_data motion_data;

    pv_play_data();
    ~pv_play_data();

    void reset();
};

struct pv_disp2d {
    int32_t pv_id;
    int32_t title_start_2d_field;
    int32_t title_end_2d_field;
    int32_t title_start_2d_low_field;
    int32_t title_end_2d_low_field;
    int32_t title_start_3d_field;
    int32_t title_end_3d_field;
    bool target_shadow_type;
    int32_t pv_spr_set_id;
    int32_t pv_aet_set_id;
    int32_t pv_main_aet_id;
    std::string title_2d_layer;

    pv_disp2d();
    ~pv_disp2d();
};

struct pv_data_camera {
    bool enable;
    int32_t id;
    int64_t time;

    pv_data_camera();
};

struct struc_540 {
    int32_t field_0;
    vec2 field_4;
    vec2 field_C;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    int32_t field_20;
};

struct struc_498 {
    int32_t field_0;
    struc_540 field_4[4];
    int32_t field_94;
    int64_t field_98;
    int64_t field_A0;
    int32_t field_A8;
    int32_t field_AC;
};

struct struc_563 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
    int64_t dsc_time;
    int64_t field_28;
};

struct struc_676 {
    bool field_0[6];
    pv_play_data_set_motion field_8[6];
    int64_t field_128;
    int64_t field_130;
    int64_t field_138;
    int64_t field_140;
    bool field_148[6];
    struc_563 field_150[6];
    float_t field_270[6];
    bool field_288[6];
    int32_t field_290[6];
    float_t field_2A8[6];
    int8_t field_2C0;
    int32_t field_2C4;
    int32_t field_2C8;
    float_t field_2CC;
    int64_t field_2D0;
};

struct pv_game;

struct pv_game_pv_data {
    int8_t field_0;
    int32_t state;
    bool play;
    uint32_t dsc_buffer[45000];
    int32_t dsc_buffer_counter;
    int64_t field_2BF30;
    int64_t field_2BF38;
    int64_t curr_time;
    float_t curr_time_float;
    float_t prev_time_float;
    bool field_2BF50;
    int64_t field_2BF58;
    bool field_2BF60;
    int32_t field_2BF64;
    int64_t field_2BF68;
    int32_t chara_id;
    pv_game* pv_game;
    //struc_162* field_2BF80;
    pv_game_music* music;
    p_file_handler dsc_file_handler;
    float_t target_anim_fps;
    float_t anim_frame_speed;
    float_t target_flying_time;
    int32_t time_signature;
    ssize_t dsc_time;
    int32_t field_2BFB0;
    int32_t field_2BFB4;
    int32_t field_2BFB8;
    int32_t field_2BFBC;
    int8_t field_2BFC0;
    bool music_play;
    int32_t field_2BFC4;
    bool pv_end;
    float_t fov;
    float_t min_dist;
    int8_t field_2BFD4;
    int8_t field_2BFD5;
    pv_play_data playdata[6];
    pv_data_camera data_camera[3];
    std::vector<int32_t> data_camera_branch_fail;
    std::vector<int32_t> data_camera_branch_success;
    std::vector<pv_data_set_motion> set_motion[6];
    prj::vector_pair<int32_t, int32_t> change_field_branch_fail;
    prj::vector_pair<int32_t, int32_t> change_field_branch_success;
    int64_t change_field_branch_time;
    bool scene_fade_enable;
    float_t scene_fade_frame;
    float_t scene_fade_duration;
    float_t scene_fade_start_alpha;
    float_t scene_fade_end_alpha;
    vec3 scene_fade_color;
    bool has_signature;
    bool has_perf_id;
    int32_t field_2C4E4;
    std::vector<struc_498> field_2C4E8;
    size_t field_2C500;
    float_t scene_rot_y;
    mat4 scene_rot_mat;
    float_t field_2C54C;
    int32_t field_2C550;
    int32_t branch_mode;
    int32_t first_challenge_note;
    int32_t last_challenge_note;
    struc_676 field_2C560;

    pv_game_pv_data();
    ~pv_game_pv_data();

    void reset();
    void reset_camera_post_process();
};

struct pv_game_play_data {
    bool pv_set;
    uint32_t aet_ids[PV_GAME_AET_MAX];
    bool aet_ids_enable[PV_GAME_AET_MAX];
    float_t field_1C8[2];
    float_t field_1D0[2];
    float_t field_1D8[2];
    float_t field_1E0[2];
    float_t field_1E8[2];
    float_t field_1F0[2];
    bool disp[2];
    int32_t field_1FC[2];
    float_t field_204[2];
    float_t field_20C[2];
    int32_t frame_state;
    bool field_218;
    bool fade_begin;
    bool fade_end;
    pv_game_aet fade;
    int32_t skin_danger_frame;
    aet_layout_data song_txt[2];
    int32_t field_2FC;
    int32_t chance_result;
    int32_t chance_points;
    vec2 chance_points_pos;
    int32_t field_310;
    int32_t slide_points;
    vec2 slide_points_pos;
    int32_t max_slide_points;
    vec2 max_slide_points_pos;
    int32_t pv_spr_set_id;
    int32_t pv_aet_set_id;
    int32_t pv_main_aet_id;
    int32_t stage_index;
    int32_t field_33C;
    AetComp comp;
    float_t field_350;
    bool field_354;
    int32_t value_text_spr_index;
    bool not_clear;
    int32_t spr_set_back_id;
    bool loaded;
    int32_t state;
    int32_t field_36C;
    bool score_preview_update;
    int32_t field_374;
    bool field_378;
    float_t field_37C;
    float_t score_preview_speed;
    uint32_t score_preview;
    int32_t value_text_display;
    float_t field_38C;
    int32_t value_text_index;
    float_t value_text_time_offset;
    int32_t combo_state;
    int32_t combo_count;
    float_t combo_disp_time;
    vec2 draw_pos;
    int32_t field_3AC;
    int32_t field_3B0;
    int32_t field_3B4;
    int32_t field_3B8;
    int32_t field_3BC;
    bool aix;
    bool ogg;
    int32_t field_3C4;
    std::string field_3C8;
    std::string se_name;
    std::string pvbranch_success_se_name;
    std::string slide_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    std::string chainslide_failure_name;
    std::string slidertouch_name;
    std::string other_se_name;
    std::string other_slide_name;
    std::string other_chainslide_first_name;
    std::string other_chainslide_sub_name;
    std::string other_chainslide_success_name;
    std::string other_chainslide_failure_name;
    std::string other_slidertouch_name;
    std::string song_file_name;
    int32_t field_5E8;
    float_t field_5EC;
    int32_t option;
    float_t field_5F4;
    char lyric[76];
    uint8_t lyric_index;
    vec4u8 lyrics_color;
    bool field_64C;
    int32_t field_650;
    float_t field_654;
    int32_t field_658;
    float_t field_65C;

    pv_game_play_data();
    ~pv_game_play_data();

    void reset();

    void sub_1401362C0();
};

struct pv_game_chara {
    chara_index chara_index;
    int32_t cos;
    int32_t chara_id;
    rob_chara_pv_data pv_data;
    rob_chara* rob_chr;
    std::vector<uint32_t> motion_ids;
    std::vector<uint32_t> motion_face_ids;

    pv_game_chara();
    ~pv_game_chara();
};

struct pv_game_edit_effect {
    int8_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    bool field_14;
};

struct pv_game_field {
    int32_t stage_index;
    std::vector<int32_t> auth_3d_uids;
    std::vector<int32_t> auth_3d_ids;
    std::map<int32_t, int32_t> auth_3d_frame_list;
    int32_t light_auth_3d_uid;
    int32_t light_auth_3d_id;
    int32_t light_frame;
    std::vector<std::string> aet_set_names[4];
    std::vector<std::string> aet_set_ids[4];
    std::map<std::pair<std::string, std::string>, int32_t> aet[4];
    int32_t spr_set_back_id;
    bool stage_flag;
    int32_t npr_type;
    int32_t cam_blur;
    bool sdw_off;
    std::vector<uint64_t> play_eff_list;
    std::vector<uint64_t> stop_eff_list;
    std::vector<uint64_t> field_1A0;

    pv_game_field();
    ~pv_game_field();
};

struct pv_game_itmpv {
    int32_t auth_3d_id;
    int32_t field_4;
    int64_t time;
};

struct struc_269 {
    prj::vector_pair<object_info, int32_t> data;
    std::vector<auth_3d_id> auth_3d_ids;
    int32_t field_30;
    int32_t field_34;

    struc_269();
    ~struc_269();
};

struct pv_game_chreff {
    chara_index src_chara_index;
    chara_index dst_chara_index;
    std::bitset<128> type;
    std::string src_auth_3d;
    std::string dst_auth_3d;
    std::string src_auth_3d_category;
    std::string dst_auth_3d_category;
    int32_t src_auth_3d_uid;
    int32_t dst_auth_3d_uid;

    pv_game_chreff();
    ~pv_game_chreff();
};

struct pv_effect_resource {
    std::string name;
    float_t emission;

    pv_effect_resource();
    ~pv_effect_resource();
};

struct pv_game_data {
    pv_game_pv_data pv_data;
    pv_game_play_data play_data;
    int32_t field_2CE98;
    int8_t field_2CE9C[128];
    int32_t field_2CF1C;
    int8_t field_2CF20;
    int32_t field_2CF24;
    float_t field_2CF28;
    float_t field_2CF2C;
    int32_t field_2CF30;
    int32_t field_2CF34;
    int32_t field_2CF38;
    int32_t life_gauge;
    int32_t score_final;
    int32_t challenge_time_total_bonus;
    int32_t combo;
    int32_t challenge_time_combo_count;
    int32_t max_combo;
    int32_t total_hit_count[5];
    int32_t hit_count[5];
    int32_t current_reference_score;
    int32_t target_count;
    int32_t field_2CF84;
    int32_t field_2CF88;
    float_t field_2CF8C;
    int32_t score_slide_chain_bonus;
    int32_t slide_chain_length;
    int32_t field_2CF98;
    int32_t field_2CF9C;
    int16_t field_2CFA0;
    int32_t field_2CFA4;
    pv_db_pv* pv;
    int32_t field_2CFB0;
    int32_t chance_point;
    int32_t reference_score;
    int32_t field_2CFBC;
    int32_t field_2CFC0;
    int32_t field_2CFC4;
    std::vector<int32_t> field_2CFC8;
    int32_t field_2CFE0;
    int32_t field_2CFE4;
    size_t notes_passed;
    int32_t field_2CFF0;
    float_t song_energy;
    float_t song_energy_base;
    float_t song_energy_border;
    int32_t life_gauge_safety_time;
    int32_t life_gauge_border;
    int32_t stage_index;
    bool field_2D00C;
    bool no_fail;
    bool field_2D00E;
    bool changed_field;
    int64_t challenge_time_start;
    int64_t challenge_time_end;
    int64_t max_time;
    float_t max_time_float;
    float_t current_time_float;
    uint64_t current_time;
    float_t current_time_float_dup;
    float_t field_2D03C;
    int32_t score_hold_multi;
    uint32_t score_hold;
    int32_t score_slide;
    bool has_slide;
    bool has_success_se;
    pv_disp2d* pv_disp2d;
    int32_t life_gauge_final;
    bool hit_border;
    int8_t field_2D05D;
    int8_t field_2D05E;
    int8_t field_2D05F;
    bool title_image_init;
    bool field_2D061;
    bool field_2D062;
    bool field_2D063;
    bool has_auth_3d_frame;
    bool has_light_frame;
    bool has_aet_frame[4];
    bool has_aet_list[4];
    uint32_t field_index;
    int32_t edit_effect_index;
    int32_t slidertouch_counter;
    int32_t change_field_branch_success_counter;
    int32_t field_2D080;
    int32_t field_2D084;
    int32_t life_gauge_bonus;
    int32_t life_gauge_total_bonus;
    int8_t field_2D090;
    int8_t field_2D091;
    int8_t field_2D092;
    int8_t field_2D093;
    uint8_t field_2D094;
    bool field_2D095;
    int8_t field_2D096;
    bool success;
    int32_t field_2D098;
    int32_t field_2D09C;
    bool use_osage_play_data;
    bool pv_end_fadeout;
    float_t rival_percentage;
    int32_t field_2D0A8;
    int32_t field_2D0AC;
    int32_t field_2D0B0[3];
    bool field_2D0BC;
    bool next_stage;
    bool field_2D0BE;
    int8_t field_2D0BF;
    int32_t field_2D0C0;
    int32_t field_2D0C4;
    pv_game_chara chara[6];
    std::vector<uint32_t> motion_set_ids;
    std::vector<uint32_t> motion_set_face_ids;
    std::vector<uint32_t> obj_set_itmpv;
    std::vector<uint32_t> obj_set_handitem;
    std::vector<uint32_t> chreff_auth_3d_obj_set_ids;
    std::vector<uint32_t> field_2D770;
    std::vector<uint32_t> stgpvhrc_obj_set_ids;
    std::vector<uint32_t> ex_song_ex_auth_obj_set_ids;
    std::vector<int32_t> stage_indices;
    std::vector<task_stage_info> stage_infos;
    bool field_2D7E8;
    int32_t field_2D7EC;
    std::vector<int32_t> spr_set_back_ids;
    int32_t field_2D808;
    int32_t field_2D80C;
    std::vector<int32_t> edit_effect_aet_set_ids;
    std::vector<int32_t> edit_effect_spr_set_ids;
    std::vector<int32_t> edit_effect_aet_ids;
    pv_game_edit_effect edit_effect;
    uint32_t camera_auth_3d_uid;
    int32_t field_2D874;
    bool task_effect_init;
    int32_t field_2D87C;
    size_t current_field;
    std::vector<pv_game_field> field_data;
    bool field_2D8A0;
    int32_t field_2D8A4;
    std::map<int32_t, int64_t> auth_3d_time;
    std::map<int32_t, int64_t> light_time;
    std::map<std::pair<std::string, std::string>, int64_t> aet_time[4];
    std::string campv_string;
    std::vector<int32_t> campv_auth_3d_uids;
    std::map<int32_t, int32_t> campv;
    int32_t data_camera_id;
    int32_t field_2D954;
    std::string itmpv_string;
    std::vector<int32_t> itmpv_auth_3d_uids;
    std::map<int32_t, pv_game_itmpv> itmpv[6];
    std::vector<std::string> itmpv_uids;
    struc_269 field_2DA08[3];
    std::vector<int32_t> loaded_auth_3d_uids;
    bool field_2DAC8;
    int32_t field_2DACC;
    std::vector<std::string> auth_3d_categories;
    std::map<int32_t, auth_3d_id> auth_3d;
    std::vector<pv_game_chreff> chreff_auth_3d;
    std::vector<pv_game_chreff> chreff_auth_3d_obj;
    int8_t field_2DB28;
    vec2 field_2DB2C;
    int32_t field_2DB34;
    time_struct field_2DB38;
    std::vector<uint64_t> effect_rs_list_hashes;
    std::vector<pv_effect_resource> effect_rs_list;

    pv_game_data();
    ~pv_game_data();
};

struct pv_game_item_mask {
    bool arr[4];

    pv_game_item_mask();
};

struct pv_game {
    bool loaded;
    bool field_1;
    bool field_2;
    bool end_pv;
    bool field_4;
    int32_t state;
    int32_t field_C;
    int32_t pv_id;
    bool field_14;
    int32_t modules[6];
    rob_chara_pv_data_item items[6];
    pv_game_item_mask items_mask[6];
    pv_game_data data;

    pv_game();
    ~pv_game();

    void ctrl();
    void disp();
    std::string get_effect_se_file_name();
    void load();
    void reset();
    bool unload();
};

struct pv_game_init_data {
    int32_t pv_id;
    int32_t difficulty;
    int32_t field_C;
    int32_t score_percentage_clear;
    int32_t life_gauge_safety_time;
    int32_t life_gauge_border;
    int32_t field_18;
    int32_t modules[6];
    rob_chara_pv_data_item items[6];
    pv_game_item_mask items_mask[6];

    pv_game_init_data();

    void reset();
};

class TaskPvGame : public app::Task {
public:
    struct Args {
        pv_game_init_data init_data;
        std::string se_name;
        std::string slide_se_name;
        std::string chainslide_first_name;
        std::string chainslide_sub_name;
        std::string chainslide_success_name;
        std::string chainslide_failure_name;
        std::string slidertouch_name;
        bool field_190;
        bool field_191;
        bool no_fail;
        bool field_193;
        bool field_194;
        bool field_195;
        bool field_196;
        bool field_197;
        bool test_pv;
        int32_t option;

        Args();
        ~Args();

        void Clear();
        void Reset();
    };

    struct Data {
        int32_t field_0;
        pv_game_init_data init_data;
        std::string se_name;
        std::string slide_se_name;
        std::string chainslide_first_name;
        std::string chainslide_sub_name;
        std::string chainslide_success_name;
        std::string chainslide_failure_name;
        std::string slidertouch_name;
        bool field_190;
        bool field_191;
        bool no_fail;
        bool field_193;
        bool field_194;
        bool field_195;
        bool field_196;
        int32_t option;

        Data();
        ~Data();

        void Clear();
        void Reset();
    } data;

    TaskPvGame();
    virtual ~TaskPvGame() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
};

bool task_pv_game_add_task(TaskPvGame::Args& args);
bool task_pv_game_check_task_ready();
bool task_pv_game_del_task();
