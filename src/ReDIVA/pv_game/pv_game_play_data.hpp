/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../CRE/auth_2d.hpp"
#include "../../CRE/pv_db.hpp"

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
    PV_GAME_AET_FRAME_TOP,
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

    PV_GAME_AET_MAX_SLIDE_NUM = PV_GAME_AET_MAX_SLIDE_NUM_0,
    PV_GAME_AET_FRONT         = PV_GAME_AET_FRONT_0,
    PV_GAME_AET_FRONT_LOW     = PV_GAME_AET_FRONT_LOW_0,
    PV_GAME_AET_FRONT_3D_SURF = PV_GAME_AET_FRONT_3D_SURF_0,
    PV_GAME_AET_BACK          = PV_GAME_AET_BACK_0,
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
    uint32_t pv_spr_set_id;
    uint32_t pv_aet_set_id;
    uint32_t pv_aet_id;
    std::string title_2d_layer;

    pv_disp2d();
    ~pv_disp2d();
};

struct pv_game_play_data {
    bool pv_set;
    uint32_t aet_ids[PV_GAME_AET_MAX];
    bool aet_ids_enable[PV_GAME_AET_MAX];
    float_t frame_btm_pos[2];
    float_t frame_btm_pos_diff[2];
    float_t frame_top_pos[2];
    float_t frame_top_pos_diff[2];
    float_t song_info_pos;
    float_t song_info_pos_diff;
    float_t song_energy_pos;
    float_t song_energy_pos_diff;
    bool frame_disp[2];
    int32_t frame_state[2];
    float_t frame_top_pos_max[2];
    float_t frame_btm_pos_max[2];
    int32_t frame_danger_state;
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
    int32_t pv_aet_id;
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
    bool update;
    int32_t field_374;
    bool field_378;
    float_t field_37C;
    float_t score_speed;
    uint32_t score;
    int32_t value_text_display;
    float_t field_38C;
    int32_t value_text_index;
    float_t value_text_time_offset;
    int32_t combo_state;
    int32_t combo_count;
    float_t combo_disp_time;
    vec2 combo_pos;
    int32_t field_3AC;
    int32_t field_3B0;
    int32_t field_3B4;
    int32_t field_3B8;
    int32_t field_3BC;
    bool aix;
    bool ogg;
    int32_t field_3C4;
    std::string song_name;
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
    int32_t ex_song_index;
    float_t field_5EC;
    int32_t option;
    float_t field_5F4;
    char lyric[76];
    bool lyric_set;
    color4u8 lyric_color;
    bool field_64C;
    int32_t field_650;
    float_t field_654;
    int32_t field_658;
    float_t field_65C;

    pv_game_play_data();
    ~pv_game_play_data();

