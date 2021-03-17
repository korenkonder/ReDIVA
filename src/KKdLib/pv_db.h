/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"

typedef enum chara {
    CHARA_MIK = 0,
    CHARA_RIN = 1,
    CHARA_LEN = 2,
    CHARA_LUK = 3,
    CHARA_NER = 4,
    CHARA_HAK = 5,
    CHARA_KAI = 6,
    CHARA_MEI = 7,
    CHARA_SAK = 8,
    CHARA_TET = 9,
} chara;

typedef enum chreff_type {
    CHARA_EFFECT_AUTH3D     = 0,
    CHARA_EFFECT_AUTH3D_OBJ = 1,
} chreff_type;

typedef enum db_format {
    DB_NONE,
    DB_AC101,
    DB_AC110,
    DB_AC120,
    DB_AC130,
    DB_AC200,
    DB_AC210,
    DB_AC211,
    DB_AC220,
    DB_AC221,
    DB_AC301,
    DB_AC310,
    DB_AC320,
    DB_AC401,
    DB_AC402,
    DB_AC410,
    DB_AC500,
    DB_AC510,
    DB_AC511,
    DB_AC520,
    DB_AC521,
    DB_AC600,
    DB_AFT101,
    DB_AFT110,
    DB_AFT120,
    DB_AFT200,
    DB_AFT300,
    DB_AFT310,
    DB_AFT400,
    DB_AFT410,
    DB_AFT500,
    DB_AFT501,
    DB_AFT510,
    DB_AFT600,
    DB_AFT701,
    DB_FT,
} db_format;

typedef enum duet_type {
    DUET_USER_SETTING   = 0,
    DUET_FIXED_SIZE     = 1,
    DUET_FIXED_SUB      = 2,
    DUET_FIXED_ALL      = 3,
    DUET_PSEUDO_DEFAULT = 4,
    DUET_PSEUDO_SAME    = 5,
    DUET_PSEUDO_SWIM    = 6,
    DUET_PSEUDO_SWIM_S  = 7,
} duet_type;

typedef enum eyes_base_adjust_type {
    EYES_BASE_ADJUST_DIRECTION = 0,
    EYES_BASE_ADJUST_CLEARANCE = 1,
    EYES_BASE_ADJUST_OFF       = 2,
} eyes_base_adjust_type;

typedef enum frame_texture_type {
    FRAME_TEXTURE_PRE_PP  = 0,
    FRAME_TEXTURE_POST_PP = 1,
    FRAME_TEXTURE_FB      = 2,
} frame_texture_type;

typedef enum movie_surface {
    MOVIE_SURFACE_BACK  = 0,
    MOVIE_SURFACE_FRONT = 1,
} movie_surface;

typedef enum performer_size {
    PERFORMER_SIZE_NORMAL     = 0,
    PERFORMER_SIZE_PLAY_CHARA = 1,
    PERFORMER_SIZE_PV_CHARA   = 2,
    PERFORMER_SIZE_SHORT      = 3,
    PERFORMER_SIZE_TALL       = 4,
} performer_size;

typedef enum performer_type {
    PERFORMER_VOCAL          = 0,
    PERFORMER_PSEUDO_DEFAULT = 1,
    PERFORMER_PSEUDO_SAME    = 2,
    PERFORMER_PSEUDO_SWIM    = 3,
    PERFORMER_PSEUDO_SWIM_S  = 4,
    PERFORMER_PSEUDO_MYCHARA = 5,
    PERFORMER_GUEST          = 6,
} performer_type;

typedef enum pv_level {
    PV_LV_00_0 = 0,
    PV_LV_00_5 = 1,
    PV_LV_01_0 = 2,
    PV_LV_01_5 = 3,
    PV_LV_02_0 = 4,
    PV_LV_02_5 = 5,
    PV_LV_03_0 = 6,
    PV_LV_03_5 = 7,
    PV_LV_04_0 = 8,
    PV_LV_04_5 = 9,
    PV_LV_05_0 = 10,
    PV_LV_05_5 = 11,
    PV_LV_06_0 = 12,
    PV_LV_06_5 = 13,
    PV_LV_07_0 = 14,
    PV_LV_07_5 = 15,
    PV_LV_08_0 = 16,
    PV_LV_08_5 = 17,
    PV_LV_09_0 = 18,
    PV_LV_09_5 = 19,
    PV_LV_10_0 = 20,
} pv_level;

