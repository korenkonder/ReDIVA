/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "vector.h"

typedef enum pv_db_chara {
    PV_DB_CHARA_MIK = 0,
    PV_DB_CHARA_RIN = 1,
    PV_DB_CHARA_LEN = 2,
    PV_DB_CHARA_LUK = 3,
    PV_DB_CHARA_NER = 4,
    PV_DB_CHARA_HAK = 5,
    PV_DB_CHARA_KAI = 6,
    PV_DB_CHARA_MEI = 7,
    PV_DB_CHARA_SAK = 8,
    PV_DB_CHARA_TET = 9,
} pv_db_chara;

typedef enum pv_db_chreff_type {
    PV_DB_CHARA_EFFECT_AUTH3D     = 0,
    PV_DB_CHARA_EFFECT_AUTH3D_OBJ = 1,
} pv_db_chreff_type;

typedef enum pv_db_format {
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
} pv_db_format;

typedef enum pv_db_duet_type {
    PV_DB_DUET_USER_SETTING   = 0,
    PV_DB_DUET_FIXED_SIZE     = 1,
    PV_DB_DUET_FIXED_SUB      = 2,
    PV_DB_DUET_FIXED_ALL      = 3,
    PV_DB_DUET_PSEUDO_DEFAULT = 4,
    PV_DB_DUET_PSEUDO_SAME    = 5,
    PV_DB_DUET_PSEUDO_SWIM    = 6,
    PV_DB_DUET_PSEUDO_SWIM_S  = 7,
} pv_db_duet_type;

typedef enum pv_db_eyes_base_adjust_type {
    PV_DB_EYES_BASE_ADJUST_DIRECTION = 0,
    PV_DB_EYES_BASE_ADJUST_CLEARANCE = 1,
    PV_DB_EYES_BASE_ADJUST_OFF       = 2,
} pv_db_eyes_base_adjust_type;

typedef enum pv_db_frame_texture_type {
    PV_DB_FRAME_TEXTURE_PRE_PP  = 0,
    PV_DB_FRAME_TEXTURE_POST_PP = 1,
    PV_DB_FRAME_TEXTURE_FB      = 2,
} pv_db_frame_texture_type;

typedef enum pv_db_movie_surface {
    PV_DB_MOVIE_SURFACE_BACK  = 0,
    PV_DB_MOVIE_SURFACE_FRONT = 1,
} pv_db_movie_surface;

typedef enum pv_db_performer_size {
    PV_DB_PERFORMER_SIZE_NORMAL     = 0,
    PV_DB_PERFORMER_SIZE_PLAY_CHARA = 1,
    PV_DB_PERFORMER_SIZE_PV_CHARA   = 2,
    PV_DB_PERFORMER_SIZE_SHORT      = 3,
    PV_DB_PERFORMER_SIZE_TALL       = 4,
} pv_db_performer_size;

typedef enum pv_db_performer_type {
    PV_DB_PERFORMER_VOCAL          = 0,
    PV_DB_PERFORMER_PSEUDO_DEFAULT = 1,
    PV_DB_PERFORMER_PSEUDO_SAME    = 2,
    PV_DB_PERFORMER_PSEUDO_SWIM    = 3,
    PV_DB_PERFORMER_PSEUDO_SWIM_S  = 4,
    PV_DB_PERFORMER_PSEUDO_MYCHARA = 5,
    PV_DB_PERFORMER_GUEST          = 6,
} pv_db_performer_type;

typedef enum pv_db_pv_level {
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
} pv_db_pv_level;

typedef struct pv_db_another_song_aft310 {
    string name;
    string song_file_name;
} pv_db_another_song_aft310;

typedef struct pv_db_another_song_aft500 {
    string name;
    string name_en;                                                 // FT/M39
    string song_file_name;
    string vocal_chara_num;                                         // FT/M39
    string vocal_disp_name;                                         // FT/M39
    string vocal_disp_name_en;                                      // FT/M39
} pv_db_another_song_aft500;

typedef struct pv_db_auth_replace_by_module {
    null_int32_t id;
    null_int32_t module_id;
    string name;
    string org_name;
} pv_db_auth_replace_by_module;

null(pv_db_chara)

typedef struct pv_db_chrcam {
    null_pv_db_chara chara;

    null_int32_t id;
    string name;
    string org_name;
} pv_db_chrcam;

