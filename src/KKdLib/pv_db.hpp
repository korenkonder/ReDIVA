/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.h"
#include "database/item_table.hpp"

enum pv_db_chreff_type {
    PV_DB_CHARA_EFFECT_AUTH3D     = 0,
    PV_DB_CHARA_EFFECT_AUTH3D_OBJ = 1,
};

enum pv_db_format {
    PV_DB_NONE,
    PV_DB_AC101,
    PV_DB_AC110,
    PV_DB_AC120,
    PV_DB_AC130,
    PV_DB_AC200,
    PV_DB_AC210,
    PV_DB_AC211,
    PV_DB_AC220,
    PV_DB_AC221,
    PV_DB_AC301,
    PV_DB_AC310,
    PV_DB_AC320,
    PV_DB_AC401,
    PV_DB_AC402,
    PV_DB_AC410,
    PV_DB_AC500,
    PV_DB_AC510,
    PV_DB_AC511,
    PV_DB_AC520,
    PV_DB_AC521,
    PV_DB_AC600,
    PV_DB_AFT101,
    PV_DB_AFT110,
    PV_DB_AFT120,
    PV_DB_AFT200,
    PV_DB_AFT210,
    PV_DB_AFT220,
    PV_DB_AFT300,
    PV_DB_AFT310,
    PV_DB_AFT400,
    PV_DB_AFT410,
    PV_DB_AFT500,
    PV_DB_AFT501,
    PV_DB_AFT510,
    PV_DB_AFT600,
    PV_DB_AFT601,
    PV_DB_AFT610,
    PV_DB_AFT620,
    PV_DB_AFT621,
    PV_DB_AFT622,
    PV_DB_AFT701,
    PV_DB_AFT710,
    PV_DB_FT,
};

enum pv_db_duet_type {
    PV_DB_DUET_USER_SETTING   = 0,
    PV_DB_DUET_FIXED_SIZE     = 1,
    PV_DB_DUET_FIXED_SUB      = 2,
    PV_DB_DUET_FIXED_ALL      = 3,
    PV_DB_DUET_PSEUDO_DEFAULT = 4,
    PV_DB_DUET_PSEUDO_SAME    = 5,
    PV_DB_DUET_PSEUDO_SWIM    = 6,
    PV_DB_DUET_PSEUDO_SWIM_S  = 7,
};

enum pv_db_eyes_base_adjust_type {
    PV_DB_EYES_BASE_ADJUST_DIRECTION = 0,
    PV_DB_EYES_BASE_ADJUST_CLEARANCE = 1,
    PV_DB_EYES_BASE_ADJUST_OFF       = 2,
};

enum pv_db_frame_texture_type {
    PV_DB_FRAME_TEXTURE_PRE_PP  = 0,
    PV_DB_FRAME_TEXTURE_POST_PP = 1,
    PV_DB_FRAME_TEXTURE_FB      = 2,
};

enum pv_db_movie_surface {
    PV_DB_MOVIE_SURFACE_BACK  = 0,
    PV_DB_MOVIE_SURFACE_FRONT = 1,
};

enum pv_db_performer_size {
    PV_DB_PERFORMER_SIZE_NORMAL     = 0,
    PV_DB_PERFORMER_SIZE_PLAY_CHARA = 1,
    PV_DB_PERFORMER_SIZE_PV_CHARA   = 2,
    PV_DB_PERFORMER_SIZE_SHORT      = 3,
    PV_DB_PERFORMER_SIZE_TALL       = 4,
};

enum pv_db_performer_type {
    PV_DB_PERFORMER_VOCAL          = 0,
    PV_DB_PERFORMER_PSEUDO_DEFAULT = 1,
    PV_DB_PERFORMER_PSEUDO_SAME    = 2,
    PV_DB_PERFORMER_PSEUDO_SWIM    = 3,
    PV_DB_PERFORMER_PSEUDO_SWIM_S  = 4,
    PV_DB_PERFORMER_PSEUDO_MYCHARA = 5,
    PV_DB_PERFORMER_GUEST          = 6,
};