typedef struct another_song_aft310 {
    char* name;
    char* song_file_name;
} another_song_aft310;

typedef struct another_song_aft500 {
    char* name;
    char* name_en;                                              // FT
    char* song_file_name;
    char* vocal_chara_num;                                      // FT
    char* vocal_disp_name;                                      // FT
    char* vocal_disp_name_en;                                   // FT
} another_song_aft500;

typedef struct auth_replace_by_module {
    null_int32_t id;
    null_int32_t module_id;
    char* name;
    char* org_name;
} auth_replace_by_module;

null(chara)

typedef struct chrcam {
    null_chara chara;

    null_int32_t id;
    char* name;
    char* org_name;
} chrcam;

null(chreff_type)

typedef struct chreff_data {
    null_chreff_type type;

    char* Name;
} chreff_data;

vector(chreff_data)

typedef struct chreff {
    vector_chreff_data data;

    null_int32_t id;
    char* name;
} chreff;

typedef struct chrmot {
    null_chara chara;

    null_int32_t id;
    char* name;
    char* org_name;
} chrmot;

typedef struct disp2d_ac200 {
    char* set_name;
    null_int32_t target_shadow_type;
    null_int32_t title_end_2d_field;
    null_int32_t title_end_2d_low_field;                        // not AC200
    null_int32_t title_end_3d_field;
    null_int32_t title_start_2d_field;
    null_int32_t title_start_2d_low_field;                      // not AC200
    null_int32_t title_start_3d_field;
} disp2d_ac200;

typedef struct disp2d_aft300 {
    char* set_name;
    null_int32_t target_shadow_type;
    null_int32_t title_2d_layer;
    null_int32_t title_end_2d_field;
    null_int32_t title_end_2d_low_field;
    null_int32_t title_end_3d_field;
    null_int32_t title_start_2d_field;
    null_int32_t title_start_2d_low_field;
    null_int32_t title_start_3d_field;
} disp2d_aft300;

null(duet_type)

typedef struct duet {
    null_duet_type type;
    null_int32_t num;
} duet;

typedef struct eyes_rot_rate {
    null_chara chr;
    null_float_t xn_rate;
    null_float_t xp_rate;
} eyes_rot_rate;

typedef struct ex_auth {
    char* name;
    char* org_name;
} ex_auth;

typedef struct ex_song_ac101 {
    null_chara chara;

    char* file;
    char* name;
} ex_song_ac101;

typedef struct ex_song_aft101 {
    null_chara chara;
    null_chara chara2P;                                         // not AFT101
    null_chara chara3P;                                         // not AFT101
    null_chara chara4P;                                         // not AFT101

    char* file;
    char* name;
} ex_song_aft101;

vector(ex_auth)

typedef struct ex_song_aft300 {
    vector_ex_auth ex_auth;                                     // AFT701/FT

    null_chara chara;
    null_chara chara2P;
    null_chara chara3P;
    null_chara chara4P;
    null_chara chara5P;                                         // not AFT300/AFT310/AFT400
    null_chara chara6P;                                         // not AFT300/AFT310/AFT400

    char* file;
    char* name;
} ex_song_aft300;

typedef struct field_ac101 {
    char* aet_back;
    char* aet_front;
    char* aet_id_back;
    char* aet_id_front;
    char* auth_3d;
    char* spr_set_back;
    char* stage;
} field_ac101;

typedef struct field_ac200 {
    vector_ptr_char auth_3d_list;                               // not AC200/AC210/AC211
    vector_int32_t auth_3d_frame_list;                          // AC520/AC521/AC600
    vector_ptr_char ex_auth_3d_list;                            // AC510/AC511/AC520/AC521/AC600

    char* aet_back;
    char* aet_back_low;                                         // not AC200
    char* aet_front;
    char* aet_front_low;                                        // not AC200
    char* aet_id_back;
    char* aet_id_back_low;                                      // not AC200
    char* aet_id_front;
    char* aet_id_front_low;                                     // not AC200
    char* auth_3d;
    char* ex_auth_3d;                                           // AC510/AC511/AC520/AC521/AC600
    char* ex_stage;                                             // AC500/AC510/AC511/AC520/AC521/AC600
    char* spr_set_back;
    char* stage;
    null_int32_t stage_flag;                                    // AC520/AC521/AC600
} field_ac200;