null(pv_db_chreff_type)

typedef struct pv_db_chreff_data {
    null_pv_db_chreff_type type;

    string Name;
} pv_db_chreff_data;

vector_old(pv_db_chreff_data)

typedef struct pv_db_chreff {
    vector_old_pv_db_chreff_data data;

    null_int32_t id;
    string name;
} pv_db_chreff;

typedef struct pv_db_chrmot {
    null_pv_db_chara chara;

    null_int32_t id;
    string name;
    string org_name;
} pv_db_chrmot;

typedef struct pv_db_disp2d_ac200 {
    string set_name;
    null_int32_t target_shadow_type;
    null_int32_t title_end_2d_field;
    null_int32_t title_end_2d_low_field;                            // not AC200
    null_int32_t title_end_3d_field;
    null_int32_t title_start_2d_field;
    null_int32_t title_start_2d_low_field;                          // not AC200
    null_int32_t title_start_3d_field;
} pv_db_disp2d_ac200;

typedef struct pv_db_disp2d_aft210 {
    string set_name;
    null_int32_t target_shadow_type;
    null_int32_t title_2d_layer;
    null_int32_t title_end_2d_field;
    null_int32_t title_end_2d_low_field;
    null_int32_t title_end_3d_field;
    null_int32_t title_start_2d_field;
    null_int32_t title_start_2d_low_field;
    null_int32_t title_start_3d_field;
} pv_db_disp2d_aft210;

null(pv_db_duet_type)

typedef struct pv_db_duet {
    null_pv_db_duet_type type;
    null_int32_t num;
} pv_db_duet;

typedef struct pv_db_eyes_rot_rate {
    null_pv_db_chara chr;
    null_float_t xn_rate;
    null_float_t xp_rate;
} pv_db_eyes_rot_rate;

typedef struct pv_db_ex_auth {
    string name;
    string org_name;
} pv_db_ex_auth;

typedef struct pv_db_ex_song_ac101 {
    null_pv_db_chara chara;

    string file;
    string name;
} pv_db_ex_song_ac101;

typedef struct pv_db_ex_song_aft101 {
    null_pv_db_chara chara;
    null_pv_db_chara chara2P;                                       // not AFT101
    null_pv_db_chara chara3P;                                       // not AFT101
    null_pv_db_chara chara4P;                                       // not AFT101

    string file;
    string name;
} pv_db_ex_song_aft101;

vector_old(pv_db_ex_auth)

typedef struct pv_db_ex_song_aft300 {
    vector_old_pv_db_ex_auth ex_auth;                               // AFT621/AFT622/AFT701/FT/M39

    null_pv_db_chara chara;
    null_pv_db_chara chara2P;
    null_pv_db_chara chara3P;
    null_pv_db_chara chara4P;
    null_pv_db_chara chara5P;                                       // not AFT300/AFT310/AFT400
    null_pv_db_chara chara6P;                                       // not AFT300/AFT310/AFT400

    string file;
    string name;
} pv_db_ex_song_aft300;

typedef struct pv_db_field_ac101 {
    string aet_back;
    string aet_front;
    string aet_id_back;
    string aet_id_front;
    string auth_3d;
    string spr_set_back;
    string stage;
} pv_db_field_ac101;

typedef struct pv_db_field_ac200 {
    vector_old_string auth_3d_list;                                 // not AC200/AC210/AC211
    vector_old_int32_t auth_3d_frame_list;                          // AC520/AC521/AC600
    vector_old_string ex_auth_3d_list;                              // AC510/AC511/AC520/AC521/AC600

    string aet_back;
    string aet_front;
    string aet_front_low;                                           // not AC200
    string aet_id_back;
    string aet_id_front;
    string aet_id_front_low;                                        // not AC200
    string auth_3d;
    string ex_auth_3d;                                              // AC510/AC511/AC520/AC521/AC600
    string ex_stage;                                                // AC500/AC510/AC511/AC520/AC521/AC600
    string spr_set_back;
    string stage;
    null_int32_t stage_flag;                                        // AC520/AC521/AC600
} pv_db_field_ac200;