enum pv_db_pv_level {
    PV_DB_PV_LV_00_0 = 0,
    PV_DB_PV_LV_00_5 = 1,
    PV_DB_PV_LV_01_0 = 2,
    PV_DB_PV_LV_01_5 = 3,
    PV_DB_PV_LV_02_0 = 4,
    PV_DB_PV_LV_02_5 = 5,
    PV_DB_PV_LV_03_0 = 6,
    PV_DB_PV_LV_03_5 = 7,
    PV_DB_PV_LV_04_0 = 8,
    PV_DB_PV_LV_04_5 = 9,
    PV_DB_PV_LV_05_0 = 10,
    PV_DB_PV_LV_05_5 = 11,
    PV_DB_PV_LV_06_0 = 12,
    PV_DB_PV_LV_06_5 = 13,
    PV_DB_PV_LV_07_0 = 14,
    PV_DB_PV_LV_07_5 = 15,
    PV_DB_PV_LV_08_0 = 16,
    PV_DB_PV_LV_08_5 = 17,
    PV_DB_PV_LV_09_0 = 18,
    PV_DB_PV_LV_09_5 = 19,
    PV_DB_PV_LV_10_0 = 20,
};

struct pv_db_another_song_aft310 {
    std::string name;
    std::string song_file_name;
};

struct pv_db_another_song_aft500 {
    std::string name;
    std::string name_en;                                                // FT/M39
    std::string song_file_name;
    std::string vocal_chara_num;                                        // FT/M39
    std::string vocal_disp_name;                                        // FT/M39
    std::string vocal_disp_name_en;                                     // FT/M39
};

struct pv_db_auth_replace_by_module {
    null_int32_t id;
    null_int32_t module_id;
    std::string name;
    std::string org_name;
};

null(chara_index)

struct pv_db_chrcam {
    null_chara_index chara;

    null_int32_t id;
    std::string name;
    std::string org_name;
};

null(pv_db_chreff_type)

struct pv_db_chreff_data {
    null_pv_db_chreff_type type;

    std::string Name;
};

struct pv_db_chreff {
    std::vector<pv_db_chreff_data> data;

    null_int32_t id;
    std::string name;
};

struct pv_db_chrmot {
    null_chara_index chara;

    null_int32_t id;
    std::string name;
    std::string org_name;
};

struct pv_db_disp2d_ac200 {
    std::string set_name;
    null_int32_t target_shadow_type;
    null_int32_t title_end_2d_field;
    null_int32_t title_end_2d_low_field;                                // not AC200
    null_int32_t title_end_3d_field;
    null_int32_t title_start_2d_field;
    null_int32_t title_start_2d_low_field;                              // not AC200
    null_int32_t title_start_3d_field;
};

struct pv_db_disp2d_aft210 {
    std::string set_name;
    null_int32_t target_shadow_type;
    null_int32_t title_2d_layer;
    null_int32_t title_end_2d_field;
    null_int32_t title_end_2d_low_field;
    null_int32_t title_end_3d_field;
    null_int32_t title_start_2d_field;
    null_int32_t title_start_2d_low_field;
    null_int32_t title_start_3d_field;
};

null(pv_db_duet_type)

struct pv_db_duet {
    null_pv_db_duet_type type;
    null_int32_t num;
};

struct pv_db_eyes_rot_rate {
    null_chara_index chr;
    null_float_t xn_rate;
    null_float_t xp_rate;
};

struct pv_db_ex_auth {
    std::string name;
    std::string org_name;
};

struct pv_db_ex_song_ac101 {
    null_chara_index chara;

    std::string file;
    std::string name;
};

struct pv_db_ex_song_aft101 {
    null_chara_index chara;
    null_chara_index chara2P;                                           // not AFT101
    null_chara_index chara3P;                                           // not AFT101
    null_chara_index chara4P;                                           // not AFT101