typedef struct field_ac500 {
    vector_ptr_char auth_3d_list;

    char* aet_back;
    char* aet_back_low;
    char* aet_front;
    char* aet_front_low;
    char* aet_id_back;
    char* aet_id_back_low;
    char* aet_id_front;
    char* aet_id_front_low;
    char* auth_3d;
    char* ex_auth_3d;                                           // not AC500
    char* ex_stage;
    char* spr_set_back;
    char* stage;
    null_int32_t stage_flag;                                    // AC520/AC521/AC600
} field_ac500;

typedef struct field_aft101 {
    vector_ptr_char auth_3d_list;
    vector_int32_t auth_3d_frame_list;
    vector_ptr_char ex_auth_3d_list;

    char* aet_back;
    char* aet_back_low;
    char* aet_front;
    char* aet_front_low;
    char* aet_id_back;
    char* aet_id_back_low;
    char* aet_id_front;
    char* aet_id_front_low;
    char* auth_3d;
    null_int32_t cam_blur;                                      // AFT110/AFT120/AFT200
    char* ex_auth_3d;
    char* ex_stage;
    char* light;                                                // AFT200
    null_int32_t light_frame;                                   // AFT200
    null_int32_t npr_type;                                      // AFT110/AFT120/AFT200
    null_int32_t sdw_off;                                       // AFT110/AFT120/AFT200
    char* spr_set_back;
    char* stage;
    null_int32_t stage_flag;
} field_aft101;

typedef struct field_aft300 {
    vector_int32_t aet_back_frame_list;
    vector_ptr_char aet_back_list;
    vector_int32_t aet_front_frame_list;
    vector_ptr_char aet_front_list;
    vector_int32_t aet_front_low_frame_list;
    vector_ptr_char aet_front_low_list;
    vector_ptr_char aet_id_back_list;
    vector_ptr_char aet_id_front_list;
    vector_ptr_char aet_id_front_low_list;
    vector_int32_t auth_3d_frame_list;
    vector_ptr_char auth_3d_list;
    vector_ptr_char ex_auth_3d_list;
    vector_ptr_char effect_rs_list;                             // AFT410
    vector_ptr_char play_eff_list;                              // AFT410
    vector_ptr_char stop_eff_list;                              // AFT410

    char* aet_back;
    int32_t aet_back_frame;
    char* aet_back_low;
    char* aet_front;
    int32_t aet_front_frame;
    char* aet_front_low;
    int32_t aet_front_low_frame;
    char* aet_id_back;
    char* aet_id_back_low;
    char* aet_id_front;
    char* aet_id_front_low;
    char* auth_3d;
    null_int32_t cam_blur;
    char* ex_auth_3d;
    char* ex_stage;
    char* light;
    null_int32_t light_frame;
    null_int32_t npr_type;
    null_int32_t sdw_off;
    char* spr_set_back;
    char* stage;
    null_int32_t stage_flag;
} field_aft300;

typedef struct field_aft500 {
    vector_int32_t aet_back_frame_list;
    vector_ptr_char aet_back_list;
    vector_int32_t aet_front_frame_list;
    vector_ptr_char aet_front_list;
    vector_int32_t aet_front_3d_surf_frame_list;
    vector_ptr_char aet_front_3d_surf_list;
    vector_int32_t aet_front_low_frame_list;
    vector_ptr_char aet_front_low_list;
    vector_ptr_char aet_id_back_list;
    vector_ptr_char aet_id_front_list;
    vector_ptr_char aet_id_front_3d_surf_list;
    vector_ptr_char aet_id_front_low_list;
    vector_int32_t auth_3d_frame_list;
    vector_ptr_char auth_3d_list;
    vector_ptr_char ex_auth_3d_list;
    vector_ptr_char effect_emision_rs_list;
    vector_ptr_char effect_rs_list;
    vector_ptr_char play_eff_list;
    vector_ptr_char stop_eff_list;

    char* aet_back;
    int32_t aet_back_frame;
    char* aet_back_low;
    char* aet_front;
    char* aet_front_3d_surf;
    int32_t aet_front_3d_surf_frame;
    int32_t aet_front_frame;
    char* aet_front_low;
    int32_t aet_front_low_frame;
    char* aet_id_back;
    char* aet_id_back_low;
    char* aet_id_front;
    char* aet_id_front_3d_surf;
    char* aet_id_front_low;
    char* auth_3d;
    null_int32_t cam_blur;
    char* ex_auth_3d;
    char* ex_stage;
    char* light;
    null_int32_t light_frame;
    null_int32_t npr_type;
    null_int32_t sdw_off;
    char* spr_set_back;
    char* stage;
    null_int32_t stage_flag;
} field_aft500;