typedef struct pv_db_field_ac500 {
    vector_old_string auth_3d_list;

    string aet_back;
    string aet_front;
    string aet_front_low;
    string aet_id_back;
    string aet_id_front;
    string aet_id_front_low;
    string auth_3d;
    string ex_auth_3d;                                              // not AC500
    string ex_stage;
    string spr_set_back;
    string stage;
    null_int32_t stage_flag;                                        // AC520/AC521/AC600
} pv_db_field_ac500;

typedef struct pv_db_field_aft101 {
    vector_old_string auth_3d_list;
    vector_old_int32_t auth_3d_frame_list;
    vector_old_string ex_auth_3d_list;

    string aet_back;
    string aet_front;
    string aet_front_low;
    string aet_id_back;
    string aet_id_front;
    string aet_id_front_low;
    string auth_3d;
    null_int32_t cam_blur;                                          // AFT110/AFT120/AFT200
    string ex_auth_3d;
    string ex_stage;
    string light;                                                   // AFT100
    null_int32_t npr_type;                                          // AFT110/AFT120/AFT200
    null_int32_t sdw_off;                                           // AFT110/AFT120/AFT200
    string spr_set_back;
    string stage;
    null_int32_t stage_flag;
} pv_db_field_aft101;

typedef struct pv_db_field_aft210 {
    vector_old_int32_t aet_back_frame_list;
    vector_old_string aet_back_list;
    vector_old_int32_t aet_front_frame_list;
    vector_old_string aet_front_list;
    vector_old_int32_t aet_front_low_frame_list;
    vector_old_string aet_front_low_list;
    vector_old_string aet_id_back_list;
    vector_old_string aet_id_front_list;
    vector_old_string aet_id_front_low_list;
    vector_old_int32_t auth_3d_frame_list;
    vector_old_string auth_3d_list;
    vector_old_string ex_auth_3d_list;
    vector_old_string effect_rs_list;                               // AFT410
    vector_old_string play_eff_list;                                // AFT410
    vector_old_string stop_eff_list;                                // AFT410

    string aet_back;
    int32_t aet_back_frame;
    string aet_front;
    int32_t aet_front_frame;
    string aet_front_low;
    int32_t aet_front_low_frame;
    string aet_id_back;
    string aet_id_front;
    string aet_id_front_low;
    string auth_3d;
    null_int32_t cam_blur;
    string ex_auth_3d;
    string ex_stage;
    string light;
    null_int32_t light_frame;
    null_int32_t npr_type;
    null_int32_t sdw_off;
    string spr_set_back;
    string stage;
    null_int32_t stage_flag;
} pv_db_field_aft210;

typedef struct pv_db_field_aft500 {
    vector_old_int32_t aet_back_frame_list;
    vector_old_string aet_back_list;
    vector_old_int32_t aet_front_frame_list;
    vector_old_string aet_front_list;
    vector_old_int32_t aet_front_3d_surf_frame_list;
    vector_old_string aet_front_3d_surf_list;
    vector_old_int32_t aet_front_low_frame_list;
    vector_old_string aet_front_low_list;
    vector_old_string aet_id_back_list;
    vector_old_string aet_id_front_list;
    vector_old_string aet_id_front_3d_surf_list;
    vector_old_string aet_id_front_low_list;
    vector_old_int32_t auth_3d_frame_list;
    vector_old_string auth_3d_list;
    vector_old_string ex_auth_3d_list;
    vector_old_string effect_emision_rs_list;
    vector_old_string effect_rs_list;
    vector_old_string play_eff_list;
    vector_old_string stop_eff_list;

    string aet_back;
    int32_t aet_back_frame;
    string aet_front;
    string aet_front_3d_surf;
    int32_t aet_front_3d_surf_frame;
    int32_t aet_front_frame;
    string aet_front_low;
    int32_t aet_front_low_frame;
    string aet_id_back;
    string aet_id_front;
    string aet_id_front_3d_surf;
    string aet_id_front_low;
    string auth_3d;
    null_int32_t cam_blur;
    string ex_auth_3d;
    string ex_stage;
    string light;
    null_int32_t light_frame;
    null_int32_t npr_type;
    null_int32_t sdw_off;
    string spr_set_back;
    string stage;
    null_int32_t stage_flag;
} pv_db_field_aft500;

typedef struct pv_db_mdata_ac110 {
    string directory;
    null_int32_t flag;
    string gam_aet;
    string gam_aet_id;
    string gam_spr;
    string sel_spr;
    string sel_spr_cmn;
    string target_shadow_type;
} pv_db_mdata_ac110;