    std::string file;
    std::string name;
};

struct pv_db_ex_song_aft300 {
    std::vector<pv_db_ex_auth> ex_auth;                                 // AFT621/AFT622/AFT701/FT/M39

    null_chara_index chara;
    null_chara_index chara2P;
    null_chara_index chara3P;
    null_chara_index chara4P;
    null_chara_index chara5P;                                           // not AFT300/AFT310/AFT400
    null_chara_index chara6P;                                           // not AFT300/AFT310/AFT400

    std::string file;
    std::string name;
};

struct pv_db_field_ac101 {
    std::string aet_back;
    std::string aet_front;
    std::string aet_id_back;
    std::string aet_id_front;
    std::string auth_3d;
    std::string spr_set_back;
    std::string stage;
};

struct pv_db_field_ac200 {
    std::vector<std::string> auth_3d_list;                              // not AC200/AC210/AC211
    std::vector<int32_t> auth_3d_frame_list;                            // AC520/AC521/AC600
    std::vector<std::string> ex_auth_3d_list;                           // AC510/AC511/AC520/AC521/AC600

    std::string aet_back;
    std::string aet_front;
    std::string aet_front_low;                                          // not AC200
    std::string aet_id_back;
    std::string aet_id_front;
    std::string aet_id_front_low;                                       // not AC200
    std::string auth_3d;
    std::string ex_auth_3d;                                             // AC510/AC511/AC520/AC521/AC600
    std::string ex_stage;                                               // AC500/AC510/AC511/AC520/AC521/AC600
    std::string spr_set_back;
    std::string stage;
    null_int32_t stage_flag;                                            // AC520/AC521/AC600
};

struct pv_db_field_ac500 {
    std::vector<std::string> auth_3d_list;

    std::string aet_back;
    std::string aet_front;
    std::string aet_front_low;
    std::string aet_id_back;
    std::string aet_id_front;
    std::string aet_id_front_low;
    std::string auth_3d;
    std::string ex_auth_3d;                                             // not AC500
    std::string ex_stage;
    std::string spr_set_back;
    std::string stage;
    null_int32_t stage_flag;                                            // AC520/AC521/AC600
};

struct pv_db_field_aft101 {
    std::vector<std::string> auth_3d_list;
    std::vector<int32_t> auth_3d_frame_list;
    std::vector<std::string> ex_auth_3d_list;

    std::string aet_back;
    std::string aet_front;
    std::string aet_front_low;
    std::string aet_id_back;
    std::string aet_id_front;
    std::string aet_id_front_low;
    std::string auth_3d;
    null_int32_t cam_blur;                                              // AFT110/AFT120/AFT200
    std::string ex_auth_3d;
    std::string ex_stage;
    std::string light;                                                  // AFT100
    null_int32_t npr_type;                                              // AFT110/AFT120/AFT200
    null_int32_t sdw_off;                                               // AFT110/AFT120/AFT200
    std::string spr_set_back;
    std::string stage;
    null_int32_t stage_flag;
};

struct pv_db_field_aft210 {
    std::vector<int32_t> aet_back_frame_list;
    std::vector<std::string> aet_back_list;
    std::vector<int32_t> aet_front_frame_list;
    std::vector<std::string> aet_front_list;
    std::vector<int32_t> aet_front_low_frame_list;
    std::vector<std::string> aet_front_low_list;
    std::vector<std::string> aet_id_back_list;
    std::vector<std::string> aet_id_front_list;
    std::vector<std::string> aet_id_front_low_list;
    std::vector<int32_t> auth_3d_frame_list;
    std::vector<std::string> auth_3d_list;
    std::vector<std::string> ex_auth_3d_list;
    std::vector<std::string> effect_rs_list;                            // AFT410
    std::vector<std::string> play_eff_list;                             // AFT410
    std::vector<std::string> stop_eff_list;                             // AFT410