typedef struct mdata_ac110 {
    char* directory;
    null_int32_t flag;
    char* gam_aet;
    char* gam_aet_id;
    char* gam_spr;
    char* sel_spr;
    char* sel_spr_cmn;
    char* target_shadow_type;
} mdata_ac110;

typedef struct mdata_ac200 {
    char* directory;
    null_int32_t flag;
} mdata_ac200;

typedef struct osage_init {
    null_int32_t frame;
    char* motion;
    char* stage;
} osage_init;

null(performer_size)
null(performer_type)

typedef struct performer_aft101 {
    null_chara chara;
    null_performer_size size;
    null_performer_type type;

    null_int32_t costume;
    null_int32_t exclude;
    null_int32_t fixed;
    null_int32_t item_back;
    null_int32_t item_face;
    null_int32_t item_neck;
    null_int32_t item_zujo;
    null_int32_t pv_costume;
} performer_aft101;

typedef struct performer_aft500 {
    null_chara chara;
    null_performer_size size;
    null_performer_type type;

    null_int32_t costume;
    null_int32_t exclude;
    null_int32_t fixed;
    null_int32_t item_back;
    null_int32_t item_face;
    null_int32_t item_kami;                                     // FT
    null_int32_t item_neck;
    null_int32_t item_zujo;
    null_int32_t pv_costume;
    null_int32_t pseudo_same_id;
} performer_aft500;

typedef struct pv_expression {
    char* file_name;
} pv_expression;

typedef struct sabi {
    null_float_t play_time;
    null_float_t start_time;
} sabi;

typedef struct sel_2d {
    char* bg;
    char* jk;
    char* logo;
    char* plate;
} sel_2d;

typedef struct song_info_ac101 {
    char* arranger;
    char* guitar_player;                                        // AC130
    char* lyrics;
    char* manipulator;
    char* music;
    char* pv_editor;
} song_info_ac101;

typedef struct song_info_ac200 {
    char* arranger;
    char* guitar_player;
    char* lyrics;
    char* manipulator;
    char* music;
    char* pv_editor;
    char* ex_info0key;
    char* ex_info0val;
    char* ex_info1key;
    char* ex_info1val;
    char* ex_info2key;
    char* ex_info2val;
    char* ex_info3key;
    char* ex_info3val;
} song_info_ac200;

typedef struct stage_param_ac200 {
    char* collision_file;
    char* stage;
    char* wind_file;
} stage_param_ac200;

typedef struct stage_param_aft300 {
    char* collision_file;
    null_int32_t mhd_id;
    char* stage;
    char* wind_file;
} stage_param_aft300;

typedef struct title_image_ac101 {
    vector_ptr_char file;                                       // AC101
    vector_ptr_char file_name;                                  // AC101

    char* aet_name;
    null_float_t time;
} title_image_ac101;

typedef struct title_image_ac200 {
    char* aet_name;
    null_float_t end_time;
    null_float_t time;
} title_image_ac200;

vector(field_ac101)

typedef struct diff_ac101 {
    vector_field_ac101 field;

    vector_ptr_char motion;

    sel_2d sel_2d;                                              // AC101
    song_info_ac101 song_info;
    title_image_ac101 title_image;

    null_int32_t level;
    char* movie_file_name;
    char* script_file_name;
    char* se_name;
    null_int32_t version;
} diff_ac101;

vector(field_ac200)

typedef struct diff_ac200 {
    vector_field_ac200 field;

    vector_ptr_char edit_effect;
    vector_ptr_char hand_item;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char pv_item;                                    // not AC200/AC210/AC211

    song_info_ac200 song_info;
    title_image_ac200 title_image;

    null_float_t edit_chara_scale;
    null_int32_t level;
    char* movie_file_name;
    char* script_file_name;
    char* se_name;
    null_int32_t version;
} diff_ac200;