typedef struct pv_db_mdata_ac200 {
    string directory;
    null_int32_t flag;
} pv_db_mdata_ac200;

typedef struct pv_db_osage_init {
    null_int32_t frame;
    string motion;
    string stage;
} pv_db_osage_init;

null(pv_db_performer_size)
null(pv_db_performer_type)

typedef struct pv_db_performer_aft101 {
    null_pv_db_chara chara;
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
} pv_db_performer_aft101;

typedef struct pv_db_performer_aft500 {
    null_pv_db_chara chara;
    null_pv_db_performer_size size;
    null_pv_db_performer_type type;

    null_int32_t costume;
    null_int32_t exclude;
    null_int32_t fixed;
    null_int32_t item_back;
    null_int32_t item_face;
    null_int32_t item_kami;                                         // FT
    null_int32_t item_neck;
    null_int32_t item_zujo;
    null_int32_t pv_costume;
    null_int32_t pseudo_same_id;
} pv_db_performer_aft500;

typedef struct pv_db_pv_expression {
    string file_name;
} pv_db_pv_expression;

typedef struct pv_db_sabi {
    null_float_t play_time;
    null_float_t start_time;
} pv_db_sabi;

typedef struct pv_db_sel_2d {
    string bg;
    string jk;
    string logo;
    string plate;
} pv_db_sel_2d;

typedef struct pv_db_song_info_ac101 {
    string arranger;
    string guitar_player;                                           // AC130
    string lyrics;
    string manipulator;
    string music;
    string pv_editor;
} pv_db_song_info_ac101;

typedef struct pv_db_song_info_ac200 {
    string arranger;
    string guitar_player;
    string lyrics;
    string manipulator;
    string music;
    string pv_editor;
    string ex_info0key;
    string ex_info0val;
    string ex_info1key;
    string ex_info1val;
    string ex_info2key;
    string ex_info2val;
    string ex_info3key;
    string ex_info3val;
} pv_db_song_info_ac200;

typedef struct pv_db_stage_param_ac200 {
    string collision_file;
    string stage;
    string wind_file;
} pv_db_stage_param_ac200;

typedef struct pv_db_stage_param_aft210 {
    string collision_file;
    null_int32_t mhd_id;
    string stage;
    string wind_file;
} pv_db_stage_param_aft210;

typedef struct pv_db_title_image_ac101 {
    vector_old_string file;                                         // AC101
    vector_old_string file_name;                                    // AC101

    string aet_name;
    null_float_t time;
} pv_db_title_image_ac101;

typedef struct pv_db_title_image_ac200 {
    string aet_name;
    null_float_t end_time;
    null_float_t time;
} pv_db_title_image_ac200;

vector_old(pv_db_field_ac101)

typedef struct pv_db_diff_ac101 {
    vector_old_pv_db_field_ac101 field;

    vector_old_string motion;

    pv_db_sel_2d sel_2d;                                            // AC101
    pv_db_song_info_ac101 song_info;
    pv_db_title_image_ac101 title_image;

    null_int32_t level;
    string movie_file_name;
    string script_file_name;
    string se_name;
    null_int32_t version;
} pv_db_diff_ac101;

vector_old(pv_db_field_ac200)

typedef struct pv_db_diff_ac200 {
    vector_old_pv_db_field_ac200 field;

    vector_old_string edit_effect;
    vector_old_string hand_item;
    vector_old_string motion;
    vector_old_string motion2P;
    vector_old_string pv_item;                                      // not AC200/AC210/AC211

    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_float_t edit_chara_scale;
    null_int32_t level;
    string movie_file_name;
    string script_file_name;
    string se_name;
    null_int32_t version;
} pv_db_diff_ac200;

null(pv_db_movie_surface)

vector_old(pv_db_field_aft101)

typedef struct pv_db_diff_aft101 {
    vector_old_pv_db_field_aft101 field;

    vector_old_string edit_effect;
    vector_old_string hand_item;
    vector_old_string motion;
    vector_old_string motion2P;
    vector_old_string motion3P;
    vector_old_string motion4P;
    vector_old_string pv_item;

    pv_db_pv_level level;
    null_pv_db_movie_surface movie_surface;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_float_t edit_chara_scale;
    string effect_se_file_name;
    null_int32_t level_sort_index;
    string movie_file_name;
    string pvbranch_success_se_name;
    string script_file_name;
    string script_format;
    string se_name;
    string slide_name;
    string slidertouch_name;
    null_int32_t version;
} pv_db_diff_aft101;