    std::string aet_back;
    int32_t aet_back_frame;
    std::string aet_front;
    int32_t aet_front_frame;
    std::string aet_front_low;
    int32_t aet_front_low_frame;
    std::string aet_id_back;
    std::string aet_id_front;
    std::string aet_id_front_low;
    std::string auth_3d;
    null_int32_t cam_blur;
    std::string ex_auth_3d;
    std::string ex_stage;
    std::string light;
    null_int32_t light_frame;
    null_int32_t npr_type;
    null_int32_t sdw_off;
    std::string spr_set_back;
    std::string stage;
    null_int32_t stage_flag;
};

struct pv_db_field_aft500 {
    std::vector<int32_t> aet_back_frame_list;
    std::vector<std::string> aet_back_list;
    std::vector<int32_t> aet_front_frame_list;
    std::vector<std::string> aet_front_list;
    std::vector<int32_t> aet_front_3d_surf_frame_list;
    std::vector<std::string> aet_front_3d_surf_list;
    std::vector<int32_t> aet_front_low_frame_list;
    std::vector<std::string> aet_front_low_list;
    std::vector<std::string> aet_id_back_list;
    std::vector<std::string> aet_id_front_list;
    std::vector<std::string> aet_id_front_3d_surf_list;
    std::vector<std::string> aet_id_front_low_list;
    std::vector<int32_t> auth_3d_frame_list;
    std::vector<std::string> auth_3d_list;
    std::vector<std::string> ex_auth_3d_list;
    std::vector<std::string> effect_emision_rs_list;
    std::vector<std::string> effect_rs_list;
    std::vector<std::string> play_eff_list;
    std::vector<std::string> stop_eff_list;

    std::string aet_back;
    int32_t aet_back_frame;
    std::string aet_front;
    std::string aet_front_3d_surf;
    int32_t aet_front_3d_surf_frame;
    int32_t aet_front_frame;
    std::string aet_front_low;
    int32_t aet_front_low_frame;
    std::string aet_id_back;
    std::string aet_id_front;
    std::string aet_id_front_3d_surf;
    std::string aet_id_front_low;
    std::string auth_3d;
    null_int32_t cam_blur;
    std::string ex_auth_3d;
    std::string ex_stage;
    std::string light;
    null_int32_t light_frame;
    null_int32_t npr_type;
    null_int32_t sdw_off;
    std::string spr_set_back;
    std::string stage;
    null_int32_t stage_flag;
};

struct pv_db_mdata_ac110 {
    std::string directory;
    null_int32_t flag;
    std::string gam_aet;
    std::string gam_aet_id;
    std::string gam_spr;
    std::string sel_spr;
    std::string sel_spr_cmn;
    std::string target_shadow_type;
};

struct pv_db_mdata_ac200 {
    std::string directory;
    null_int32_t flag;
};

struct pv_db_osage_init {
    null_int32_t frame;
    std::string motion;
    std::string stage;
};

null(pv_db_performer_size)
null(pv_db_performer_type)

struct pv_db_performer_aft101 {
    null_chara_index chara;
    null_pv_db_performer_size size;
    null_pv_db_performer_type type;

    null_int32_t costume;
    null_int32_t exclude;
    null_int32_t fixed;
    null_int32_t item_back;
    null_int32_t item_face;
    null_int32_t item_neck;
    null_int32_t item_zujo;
    null_int32_t pv_costume;
};

struct pv_db_performer_aft500 {
    null_chara_index chara;
    null_pv_db_performer_size size;
    null_pv_db_performer_type type;

    null_int32_t costume;
    null_int32_t exclude;
    null_int32_t fixed;
    null_int32_t item_back;
    null_int32_t item_face;
    null_int32_t item_kami;                                             // FT
    null_int32_t item_neck;
    null_int32_t item_zujo;
    null_int32_t pv_costume;
    null_int32_t pseudo_same_id;
};

struct pv_db_pv_expression {
    std::string file_name;
};