null(movie_surface)

vector(field_aft101)

typedef struct diff_aft101 {
    vector_field_aft101 field;

    vector_ptr_char edit_effect;
    vector_ptr_char hand_item;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char motion3P;
    vector_ptr_char motion4P;
    vector_ptr_char pv_item;

    pv_level level;
    null_movie_surface movie_surface;
    song_info_ac200 song_info;
    title_image_ac200 title_image;

    char* chainslide_failure_name;
    char* chainslide_first_name;
    char* chainslide_sub_name;
    char* chainslide_success_name;
    null_float_t edit_chara_scale;
    char* effect_se_file_name;
    null_int32_t level_sort_index;
    char* movie_file_name;
    char* pvbranch_success_se_name;
    char* script_file_name;
    char* script_format;
    char* se_name;
    char* slide_name;
    char* slidertouch_name;
    null_int32_t version;
} diff_aft101;

typedef struct diff_attr {
    null_int32_t extra;
    null_int32_t original;
    null_int32_t slide;
} diff_attr;

vector(field_aft300)
vector(performer_aft101)

typedef struct diff_aft300 {
    vector_field_aft300 field;
    vector_performer_aft101 performer;

    vector_ptr_char edit_effect;
    vector_ptr_char edit_effect_low_field;
    vector_ptr_char effect_se_name_list;
    vector_ptr_char hand_item;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char motion3P;
    vector_ptr_char motion4P;
    vector_ptr_char motion5P;                                   // AFT410
    vector_ptr_char motion6P;                                   // AFT410
    vector_ptr_char movie_list;
    vector_ptr_char pv_item;

    diff_attr attribute;
    pv_level level;
    null_movie_surface movie_surface;
    song_info_ac200 song_info;
    title_image_ac200 title_image;

    char* chainslide_failure_name;
    char* chainslide_first_name;
    char* chainslide_sub_name;
    char* chainslide_success_name;
    null_int32_t edition;
    null_float_t edit_chara_scale;
    char* effect_se_file_name;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t level_sort_index;
    char* movie_file_name;
    char* pvbranch_success_se_name;
    char* script_file_name;
    char* script_format;
    char* se_name;
    char* slide_name;
    char* slidertouch_name;
    null_float_t sudden_timing;
    null_int32_t version;
} diff_aft300;

vector(diff_aft300)

typedef struct difficulty_aft300 {
    vector_diff_aft300 diff[5];
    diff_attr attribute;
} difficulty_aft300;

vector(field_aft500)
vector(performer_aft500)

typedef struct diff_aft500 {
    vector_field_aft500 field;
    vector_performer_aft500 performer;

    vector_ptr_char edit_effect;
    vector_ptr_char edit_effect_low_field;
    vector_ptr_char effect_se_name_list;
    vector_ptr_char hand_item;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char motion3P;
    vector_ptr_char motion4P;
    vector_ptr_char motion5P;
    vector_ptr_char motion6P;
    vector_ptr_char movie_list;
    vector_ptr_char pv_item;

    diff_attr attribute;
    pv_level level;
    null_movie_surface movie_surface;
    song_info_ac200 song_info_en;                               // FT
    song_info_ac200 song_info;
    title_image_ac200 title_image;

    char* chainslide_failure_name;
    char* chainslide_first_name;
    char* chainslide_sub_name;
    char* chainslide_success_name;
    null_int32_t edition;
    null_float_t edit_chara_scale;
    char* effect_se_file_name;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_ps4_dlc;                                    // FT
    null_int32_t level_sort_index;
    char* movie_file_name;
    char* pvbranch_success_se_name;
    char* script_file_name;
    char* script_format;
    char* se_name;
    char* slide_name;
    char* slidertouch_name;
    null_float_t sudden_timing;
    null_int32_t version;
} diff_aft500;

vector(diff_aft500)

typedef struct difficulty_aft500 {
    vector_diff_aft500 diff[5];
    diff_attr attribute;
} difficulty_aft500;

vector(ex_song_ac101)
vector(osage_init)