typedef struct pv_db_diff_attr {
    null_int32_t extra;
    null_int32_t original;
    null_int32_t slide;
} pv_db_diff_attr;

vector_old(pv_db_field_aft210)
vector_old(pv_db_performer_aft101)

typedef struct pv_db_diff_aft300 {
    vector_old_pv_db_field_aft210 field;
    vector_old_pv_db_performer_aft101 performer;

    vector_old_string edit_effect;
    vector_old_string edit_effect_low_field;
    vector_old_string effect_se_name_list;
    vector_old_string hand_item;
    vector_old_string motion;
    vector_old_string motion2P;
    vector_old_string motion3P;
    vector_old_string motion4P;
    vector_old_string motion5P;                                     // AFT410
    vector_old_string motion6P;                                     // AFT410
    vector_old_string movie_list;
    vector_old_string pv_item;

    pv_db_diff_attr attribute;
    pv_db_pv_level level;
    null_pv_db_movie_surface movie_surface;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_int32_t edition;
    null_int32_t is_ps4dlc;                                         // FT/M39
    null_float_t edit_chara_scale;
    string effect_se_file_name;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t level_sort_index;
    string movie_file_name;
    string pvbranch_success_se_name;
    string script_file_name;
    string script_format;
    string se_name;
    string slide_name;
    string slidertouch_name;
    null_float_t sudden_timing;
    null_int32_t version;
} pv_db_diff_aft300;

vector_old(pv_db_diff_aft300)

typedef struct pv_db_difficulty_aft300 {
    vector_old_pv_db_diff_aft300 diff[5];
    pv_db_diff_attr attribute;
} pv_db_difficulty_aft300;

vector_old(pv_db_field_aft500)
vector_old(pv_db_performer_aft500)

typedef struct pv_db_diff_aft500 {
    vector_old_pv_db_field_aft500 field;
    vector_old_pv_db_performer_aft500 performer;

    vector_old_string edit_effect;
    vector_old_string edit_effect_low_field;
    vector_old_string effect_se_name_list;
    vector_old_string hand_item;
    vector_old_string motion;
    vector_old_string motion2P;
    vector_old_string motion3P;
    vector_old_string motion4P;
    vector_old_string motion5P;
    vector_old_string motion6P;
    vector_old_string movie_list;
    vector_old_string pv_item;

    pv_db_diff_attr attribute;
    pv_db_pv_level level;
    null_pv_db_movie_surface movie_surface;
    pv_db_song_info_ac200 song_info_en;                             // FT/M39
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_int32_t edition;
    null_float_t edit_chara_scale;
    string effect_se_file_name;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_ps4_dlc;                                        // FT/M39
    null_int32_t level_sort_index;
    string movie_file_name;
    string pvbranch_success_se_name;
    string script_file_name;
    string script_format;
    string se_name;
    string slide_name;
    string slidertouch_name;
    null_float_t sudden_timing;
    null_int32_t version;
} pv_db_diff_aft500;

vector_old(pv_db_diff_aft500)

typedef struct pv_db_difficulty_aft500 {
    vector_old_pv_db_diff_aft500 diff[5];
    pv_db_diff_attr attribute;
} pv_db_difficulty_aft500;

vector_old(pv_db_ex_song_ac101)
vector_old(pv_db_osage_init)

typedef struct pv_db_pv_ac101 {
    vector_old_pv_db_ex_song_ac101 ex_song;
    vector_old_pv_db_field_ac200 field;
    vector_old_pv_db_osage_init osage_init;                         // AC120/AC130

    vector_old_string lyric;
    vector_old_string motion;

    pv_db_diff_ac101 difficulty[5];
    pv_db_mdata_ac110 mdata;                                        // AC110/AC120/AC130
    pv_db_sabi sabi;
    pv_db_sel_2d sel_2d;                                            // AC101
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;
    null_pv_db_chara unlock;

    null_int32_t bpm;
    null_int32_t date;
    null_int32_t demo_flag;                                         // AC110/AC120/AC130
    null_int32_t m_data_flag;                                       // AC101
    string movie_file_name;
    string se_name;
    string song_file_name;
    string song_name;
    string song_name_reading;
} pv_db_pv_ac101;