    void combo_ctrl(float_t delta_time);
    void ctrl(float_t delta_time);
    void disable_update();
    void disp();
    void disp_chance_point();
    void disp_chance_points(int32_t chance_points, float_t pos_x, float_t pos_y);
    void disp_chance_txt();
    void disp_combo();
    void disp_max_slide_point();
    void disp_score();
    void disp_slide_point();
    void disp_slide_points(int32_t max_slide_point, float_t pos_x, float_t pos_y, bool max_slide);
    void disp_song_energy();
    void disp_spr_set_back();
    void fade_begin_ctrl();
    bool fade_end_ctrl();
    void frame_ctrl();
    void frame_set(bool disp, int32_t index, int32_t state, float_t duration);
    void free_aet(pv_aet aet, int32_t index);
    void free_aet_title_image();
    float_t get_aet_frame(pv_aet aet, int32_t index);
    float_t get_aet_id_frame(pv_game_aet aet_index);
    bool get_aet_next_info_disp();
    int32_t get_digit_count(int32_t score, int32_t digits);
    void init_aet(pv_aet aet, int32_t index, std::string&& layer_name, std::string&& aet_name,
        bool* not_init, float_t start_time, float_t end_time, FrameRateControl* frame_rate_control);
    void init_aet_black_fade();
    void init_aet_chance_frame();
    void init_aet_chance_point(int32_t chance_points, float_t pos_x, float_t pos_y);
    void init_aet_chance_txt(bool end);
    void init_aet_combo(int32_t target_display_index,
        float_t pos_x, float_t pos_y, int32_t combo_count);
    void init_aet_demo_font();
    void init_aet_demo_logo();
    float_t init_aet_edit_effect(int32_t aet_id, const char* name, bool loop, bool low_field);
    void init_aet_frame(bool danger);
    uint32_t init_aet_gam_cmn(pv_game_aet aet_index, spr::SprPrio prio,
        AetFlags flags, const char* layer_name, const vec2* pos = 0, const vec2* scale = 0);
    void init_aet_id(int32_t aet_id, const char* layer_name,
        int32_t index, pv_game_aet aet_index, spr::SprPrio prio, bool* not_init,
        float_t start_time, float_t end_time, FrameRateControl* frame_rate_control);
    void init_aet_level_info();
    void init_aet_next_info();
    void init_aet_option_info();
    void init_aet_slide_max(int32_t slide_point, float_t pos_x, float_t pos_y);
    void init_aet_slide_point(int32_t slide_point, float_t pos_x, float_t pos_y);
    void init_aet_song_energy();
    void init_aet_song_icon_loop();
    void init_aet_stage_info();
    void init_aet_success_info();
    void init_aet_title_image(const char* name);
    void init_aet_white_fade();
    bool load_auth_2d(int32_t stage_index, pv_disp2d* disp2d);
    bool load_auth_2d_data();
    void read_auth_2d_data();
    void reset();
    void set_aet_edit_effect_frame(float_t frame);
    void set_aet_end_time(pv_aet aet, int32_t index, float_t value);
    void set_aet_frame(pv_aet aet, int32_t index, float_t value);
    void set_aet_id_alpha(pv_game_aet aet_index, float_t value);
    void set_aet_id_end_time(pv_game_aet aet_index, float_t value);
    void set_aet_id_frame(pv_game_aet aet_index, float_t value);
    void set_aet_id_play(pv_game_aet aet_index, bool value);
    void set_aet_id_position(pv_game_aet aet_index, const vec3& position);
    void set_aet_id_visible(pv_game_aet aet_index, bool value);
    void set_aet_id_visible_enable(pv_game_aet aet_index, bool value);
    void set_aet_song_energy();
    void set_aet_visible(pv_aet aet, int32_t index, bool value);
    void set_not_clear();
    void set_pv_disp2d(pv_disp2d* disp2d);
    void set_song_energy(bool disp);
    void set_spr_set_back_id(int32_t value);
    void set_stage_index(int32_t value);
    void set_value_text(int32_t index, float_t time_offset);
    void skin_danger_ctrl();
    void score_update(uint32_t score_final, bool update);
    void ui_set_disp();
    void ui_set_pos();
    void unload();
    void unload_aet();
    void unload_auth_2d();
    void unload_auth_2d_data();

    void sub_140134670(int32_t chance_result);
    void sub_140134730();
    void sub_1401349B0();
    void sub_1401349C0(uint32_t set_id);
    void sub_140135ED0();
    void sub_1401362C0();
    void sub_140137BE0();
    void sub_140137DD0(int32_t a2, float_t delta_time, vec2& pos, int32_t combo_count);
    void sub_140137F80(bool update, float_t duration);

    inline void reinit_aet_gam_cmn(pv_game_aet aet_index, spr::SprPrio prio,
        AetFlags flags, const char* layer_name, const vec2* pos = 0, const vec2* scale = 0) {
        aet_manager_free_aet_object_reset(&aet_ids[aet_index]);
        aet_ids[aet_index] = init_aet_gam_cmn(aet_index, prio, flags, layer_name, pos, scale);
    }
};

extern float_t get_percentage_clear_excellent();
extern float_t get_percentage_clear_great();