struct pv_db_sabi {
    null_float_t play_time;
    null_float_t start_time;
};

struct pv_db_sel_2d {
    std::string bg;
    std::string jk;
    std::string logo;
    std::string plate;
};

struct pv_db_song_info_ac101 {
    std::string arranger;
    std::string guitar_player;                                          // AC130
    std::string lyrics;
    std::string manipulator;
    std::string music;
    std::string pv_editor;
};

struct pv_db_song_info_ac200 {
    std::string arranger;
    std::string guitar_player;
    std::string lyrics;
    std::string manipulator;
    std::string music;
    std::string pv_editor;
    std::pair<std::string, std::string> ex_info[4];
};

struct pv_db_stage_param_ac200 {
    std::string collision_file;
    std::string stage;
    std::string wind_file;
};

struct pv_db_stage_param_aft210 {
    std::string collision_file;
    null_int32_t mhd_id;
    std::string stage;
    std::string wind_file;
};

struct pv_db_title_image_ac101 {
    std::vector<std::string> file;                                      // AC101
    std::vector<std::string> file_name;                                 // AC101

    std::string aet_name;
    null_float_t time;
};

struct pv_db_title_image_ac200 {
    std::string aet_name;
    null_float_t end_time;
    null_float_t time;
};

struct pv_db_diff_ac101 {
    std::vector<pv_db_field_ac101> field;

    std::vector<std::string> motion;

    pv_db_sel_2d sel_2d;                                                // AC101
    pv_db_song_info_ac101 song_info;
    pv_db_title_image_ac101 title_image;

    null_int32_t level;
    std::string movie_file_name;
    std::string script_file_name;
    std::string se_name;
    null_int32_t version;
};

struct pv_db_diff_ac200 {
    std::vector<pv_db_field_ac200> field;

    std::vector<std::string> edit_effect;
    std::vector<std::string> hand_item;
    std::vector<std::string> motion;
    std::vector<std::string> motion2P;
    std::vector<std::string> pv_item;                                    // not AC200/AC210/AC211

    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_float_t edit_chara_scale;
    null_int32_t level;
    std::string movie_file_name;
    std::string script_file_name;
    std::string se_name;
    null_int32_t version;
};

null(pv_db_movie_surface)

struct pv_db_diff_aft101 {
    std::vector<pv_db_field_aft101> field;

    std::vector<std::string> edit_effect;
    std::vector<std::string> hand_item;
    std::vector<std::string> motion;
    std::vector<std::string> motion2P;
    std::vector<std::string> motion3P;
    std::vector<std::string> motion4P;
    std::vector<std::string> pv_item;

    pv_db_pv_level level;
    null_pv_db_movie_surface movie_surface;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_float_t edit_chara_scale;
    std::string effect_se_file_name;
    null_int32_t level_sort_index;
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    std::string script_file_name;
    std::string script_format;
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    null_int32_t version;
};

struct pv_db_diff_attr {
    null_int32_t extra;
    null_int32_t original;
    null_int32_t slide;
};

struct pv_db_diff_aft300 {
    std::vector<pv_db_field_aft210> field;
    std::vector<pv_db_performer_aft101> performer;

    std::vector<std::string> edit_effect;
    std::vector<std::string> edit_effect_low_field;
    std::vector<std::string> effect_se_name_list;
    std::vector<std::string> hand_item;
    std::vector<std::string> motion;
    std::vector<std::string> motion2P;
    std::vector<std::string> motion3P;
    std::vector<std::string> motion4P;
    std::vector<std::string> motion5P;                                  // AFT410
    std::vector<std::string> motion6P;                                  // AFT410
    std::vector<std::string> movie_list;
    std::vector<std::string> pv_item;

    pv_db_diff_attr attribute;
    pv_db_pv_level level;
    null_pv_db_movie_surface movie_surface;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_int32_t edition;
    null_int32_t is_ps4dlc;                                             // FT/M39
    null_float_t edit_chara_scale;
    std::string effect_se_file_name;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t level_sort_index;
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    std::string script_file_name;
    std::string script_format;
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    null_float_t sudden_timing;
    null_int32_t version;
};