typedef struct pv_ac101 {
    vector_ex_song_ac101 ex_song;
    vector_field_ac200 field;
    vector_osage_init osage_init;                               // AC120/AC130

    vector_ptr_char lyric;
    vector_ptr_char motion;

    diff_ac101 difficulty[5];
    mdata_ac110 mdata;                                          // AC110/AC120/AC130
    sabi sabi;
    sel_2d sel_2d;                                              // AC101
    song_info_ac200 song_info;
    title_image_ac200 title_image;
    null_chara unlock;

    null_int32_t bpm;
    null_int32_t date;
    null_int32_t demo_flag;                                     // AC110/AC120/AC130
    null_int32_t m_data_flag;                                   // AC101
    char* movie_file_name;
    char* se_name;
    char* song_file_name;
    char* song_name;
    char* song_name_reading;
} pv_ac101;

vector(stage_param_ac200)

typedef struct pv_ac200 {
    vector_ex_song_ac101 ex_song;
    vector_field_ac200 field;
    vector_osage_init osage_init;
    vector_stage_param_ac200 stage_param;

    vector_ptr_char edit_effect;
    vector_ptr_char hand_item;
    vector_ptr_char lyric;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char pv_item;                                    // not AC200/AC210/AC211

    diff_ac200 difficulty[5];
    disp2d_ac200 disp2d;
    duet duet;
    mdata_ac200 mdata;
    sabi sabi;
    song_info_ac200 song_info;
    title_image_ac200 title_image;
    null_chara unlock1P;
    null_chara unlock2P;

    null_int32_t bpm;
    null_int32_t date;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    char* movie_file_name;
    null_int32_t remix_parent;
    char* se_name;
    char* song_file_name;
    char* song_name;
    char* song_name_reading;
    null_bool use_osage_play_data;                              // not AC200
} pv_ac200;

null(eyes_base_adjust_type)

vector(chreff)
vector(ex_song_aft101)
vector(eyes_rot_rate)

typedef struct pv_aft101 {
    vector_chreff chreff;
    vector_ex_song_aft101 ex_song;
    vector_eyes_rot_rate eyes_rot_rate;                         // AFT200
    vector_field_aft101 field;
    vector_osage_init osage_init;
    vector_performer_aft101 performer;
    vector_stage_param_ac200 stage_param;

    vector_ptr_char edit_effect;
    vector_ptr_char effect_se_name_list;
    vector_ptr_char hand_item;
    vector_ptr_char lyric;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char motion3P;
    vector_ptr_char motion4P;
    vector_ptr_char pv_item;

    diff_aft101 difficulty[5];
    disp2d_ac200 disp2d;
    null_eyes_base_adjust_type eyes_base_adjust_type;           // AFT200
    mdata_ac200 mdata;
    null_movie_surface movie_surface;
    pv_expression pv_expression;
    sabi sabi;
    song_info_ac200 song_info;
    title_image_ac200 title_image;

    null_int32_t bpm;
    char* chainslide_failure_name;
    char* chainslide_first_name;
    char* chainslide_sub_name;
    char* chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    char* effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_int32_t is_old_pv;                                     // AFT200
    char* movie_file_name;
    char* pvbranch_success_se_name;
    null_int32_t remix_parent;
    char* se_name;
    char* slide_name;
    char* slidertouch_name;
    char* song_file_name;
    char* song_name;
    char* song_name_reading;
    null_bool use_osage_play_data;
} pv_aft101;

vector(another_song_aft310)
vector(chrcam)
vector(ex_song_aft300)
vector(stage_param_aft300)