vector_old(pv_db_stage_param_ac200)

typedef struct pv_db_pv_ac200 {
    vector_old_pv_db_ex_song_ac101 ex_song;
    vector_old_pv_db_field_ac200 field;
    vector_old_pv_db_osage_init osage_init;
    vector_old_pv_db_stage_param_ac200 stage_param;

    vector_old_string edit_effect;
    vector_old_string hand_item;
    vector_old_string lyric;
    vector_old_string motion;
    vector_old_string motion2P;
    vector_old_string pv_item;                                      // not AC200/AC210/AC211

    pv_db_diff_ac200 difficulty[5];
    pv_db_disp2d_ac200 disp2d;
    pv_db_duet duet;
    pv_db_mdata_ac200 mdata;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;
    null_pv_db_chara unlock1p;
    null_pv_db_chara unlock2p;

    null_int32_t bpm;
    null_int32_t date;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    string movie_file_name;
    null_int32_t remix_parent;
    string se_name;
    string song_file_name;
    string song_name;
    string song_name_reading;
    null_bool use_osage_play_data;                                  // not AC200
} pv_db_pv_ac200;

null(pv_db_eyes_base_adjust_type)

vector_old(pv_db_chreff)
vector_old(pv_db_ex_song_aft101)
vector_old(pv_db_eyes_rot_rate)

typedef struct pv_db_pv_aft101 {
    vector_old_pv_db_chreff chreff;
    vector_old_pv_db_ex_song_aft101 ex_song;
    vector_old_pv_db_eyes_rot_rate eyes_rot_rate;                   // AFT200
    vector_old_pv_db_field_aft101 field;
    vector_old_pv_db_osage_init osage_init;
    vector_old_pv_db_performer_aft101 performer;
    vector_old_pv_db_stage_param_ac200 stage_param;

    vector_old_string edit_effect;
    vector_old_string effect_se_name_list;
    vector_old_string hand_item;
    vector_old_string lyric;
    vector_old_string motion;
    vector_old_string motion2p;
    vector_old_string motion3p;
    vector_old_string motion4p;
    vector_old_string pv_item;

    pv_db_diff_aft101 difficulty[5];
    pv_db_disp2d_ac200 disp2d;
    null_pv_db_eyes_base_adjust_type eyes_base_adjust_type;         // AFT200
    pv_db_mdata_ac200 mdata;
    null_pv_db_movie_surface movie_surface;
    pv_db_pv_expression pv_expression;
    pv_db_sabi sabi;
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_int32_t bpm;
    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_int32_t is_old_pv;                                         // AFT200
    string movie_file_name;
    string pvbranch_success_se_name;
    null_int32_t remix_parent;
    string se_name;
    string slide_name;
    string slidertouch_name;
    string song_file_name;
    string song_name;
    string song_name_reading;
    null_bool use_osage_play_data;
} pv_db_pv_aft101;

vector_old(pv_db_stage_param_aft210)

typedef struct pv_db_pv_aft210 {
    vector_old_pv_db_chreff chreff;
    vector_old_pv_db_ex_song_aft101 ex_song;
    vector_old_pv_db_eyes_rot_rate eyes_rot_rate;
    vector_old_pv_db_field_aft210 field;
    vector_old_pv_db_osage_init osage_init;
    vector_old_pv_db_performer_aft101 performer;
    vector_old_pv_db_stage_param_aft210 stage_param;

    vector_old_string edit_effect;
    vector_old_string effect_se_name_list;
    vector_old_string hand_item;
    vector_old_string lyric;
    vector_old_string motion;
    vector_old_string motion2p;
    vector_old_string motion3p;
    vector_old_string motion4p;
    vector_old_string pv_item;

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
    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_int32_t is_old_pv;
    string movie_file_name;
    string pvbranch_success_se_name;
    null_int32_t remix_parent;
    string se_name;
    string slide_name;
    string slidertouch_name;
    string song_file_name;
    string song_name;
    string song_name_reading;
    null_bool use_osage_play_data;
} pv_db_pv_aft210;

vector_old(pv_db_another_song_aft310)
vector_old(pv_db_chrcam)
vector_old(pv_db_ex_song_aft300)