struct pv_db_difficulty_aft300 {
    std::vector<pv_db_diff_aft300> diff[5];
    pv_db_diff_attr attribute;
};

struct pv_db_diff_aft500 {
    std::vector<pv_db_field_aft500> field;
    std::vector<pv_db_performer_aft500> performer;

    std::vector<std::string> edit_effect;
    std::vector<std::string> edit_effect_low_field;
    std::vector<std::string> effect_se_name_list;
    std::vector<std::string> hand_item;
    std::vector<std::string> motion;
    std::vector<std::string> motion2P;
    std::vector<std::string> motion3P;
    std::vector<std::string> motion4P;
    std::vector<std::string> motion5P;
    std::vector<std::string> motion6P;
    std::vector<std::string> movie_list;
    std::vector<std::string> pv_item;

    pv_db_diff_attr attribute;
    pv_db_pv_level level;
    null_pv_db_movie_surface movie_surface;
    pv_db_song_info_ac200 song_info_en;                                 // FT/M39
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_int32_t edition;
    null_float_t edit_chara_scale;
    std::string effect_se_file_name;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_ps4_dlc;                                            // FT/M39
    null_int32_t level_sort_index;
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    std::string script_file_name;
    std::string script_format;
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    null_float_t sudden_timing;
    null_int32_t version;
};

struct pv_db_difficulty_aft500 {
    std::vector<pv_db_diff_aft500> diff[5];
    pv_db_diff_attr attribute;
};

struct pv_db_pv_ac101 {
    std::vector<pv_db_ex_song_ac101> ex_song;
    std::vector<pv_db_field_ac200> field;
    std::vector<pv_db_osage_init> osage_init;                           // AC120/AC130

    std::vector<std::string> lyric;
    std::vector<std::string> motion;

    pv_db_diff_ac101 difficulty[5];
    pv_db_mdata_ac110 mdata;                                            // AC110/AC120/AC130
    pv_db_sabi sabi;
    pv_db_sel_2d sel_2d;                                                // AC101
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;
    null_chara_index unlock;

    null_int32_t bpm;
    null_int32_t date;
    null_int32_t demo_flag;                                             // AC110/AC120/AC130
    null_int32_t m_data_flag;                                           // AC101
    std::string movie_file_name;
    std::string se_name;
    std::string song_file_name;
    std::string song_name;
    std::string song_name_reading;
};

struct pv_db_pv_ac200 {
    std::vector<pv_db_ex_song_ac101> ex_song;
    std::vector<pv_db_field_ac200> field;
    std::vector<pv_db_osage_init> osage_init;
    std::vector<pv_db_stage_param_ac200> stage_param;

    std::vector<std::string> edit_effect;
    std::vector<std::string> hand_item;
    std::vector<std::string> lyric;
    std::vector<std::string> motion;
    std::vector<std::string> motion2P;
    std::vector<std::string> pv_item;                                   // not AC200/AC210/AC211

    pv_db_diff_ac200 difficulty[5];
    pv_db_disp2d_ac200 disp2d;
    pv_db_duet duet;
    pv_db_mdata_ac200 mdata;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;
    null_chara_index unlock1p;
    null_chara_index unlock2p;

    null_int32_t bpm;
    null_int32_t date;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    std::string movie_file_name;
    null_int32_t remix_parent;
    std::string se_name;
    std::string song_file_name;
    std::string song_name;
    std::string song_name_reading;
    null_bool use_osage_play_data;                                      // not AC200
};

null(pv_db_eyes_base_adjust_type)