typedef struct pv_aft300 {
    vector_another_song_aft310 another_song;                    // not AFT300
    vector_chrcam chrcam;                                       // not AFT300
    vector_chreff chreff;
    vector_ex_song_aft300 ex_song;
    vector_eyes_rot_rate eyes_rot_rate;
    vector_field_aft300 field;
    vector_osage_init osage_init;
    vector_performer_aft101 performer;
    vector_stage_param_aft300 stage_param;

    vector_ptr_char edit_effect;
    vector_ptr_char edit_effect_low_field;                      // not AFT300
    vector_ptr_char effect_se_name_list;
    vector_ptr_char hand_item;
    vector_ptr_char lyric;
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char motion3P;
    vector_ptr_char motion4P;
    vector_ptr_char motion5P;                                   // AFT410
    vector_ptr_char motion6P;                                   // AFT410
    vector_ptr_char pv_item;

    difficulty_aft300 difficulty;
    disp2d_aft300 disp2d;
    null_eyes_base_adjust_type eyes_base_adjust_type;
    mdata_ac200 mdata;
    null_movie_surface movie_surface;
    pv_expression pv_expression;
    sabi sabi;
    song_info_ac200 song_info;
    title_image_ac200 title_image;

    null_int32_t bpm;
    char* chainslide_failure_name;
    char* chainslide_first_name;
    char* chainslide_sub_name;
    char* chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    char* effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_old_pv;
    char* movie_file_name;
    char* pvbranch_success_se_name;
    null_int32_t pre_play_script;                               // AFT410
    null_int32_t remix_parent;
    char* se_name;
    char* slide_name;
    char* slidertouch_name;
    char* song_file_name;
    char* song_name;
    char* song_name_reading;
    null_float_t sudden_timing;
    null_bool use_osage_play_data;
} pv_aft300;

null(frame_texture_type)

vector(another_song_aft500)
vector(auth_replace_by_module)
vector(chrmot)

typedef struct pv_aft500 {
    vector_another_song_aft500 another_song;
    vector_auth_replace_by_module auth_replace_by_module;       // AFT600/AFT701/FT
    vector_chrcam chrcam;
    vector_chreff chreff;
    vector_chrmot chrmot;                                       // AFT701/FT
    vector_ex_song_aft300 ex_song;
    vector_eyes_rot_rate eyes_rot_rate;
    vector_field_aft500 field;
    vector_osage_init osage_init;
    vector_performer_aft500 performer;
    vector_stage_param_aft300 stage_param;

    vector_ptr_char edit_effect;
    vector_ptr_char edit_effect_low_field;
    vector_ptr_char effect_se_name_list;
    vector_ptr_char hand_item;
    vector_ptr_char lyric;
    vector_ptr_char lyric_en;                                   // FT
    vector_ptr_char motion;
    vector_ptr_char motion2P;
    vector_ptr_char motion3P;
    vector_ptr_char motion4P;
    vector_ptr_char motion5P;
    vector_ptr_char motion6P;
    vector_ptr_char movie_list;
    vector_ptr_char pv_item;

    difficulty_aft300 difficulty;
    disp2d_aft300 disp2d;
    null_eyes_base_adjust_type eyes_base_adjust_type;
    null_frame_texture_type frame_texture_a_type;
    null_frame_texture_type frame_texture_b_type;
    null_frame_texture_type frame_texture_c_type;
    null_frame_texture_type frame_texture_d_type;
    null_frame_texture_type frame_texture_e_type;
    null_frame_texture_type frame_texture_type;
    mdata_ac200 mdata;
    null_movie_surface movie_surface;
    pv_expression pv_expression;
    sabi sabi;
    song_info_ac200 song_info_en;                               // FT
    song_info_ac200 song_info;
    title_image_ac200 title_image;

    null_int32_t bpm;
    char* chainslide_failure_name;
    char* chainslide_first_name;
    char* chainslide_sub_name;
    char* chainslide_success_name;
    null_int32_t date;
    null_int32_t disable_calc_motfrm_limit;
    char* effect_se_file_name;
    null_int32_t edit;
    null_float_t edit_chara_scale;
    null_int32_t eyes_xrot_adjust;
    char* frame_texture;
    char* frame_texture_a;
    char* frame_texture_b;
    char* frame_texture_c;
    char* frame_texture_d;
    char* frame_texture_e;
    null_float_t hidden_timing;
    null_float_t high_speed_rate;
    null_int32_t is_old_pv;
    char* movie_file_name;
    char* pvbranch_success_se_name;
    null_int32_t pack;                                          // FT
    null_int32_t pre_play_script;
    null_int32_t rank_board_id;                                 // FT
    null_int32_t remix_parent;
    null_float_t resolution_scale;                                // FT
    null_float_t resolution_scale_neo;                            // FT
    char* se_name;
    char* slide_name;
    char* slidertouch_name;
    char* song_file_name;
    char* song_name;
    char* song_name_en;                                         // FT
    char* song_name_reading;
    char* song_name_reading_en;                                 // FT
    null_float_t sudden_timing;
    null_bool use_osage_play_data;
} pv_aft500;