typedef struct pv_db_pv_aft300 {
    vector_old_pv_db_another_song_aft310 another_song;              // not AFT300
    vector_old_pv_db_chrcam chrcam;                                 // not AFT300
    vector_old_pv_db_chreff chreff;
    vector_old_pv_db_ex_song_aft300 ex_song;
    vector_old_pv_db_eyes_rot_rate eyes_rot_rate;
    vector_old_pv_db_field_aft210 field;
    vector_old_pv_db_osage_init osage_init;
    vector_old_pv_db_performer_aft101 performer;
    vector_old_pv_db_stage_param_aft210 stage_param;

    vector_old_string edit_effect;
    vector_old_string edit_effect_low_field;                        // not AFT300
    vector_old_string effect_se_name_list;
    vector_old_string hand_item;
    vector_old_string lyric;
    vector_old_string motion;
    vector_old_string motion2p;
    vector_old_string motion3p;
    vector_old_string motion4p;
    vector_old_string motion5p;                                     // AFT410
    vector_old_string motion6p;                                     // AFT410
    vector_old_string pv_item;

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
    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_old_pv;
    string movie_file_name;
    string pvbranch_success_se_name;
    null_int32_t pre_play_script;                                   // AFT410
    null_int32_t remix_parent;
    string se_name;
    string slide_name;
    string slidertouch_name;
    string song_file_name;
    string song_name;
    string song_name_reading;
    null_float_t sudden_timing;
    null_bool use_osage_play_data;
} pv_db_pv_aft300;

null(pv_db_frame_texture_type)

vector_old(pv_db_another_song_aft500)
vector_old(pv_db_auth_replace_by_module)
vector_old(pv_db_chrmot)

typedef struct pv_db_pv_aft500 {
    vector_old_pv_db_another_song_aft500 another_song;
    vector_old_pv_db_auth_replace_by_module auth_replace_by_module; // AFT600/AFT601/AFT610/AFT620/AFT621/AFT622/AFT701/FT/M39
    vector_old_pv_db_chrcam chrcam;
    vector_old_pv_db_chreff chreff;
    vector_old_pv_db_chrmot chrmot;                                 // AFT620/AFT621/AFT622/AFT701/FT/M39
    vector_old_pv_db_ex_song_aft300 ex_song;
    vector_old_pv_db_eyes_rot_rate eyes_rot_rate;
    vector_old_pv_db_field_aft500 field;
    vector_old_pv_db_osage_init osage_init;
    vector_old_pv_db_performer_aft500 performer;
    vector_old_pv_db_stage_param_aft210 stage_param;

    vector_old_string edit_effect;
    vector_old_string edit_effect_low_field;
    vector_old_string effect_se_name_list;
    vector_old_string hand_item;
    vector_old_string lyric;
    vector_old_string lyric_en;                                     // FT/M39
    vector_old_string motion;
    vector_old_string motion2P;
    vector_old_string motion3P;
    vector_old_string motion4P;
    vector_old_string motion5P;
    vector_old_string motion6P;
    vector_old_string movie_list;
    vector_old_string pv_item;

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
    pv_db_song_info_ac200 song_info_en;                             // FT/M39
    pv_db_song_info_ac200 song_info;
    pv_db_title_image_ac200 title_image;

    null_int32_t bpm;
    string chainslide_failure_name;
    string chainslide_first_name;
    string chainslide_sub_name;
    string chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    string effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    string frame_texture;
    string frame_texture_a;
    string frame_texture_b;
    string frame_texture_c;
    string frame_texture_d;
    string frame_texture_e;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_old_pv;
    string movie_file_name;
    string pvbranch_success_se_name;
    null_int32_t pack;                                              // FT/M39
    null_int32_t pre_play_script;
    null_int32_t rank_board_id;                                     // FT/M39
    null_int32_t remix_parent;
    null_float_t resolution_scale;                                  // FT/M39
    null_float_t resolution_scale_neo;                              // FT/M39
    string se_name;
    string slide_name;
    string slidertouch_name;
    string song_file_name;
    string song_name;
    string song_name_en;                                            // FT/M39
    string song_name_reading;
    string song_name_reading_en;                                    // FT/M39
    null_float_t sudden_timing;
    null_bool use_osage_play_data;
} pv_db_pv_aft500;