struct pv_db_pv_aft101 {
    std::vector<pv_db_chreff> chreff;
    std::vector<pv_db_ex_song_aft101> ex_song;
    std::vector<pv_db_eyes_rot_rate> eyes_rot_rate;                     // AFT200
    std::vector<pv_db_field_aft101> field;
    std::vector<pv_db_osage_init> osage_init;
    std::vector<pv_db_performer_aft101> performer;
    std::vector<pv_db_stage_param_ac200> stage_param;

    std::vector<std::string> edit_effect;
    std::vector<std::string> effect_se_name_list;
    std::vector<std::string> hand_item;
    std::vector<std::string> lyric;
    std::vector<std::string> motion;
    std::vector<std::string> motion2p;
    std::vector<std::string> motion3p;
    std::vector<std::string> motion4p;
    std::vector<std::string> pv_item;

    pv_db_diff_aft101 difficulty[5];
    pv_db_disp2d_ac200 disp2d;
    null_pv_db_eyes_base_adjust_type eyes_base_adjust_type;             // AFT200
    pv_db_mdata_ac200 mdata;
    null_pv_db_movie_surface movie_surface;
    pv_db_pv_expression pv_expression;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_int32_t bpm;
    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    std::string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_int32_t is_old_pv;                                             // AFT200
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    null_int32_t remix_parent;
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    std::string song_file_name;
    std::string song_name;
    std::string song_name_reading;
    null_bool use_osage_play_data;
};

struct pv_db_pv_aft210 {
    std::vector<pv_db_chreff> chreff;
    std::vector<pv_db_ex_song_aft101> ex_song;
    std::vector<pv_db_eyes_rot_rate> eyes_rot_rate;
    std::vector<pv_db_field_aft210> field;
    std::vector<pv_db_osage_init> osage_init;
    std::vector<pv_db_performer_aft101> performer;
    std::vector<pv_db_stage_param_aft210> stage_param;

    std::vector<std::string> edit_effect;
    std::vector<std::string> effect_se_name_list;
    std::vector<std::string> hand_item;
    std::vector<std::string> lyric;
    std::vector<std::string> motion;
    std::vector<std::string> motion2p;
    std::vector<std::string> motion3p;
    std::vector<std::string> motion4p;
    std::vector<std::string> pv_item;

    pv_db_diff_aft101 difficulty[5];
    pv_db_disp2d_aft210 disp2d;
    null_pv_db_eyes_base_adjust_type eyes_base_adjust_type;
    pv_db_mdata_ac200 mdata;
    null_pv_db_movie_surface movie_surface;
    pv_db_pv_expression pv_expression;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_int32_t bpm;
    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    std::string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_int32_t is_old_pv;
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    null_int32_t remix_parent;
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    std::string song_file_name;
    std::string song_name;
    std::string song_name_reading;
    null_bool use_osage_play_data;
};

struct pv_db_pv_aft300 {
    std::vector<pv_db_another_song_aft310> another_song;                // not AFT300
    std::vector<pv_db_chrcam> chrcam;                                   // not AFT300
    std::vector<pv_db_chreff> chreff;
    std::vector<pv_db_ex_song_aft300> ex_song;
    std::vector<pv_db_eyes_rot_rate> eyes_rot_rate;
    std::vector<pv_db_field_aft210> field;
    std::vector<pv_db_osage_init> osage_init;
    std::vector<pv_db_performer_aft101> performer;
    std::vector<pv_db_stage_param_aft210> stage_param;

    std::vector<std::string> edit_effect;
    std::vector<std::string> edit_effect_low_field;                     // not AFT300
    std::vector<std::string> effect_se_name_list;
    std::vector<std::string> hand_item;
    std::vector<std::string> lyric;
    std::vector<std::string> motion;
    std::vector<std::string> motion2p;
    std::vector<std::string> motion3p;
    std::vector<std::string> motion4p;
    std::vector<std::string> motion5p;                                  // AFT410
    std::vector<std::string> motion6p;                                  // AFT410
    std::vector<std::string> pv_item;

    pv_db_difficulty_aft300 difficulty;
    pv_db_disp2d_aft210 disp2d;
    null_pv_db_eyes_base_adjust_type eyes_base_adjust_type;
    pv_db_mdata_ac200 mdata;
    null_pv_db_movie_surface movie_surface;
    pv_db_pv_expression pv_expression;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_int32_t bpm;
    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    std::string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_old_pv;
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    null_int32_t pre_play_script;                                       // AFT410
    null_int32_t remix_parent;
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    std::string song_file_name;
    std::string song_name;
    std::string song_name_reading;
    null_float_t sudden_timing;
    null_bool use_osage_play_data;
};

null(pv_db_frame_texture_type)

struct pv_db_pv_aft500 {
    std::vector<pv_db_another_song_aft500> another_song;
    std::vector<pv_db_auth_replace_by_module> auth_replace_by_module;   // AFT600/AFT601/AFT610/AFT620/AFT621/AFT622/AFT701/FT/M39
    std::vector<pv_db_chrcam> chrcam;
    std::vector<pv_db_chreff> chreff;
    std::vector<pv_db_chrmot> chrmot;                                   // AFT620/AFT621/AFT622/AFT701/FT/M39
    std::vector<pv_db_ex_song_aft300> ex_song;
    std::vector<pv_db_eyes_rot_rate> eyes_rot_rate;
    std::vector<pv_db_field_aft500> field;
    std::vector<pv_db_osage_init> osage_init;
    std::vector<pv_db_performer_aft500> performer;
    std::vector<pv_db_stage_param_aft210> stage_param;

    std::vector<std::string> edit_effect;
    std::vector<std::string> edit_effect_low_field;
    std::vector<std::string> effect_se_name_list;
    std::vector<std::string> hand_item;
    std::vector<std::string> lyric;
    std::vector<std::string> lyric_en;                                  // FT/M39
    std::vector<std::string> motion;
    std::vector<std::string> motion2P;
    std::vector<std::string> motion3P;
    std::vector<std::string> motion4P;
    std::vector<std::string> motion5P;
    std::vector<std::string> motion6P;
    std::vector<std::string> movie_list;
    std::vector<std::string> pv_item;

    pv_db_difficulty_aft300 difficulty;
    pv_db_disp2d_aft210 disp2d;
    null_pv_db_eyes_base_adjust_type eyes_base_adjust_type;
    null_pv_db_frame_texture_type frame_texture_a_type;
    null_pv_db_frame_texture_type frame_texture_b_type;
    null_pv_db_frame_texture_type frame_texture_c_type;
    null_pv_db_frame_texture_type frame_texture_d_type;
    null_pv_db_frame_texture_type frame_texture_e_type;
    null_pv_db_frame_texture_type frame_texture_type;
    pv_db_mdata_ac200 mdata;
    null_pv_db_movie_surface movie_surface;
    pv_db_pv_expression pv_expression;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info_en;                                 // FT/M39
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_int32_t bpm;
    std::string chainslide_failure_name;
    std::string chainslide_first_name;
    std::string chainslide_sub_name;
    std::string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    std::string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    std::string frame_texture;
    std::string frame_texture_a;
    std::string frame_texture_b;
    std::string frame_texture_c;
    std::string frame_texture_d;
    std::string frame_texture_e;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_old_pv;
    std::string movie_file_name;
    std::string pvbranch_success_se_name;
    null_int32_t pack;                                                  // FT/M39
    null_int32_t pre_play_script;
    null_int32_t rank_board_id;                                         // FT/M39
    null_int32_t remix_parent;
    null_float_t resolution_scale;                                      // FT/M39
    null_float_t resolution_scale_neo;                                  // FT/M39
    std::string se_name;
    std::string slide_name;
    std::string slidertouch_name;
    std::string song_file_name;
    std::string song_name;
    std::string song_name_en;                                           // FT/M39
    std::string song_name_reading;
    std::string song_name_reading_en;                                   // FT/M39
    null_float_t sudden_timing;
    null_bool use_osage_play_data;
};
