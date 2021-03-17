/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef enum dsc_diff {
    DSC_DIFF_EASY = 0,
    DSC_DIFF_NORMAL = 1,
    DSC_DIFF_HARD = 2,
    DSC_DIFF_EXTREME = 3,
    DSC_DIFF_ENCORE = 4,
} dsc_diff;

typedef struct dsc_type {
    dsc_diff difficulty : 3;
    uint32_t extra : 1;
} dsc_type;

typedef enum dsc_f2_mode {
    DSC_MODE_SYSTEM = 0,
    DSC_MODE_MOUTH = 1,
    DSC_MODE_SCENE = 2,
    DSC_MODE_DAYO = 3,
} dsc_f2_mode;

typedef struct dsc_f2_type {
    dsc_diff difficulty : 3;
    dsc_f2_mode type : 2;
    uint32_t success : 1;
} dsc_f2_type;

#pragma region dsc_note
typedef struct dsc_note7 {
    int32_t type;
    int32_t x_pos;
    int32_t y_pos;
    int32_t angle;
    int32_t distance;
    int32_t amplitude;
    int32_t wave_count;
} dsc_note7;

typedef struct dsc_note11 {
    int32_t type;
    int32_t hold_timer;
    int32_t hold_end;
    int32_t x_pos;
    int32_t y_pos;
    int32_t angle;
    int32_t wave_count;
    int32_t distance;
    int32_t amplitude;
    int32_t timing;
    int32_t bar_time;
} dsc_note11;

typedef struct dsc_note12 {
    int32_t type;
    int32_t hold_timer;
    int32_t hold_end;
    int32_t x_pos;
    int32_t y_pos;
    int32_t angle;
    int32_t wave_count;
    int32_t distance;
    int32_t amplitude;
    int32_t timing;
    int32_t bar_time;
    int32_t target_effect;
} dsc_note12;
#pragma endregion

#pragma region dsc_func
typedef struct dsc_func {
    int32_t id;
    int32_t length;
    wchar_t* name;
} dsc_func;

typedef struct dsc_func_ac {
    int32_t id;
    int32_t length_old;
    int32_t length;
    wchar_t* name;
} dsc_func_ac;
#pragma endregion

#pragma region dsc_target
typedef enum dsc_target_ac {
    DSC_TARGET_AC_TRIANGLE                  = 0x00,
    DSC_TARGET_AC_CIRCLE                    = 0x01,
    DSC_TARGET_AC_CROSS                     = 0x02,
    DSC_TARGET_AC_SQUARE                    = 0x03,
    DSC_TARGET_AC_TRIANGLE_HOLD             = 0x04,
    DSC_TARGET_AC_CIRCLE_HOLD               = 0x05,
    DSC_TARGET_AC_CROSS_HOLD                = 0x06,
    DSC_TARGET_AC_SQUARE_HOLD               = 0x07,
    DSC_TARGET_AC_RANDOM                    = 0x08,
    DSC_TARGET_AC_RANDOM_HOLD               = 0x09,
    DSC_TARGET_AC_PREVIOUS                  = 0x0A,
} dsc_target_ac;

typedef enum dsc_target_ft {
    DSC_TARGET_FT_TRIANGLE                  = 0x00,
    DSC_TARGET_FT_CIRCLE                    = 0x01,
    DSC_TARGET_FT_CROSS                     = 0x02,
    DSC_TARGET_FT_SQUARE                    = 0x03,
    DSC_TARGET_FT_TRIANGLE_HOLD             = 0x04,
    DSC_TARGET_FT_CIRCLE_HOLD               = 0x05,
    DSC_TARGET_FT_CROSS_HOLD                = 0x06,
    DSC_TARGET_FT_SQUARE_HOLD               = 0x07,
    DSC_TARGET_FT_RANDOM                    = 0x08,
    DSC_TARGET_FT_RANDOM_HOLD               = 0x09,
    DSC_TARGET_FT_PREVIOUS                  = 0x0A,
    DSC_TARGET_FT_SLIDE_L                   = 0x0C,
    DSC_TARGET_FT_SLIDE_R                   = 0x0D,
    DSC_TARGET_FT_SLIDE_CHAIN_L             = 0x0F,
    DSC_TARGET_FT_SLIDE_CHAIN_R             = 0x10,
    DSC_TARGET_FT_GREEN_SQUARE              = 0x11,
    DSC_TARGET_FT_TRIANGLE_CHANCE           = 0x12,
    DSC_TARGET_FT_CIRCLE_CHANCE             = 0x13,
    DSC_TARGET_FT_CROSS_CHANCE              = 0x14,
    DSC_TARGET_FT_SQUARE_CHANCE             = 0x15,
    DSC_TARGET_FT_SLIDE_L_CHANCE            = 0x17,
    DSC_TARGET_FT_SLIDE_R_CHANCE            = 0x18,
} dsc_target_ft;

typedef enum dsc_target_dt {
    DSC_TARGET_DT_TRIANGLE                  = 0x00,
    DSC_TARGET_DT_CIRCLE                    = 0x01,
    DSC_TARGET_DT_CROSS                     = 0x02,
    DSC_TARGET_DT_SQUARE                    = 0x03,
    DSC_TARGET_DT_TRIANGLE_DOUBLE           = 0x04,
    DSC_TARGET_DT_CIRCLE_DOUBLE             = 0x05,
    DSC_TARGET_DT_CROSS_DOUBLE              = 0x06,
    DSC_TARGET_DT_SQUARE_DOUBLE             = 0x07,
    DSC_TARGET_DT_TRIANGLE_HOLD             = 0x08,
    DSC_TARGET_DT_CIRCLE_HOLD               = 0x09,
    DSC_TARGET_DT_CROSS_HOLD                = 0x0A,
    DSC_TARGET_DT_SQUARE_HOLD               = 0x0B,
} dsc_target_dt;

typedef enum dsc_target_f {
    DSC_TARGET_F_TRIANGLE                   = 0x00,
    DSC_TARGET_F_CIRCLE                     = 0x01,
    DSC_TARGET_F_CROSS                      = 0x02,
    DSC_TARGET_F_SQUARE                     = 0x03,
    DSC_TARGET_F_TRIANGLE_DOUBLE            = 0x04,
    DSC_TARGET_F_CIRCLE_DOUBLE              = 0x05,
    DSC_TARGET_F_CROSS_DOUBLE               = 0x06,
    DSC_TARGET_F_SQUARE_DOUBLE              = 0x07,
    DSC_TARGET_F_TRIANGLE_HOLD              = 0x08,
    DSC_TARGET_F_CIRCLE_HOLD                = 0x09,
    DSC_TARGET_F_CROSS_HOLD                 = 0x0A,
    DSC_TARGET_F_SQUARE_HOLD                = 0x0B,
    DSC_TARGET_F_STAR                       = 0x0C,
    DSC_TARGET_F_STAR_HOLD                  = 0x0D,
    DSC_TARGET_F_STAR_DOUBLE                = 0x0E,
    DSC_TARGET_F_CHANCE_STAR                = 0x0F,
    DSC_TARGET_F_EDIT_CHANCE_STAR           = 0x10,
    DSC_TARGET_F_STAR_UP                    = 0x12,
    DSC_TARGET_F_STAR_RIGHT                 = 0x13,
    DSC_TARGET_F_STAR_DOWN                  = 0x14,
    DSC_TARGET_F_STAR_LEFT                  = 0x15,
} dsc_target_f;

typedef enum dsc_target_f2 {
    DSC_TARGET_F2_TRIANGLE                  = 0x00,
    DSC_TARGET_F2_CIRCLE                    = 0x01,
    DSC_TARGET_F2_CROSS                     = 0x02,
    DSC_TARGET_F2_SQUARE                    = 0x03,
    DSC_TARGET_F2_TRIANGLE_DOUBLE           = 0x04,
    DSC_TARGET_F2_CIRCLE_DOUBLE             = 0x05,
    DSC_TARGET_F2_CROSS_DOUBLE              = 0x06,
    DSC_TARGET_F2_SQUARE_DOUBLE             = 0x07,
    DSC_TARGET_F2_TRIANGLE_HOLD             = 0x08,
    DSC_TARGET_F2_CIRCLE_HOLD               = 0x09,
    DSC_TARGET_F2_CROSS_HOLD                = 0x0A,
    DSC_TARGET_F2_SQUARE_HOLD               = 0x0B,
    DSC_TARGET_F2_STAR                      = 0x0C,
    DSC_TARGET_F2_STAR_HOLD                 = 0x0D,
    DSC_TARGET_F2_STAR_DOUBLE               = 0x0E,
    DSC_TARGET_F2_CHANCE_STAR               = 0x0F,
    DSC_TARGET_F2_EDIT_CHANCE_STAR          = 0x10,
    DSC_TARGET_F2_LINK_STAR                 = 0x16,
    DSC_TARGET_F2_LINK_STAR_END             = 0x17,
} dsc_target_f2;

typedef enum dsc_target_mgf {
    DSC_TARGET_MGF_TRIANGLE                 = 0x00,
    DSC_TARGET_MGF_CIRCLE                   = 0x01,
    DSC_TARGET_MGF_CROSS                    = 0x02,
    DSC_TARGET_MGF_SQUARE                   = 0x03,
    DSC_TARGET_MGF_TRIANGLE_DOUBLE          = 0x04,
    DSC_TARGET_MGF_CIRCLE_DOUBLE            = 0x05,
    DSC_TARGET_MGF_CROSS_DOUBLE             = 0x06,
    DSC_TARGET_MGF_SQUARE_DOUBLE            = 0x07,
    DSC_TARGET_MGF_TRIANGLE_HOLD            = 0x08,
    DSC_TARGET_MGF_CIRCLE_HOLD              = 0x09,
    DSC_TARGET_MGF_CROSS_HOLD               = 0x0A,
    DSC_TARGET_MGF_SQUARE_HOLD              = 0x0B,
    DSC_TARGET_MGF_TRIANGLE_FEVER           = 0x21,
    DSC_TARGET_MGF_CIRCLE_FEVER             = 0x22,
    DSC_TARGET_MGF_CROSS_FEVER              = 0x23,
    DSC_TARGET_MGF_SQUARE_FEVER             = 0x24,
    DSC_TARGET_MGF_TRIANGLE_FEVER_DOUBLE    = 0x25,
    DSC_TARGET_MGF_CIRCLE_FEVER_DOUBLE      = 0x26,
    DSC_TARGET_MGF_CROSS_FEVER_DOUBLE       = 0x27,
    DSC_TARGET_MGF_SQUARE_FEVER_DOUBLE      = 0x28,
    DSC_TARGET_MGF_TRIANGLE_FEVER_HOLD      = 0x29,
    DSC_TARGET_MGF_CIRCLE_FEVER_HOLD        = 0x2A,
    DSC_TARGET_MGF_CROSS_FEVER_HOLD         = 0x2B,
    DSC_TARGET_MGF_SQUARE_FEVER_HOLD        = 0x2C,
} dsc_target_mgf;

typedef enum dsc_target_id_x {
    DSC_TARGET_X_TRIANGLE                   = 0x00,
    DSC_TARGET_X_CIRCLE                     = 0x01,
    DSC_TARGET_X_CROSS                      = 0x02,
    DSC_TARGET_X_SQUARE                     = 0x03,
    DSC_TARGET_X_TRIANGLE_DOUBLE            = 0x04,
    DSC_TARGET_X_CIRCLE_DOUBLE              = 0x05,
    DSC_TARGET_X_CROSS_DOUBLE               = 0x06,
    DSC_TARGET_X_SQUARE_DOUBLE              = 0x07,
    DSC_TARGET_X_TRIANGLE_HOLD              = 0x08,
    DSC_TARGET_X_CIRCLE_HOLD                = 0x09,
    DSC_TARGET_X_CROSS_HOLD                 = 0x0A,
    DSC_TARGET_X_SQUARE_HOLD                = 0x0B,
    DSC_TARGET_X_STAR                       = 0x0C,
    DSC_TARGET_X_STAR_HOLD                  = 0x0D,
    DSC_TARGET_X_STAR_DOUBLE                = 0x0E,
    DSC_TARGET_X_CHANCE_STAR                = 0x0F,
    DSC_TARGET_X_EDIT_CHANCE_STAR           = 0x10,
    DSC_TARGET_X_STAR_RUSH                  = 0x11,
    DSC_TARGET_X_LINK_STAR                  = 0x16,
    DSC_TARGET_X_LINK_STAR_END              = 0x17,
    DSC_TARGET_X_CIRCLE_RUSH_TEST           = 0x18,
    DSC_TARGET_X_TRIANGLE_RUSH              = 0x19,
    DSC_TARGET_X_CIRCLE_RUSH                = 0x1A,
    DSC_TARGET_X_CROSS_RUSH                 = 0x1B,
    DSC_TARGET_X_SQUARE_RUSH                = 0x1C,
} dsc_target_id_x;
#pragma endregion

static const dsc_func dsc_ac101_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 3, L"MIKU_MOVE" },
    { 3, 1, L"MIKU_ROT" },
    { 4, 1, L"MIKU_DISP" },
    { 5, 1, L"MIKU_SHADOW" },
    { 6, 7, L"TARGET" },
    { 7, 3, L"SET_MOTION" },
    { 8, 1, L"SET_PLAYDATA" },
    { 9, 5, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 1, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 2, L"EYE_ANIM" },
    { 19, 3, L"MOUTH_ANIM" },
    { 20, 4, L"HAND_ANIM" },
    { 21, 3, L"LOOK_ANIM" },
    { 22, 3, L"EXPRESSION" },
    { 23, 4, L"LOOK_CAMERA" },
    { 24, 1, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 1, L"MODE_SELECT" },
    { 27, 2, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 1, L"SHADOWHEIGHT" },
    { 30, 2, L"EDIT_FACE" },
    { 31, 19, L"MOVE_CAMERA" },
    { 32, 0, L"PV_END" },
    { 33, 2, L"SHADOWPOS" },
    { 34, 2, L"NEAR_CLIP" },
    { 35, 1, L"CLOTH_WET" },
    { 36, 6, L"SCENE_FADE" },               // AC110
    { 37, 6, L"TONE_TRANS" },               // AC120
    { 38, 1, L"SATURATE" },
    { 39, 1, L"FADE_MODE" },
};

static const dsc_func_ac dsc_ac_func[] = {
    { 0, 0, 0, L"END" },
    { 1, 1, 1, L"TIME" },
    { 2, 3, 4, L"MIKU_MOVE" },
    { 3, 1, 2, L"MIKU_ROT" },
    { 4, 1, 2, L"MIKU_DISP" },
    { 5, 1, 2, L"MIKU_SHADOW" },
    { 6, 7, 7, L"TARGET" },
    { 7, 3, 4, L"SET_MOTION" },
    { 8, 1, 2, L"SET_PLAYDATA" },
    { 9, 5, 6, L"EFFECT" },
    { 10, 2, 2, L"FADEIN_FIELD" },
    { 11, 1, 1, L"EFFECT_OFF" },
    { 12, 6, 6, L"SET_CAMERA" },
    { 13, 2, 2, L"DATA_CAMERA" },
    { 14, 1, 1, L"CHANGE_FIELD" },
    { 15, 1, 1, L"HIDE_FIELD" },
    { 16, 3, 3, L"MOVE_FIELD" },
    { 17, 2, 2, L"FADEOUT_FIELD" },
    { 18, 2, 3, L"EYE_ANIM" },
    { 19, 3, 5, L"MOUTH_ANIM" },
    { 20, 4, 5, L"HAND_ANIM" },
    { 21, 3, 4, L"LOOK_ANIM" },
    { 22, 3, 4, L"EXPRESSION" },
    { 23, 4, 5, L"LOOK_CAMERA" },
    { 24, 1, 2, L"LYRIC" },
    { 25, 0, 0, L"MUSIC_PLAY" },
    { 26, 1, 2, L"MODE_SELECT" },
    { 27, 2, 4, L"EDIT_MOTION" },
    { 28, 2, 2, L"BAR_TIME_SET" },
    { 29, 1, 2, L"SHADOWHEIGHT" },
    { 30, 2, 1, L"EDIT_FACE" },
    { 31, 19, 21, L"MOVE_CAMERA" },
    { 32, 0, 0, L"PV_END" },
    { 33, 2, 3, L"SHADOWPOS" },
    { 34, 2, 2, L"EDIT_LYRIC" },
    { 35, 5, 5, L"EDIT_TARGET" },
    { 36, 1, 1, L"EDIT_MOUTH" },
    { 37, 1, 1, L"SET_CHARA" },
    { 38, 7, 7, L"EDIT_MOVE" },
    { 39, 1, 1, L"EDIT_SHADOW" },
    { 40, 1, 1, L"EDIT_EYELID" },
    { 41, 2, 2, L"EDIT_EYE" },
    { 42, 1, 1, L"EDIT_ITEM" },
    { 43, 2, 2, L"EDIT_EFFECT" },
    { 44, 1, 1, L"EDIT_DISP" },
    { 45, 2, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, 3, L"AIM" },
    { 47, 3, 3, L"HAND_ITEM" },
    { 48, 1, 1, L"EDIT_BLUSH" },
    { 49, 2, 2, L"NEAR_CLIP" },
    { 50, 1, 2, L"CLOTH_WET" },
    { 51, 3, 3, L"LIGHT_ROT" },
    { 52, 6, 6, L"SCENE_FADE" },
    { 53, 6, 6, L"TONE_TRANS" },
    { 54, 1, 1, L"SATURATE" },
    { 55, 1, 1, L"FADE_MODE" },
    { 56, 2, 2, L"AUTO_BLINK" },
    { 57, 3, 3, L"PARTS_DISP" },
    { 58, 1, 1, L"TARGET_FLYING_TIME" },    // AC210
    { 59, 2, 2, L"CHARA_SIZE" },
    { 60, 2, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, L"ITEM_ANIM" },
    { 62, 4, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, 1, L"SCENE_ROT" },
    { 64, 2, 2, L"EDIT_MOT_SMOOTH_LEN" },   // AC500
    { 65, 1, 1, L"PV_BRANCH_MODE" },        // AC510
    { 66, 2, 2, L"DATA_CAMERA_START" },
    { 67, 1, 1, L"MOVIE_PLAY" },
    { 68, 1, 1, L"MOVIE_DISP" },
    { 69, 3, 3, L"WIND" },
    { 70, 3, 3, L"OSAGE_STEP" },
    { 71, 3, 3, L"OSAGE_MV_CCL" },
    { 72, 2, 2, L"CHARA_COLOR" },
    { 73, 1, 1, L"SE_EFFECT" },
    { 74, 9, 9, L"EDIT_MOVE_XYZ" },
    { 75, 3, 3, L"EDIT_EYELID_ANIM" },
    { 76, 2, 2, L"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, L"EDIT_MOTION_LOOP" },
    { 78, 2, 2, L"EDIT_EXPRESSION" },
    { 79, 3, 3, L"EDIT_EYE_ANIM" },
    { 80, 2, 2, L"EDIT_MOUTH_ANIM" },
    { 81, 24, 24, L"EDIT_CAMERA" },
    { 82, 1, 1, L"EDIT_MODE_SELECT" },
    { 83, 2, 2, L"PV_END_FADEOUT" },
    { 84, 1, 1, L"TARGET_FLAG" },
    { 85, 3, 3, L"ITEM_ANIM_ATTACH" },
    { 86, 1, 1, L"SHADOW_RANGE" },
    { 87, 3, 3, L"HAND_SCALE" },            // AFT101
    { 88, 4, 4, L"LIGHT_POS" },
    { 89, 1, 1, L"FACE_TYPE" },             // AFT200
    { 90, 2, 2, L"SHADOW_CAST" },           // AFT300
    { 91, 6, 6, L"EDIT_MOTION_F" },         // AFT310
    { 92, 3, 3, L"FOG" },                   // AFT410
    { 93, 2, 2, L"f2_header" },
    { 94, 3, 3, L"COLOR_COLLE" },
    { 95, 3, 3, L"DOF" },
    { 96, 4, 4, L"CHARA_ALPHA" },
    { 97, 1, 1, L"AOTO_CAP" },
    { 98, 1, 1, L"MAN_CAP" },
    { 99, 3, 3, L"TOON" },
    { 100, 3, 3, L"SHIMMER" },
    { 101, 4, 4, L"ITEM_ALPHA" },
    { 102, 1, 1, L"MOVIE_CUT_CHG" },
    { 103, 3, 3, L"CHARA_LIGHT" },
    { 104, 3, 3, L"STAGE_LIGHT" },
    { 105, 8, 8, L"AGEAGE_CTRL" },          // AFT701
    { 106, 2, 2, L"PSE" },                  // FT
};

static const dsc_func_ac dsc_f_func[] = {
    { 0, 0, 0, L"END" },
    { 1, 1, 1, L"TIME" },
    { 2, 3, 4, L"MIKU_MOVE" },
    { 3, 1, 2, L"MIKU_ROT" },
    { 4, 1, 2, L"MIKU_DISP" },
    { 5, 1, 2, L"MIKU_SHADOW" },
    { 6, 7, 11, L"TARGET" },
    { 7, 3, 4, L"SET_MOTION" },
    { 8, 1, 2, L"SET_PLAYDATA" },
    { 9, 5, 6, L"EFFECT" },
    { 10, 2, 2, L"FADEIN_FIELD" },
    { 11, 1, 1, L"EFFECT_OFF" },
    { 12, 6, 6, L"SET_CAMERA" },
    { 13, 2, 2, L"DATA_CAMERA" },
    { 14, 1, 1, L"CHANGE_FIELD" },
    { 15, 1, 1, L"HIDE_FIELD" },
    { 16, 3, 3, L"MOVE_FIELD" },
    { 17, 2, 2, L"FADEOUT_FIELD" },
    { 18, 2, 3, L"EYE_ANIM" },
    { 19, 3, 5, L"MOUTH_ANIM" },
    { 20, 4, 5, L"HAND_ANIM" },
    { 21, 3, 4, L"LOOK_ANIM" },
    { 22, 3, 4, L"EXPRESSION" },
    { 23, 4, 5, L"LOOK_CAMERA" },
    { 24, 1, 2, L"LYRIC" },
    { 25, 0, 0, L"MUSIC_PLAY" },
    { 26, 1, 2, L"MODE_SELECT" },
    { 27, 2, 4, L"EDIT_MOTION" },
    { 28, 2, 2, L"BAR_TIME_SET" },
    { 29, 1, 2, L"SHADOWHEIGHT" },
    { 30, 2, 1, L"EDIT_FACE" },
    { 31, 19, 21, L"MOVE_CAMERA" },
    { 32, 0, 0, L"PV_END" },
    { 33, 2, 3, L"SHADOWPOS" },
    { 34, 2, 2, L"EDIT_LYRIC" },
    { 35, 5, 5, L"EDIT_TARGET" },
    { 36, 1, 1, L"EDIT_MOUTH" },
    { 37, 1, 1, L"SET_CHARA" },
    { 38, 7, 7, L"EDIT_MOVE" },
    { 39, 1, 1, L"EDIT_SHADOW" },
    { 40, 1, 1, L"EDIT_EYELID" },
    { 41, 2, 2, L"EDIT_EYE" },
    { 42, 1, 1, L"EDIT_ITEM" },
    { 43, 2, 2, L"EDIT_EFFECT" },
    { 44, 1, 1, L"EDIT_DISP" },
    { 45, 2, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, 3, L"AIM" },
    { 47, 3, 3, L"HAND_ITEM" },
    { 48, 1, 1, L"EDIT_BLUSH" },
    { 49, 2, 2, L"NEAR_CLIP" },
    { 50, 1, 2, L"CLOTH_WET" },
    { 51, 3, 3, L"LIGHT_ROT" },
    { 52, 6, 6, L"SCENE_FADE" },
    { 53, 6, 6, L"TONE_TRANS" },
    { 54, 1, 1, L"SATURATE" },
    { 55, 1, 1, L"FADE_MODE" },
    { 56, 2, 2, L"AUTO_BLINK" },
    { 57, 3, 3, L"PARTS_DISP" },
    { 58, 1, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, 2, L"CHARA_SIZE" },
    { 60, 2, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, 4, L"ITEM_ANIM" },
    { 62, 4, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, 1, L"SCENE_ROT" },
    { 64, 2, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, 1, L"PV_BRANCH_MODE" },
    { 66, 2, 2, L"DATA_CAMERA_START" },
    { 67, 1, 1, L"MOVIE_PLAY" },
    { 68, 1, 1, L"MOVIE_DISP" },
    { 69, 3, 3, L"WIND" },
    { 70, 3, 3, L"OSAGE_STEP" },
    { 71, 3, 3, L"OSAGE_MV_CCL" },
    { 72, 2, 2, L"CHARA_COLOR" },
    { 73, 1, 1, L"SE_EFFECT" },
    { 74, 9, 9, L"EDIT_MOVE_XYZ" },
    { 75, 3, 3, L"EDIT_EYELID_ANIM" },
    { 76, 2, 2, L"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, 4, L"EDIT_MOTION_LOOP" },
    { 78, 2, 2, L"EDIT_EXPRESSION" },
    { 79, 3, 3, L"EDIT_EYE_ANIM" },
    { 80, 2, 2, L"EDIT_MOUTH_ANIM" },
    { 81, 24, 24, L"EDIT_CAMERA" },
    { 82, 1, 1, L"EDIT_MODE_SELECT" },
    { 83, 2, 2, L"PV_END_FADEOUT" },
};

static const dsc_func dsc_f2_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 4, L"MIKU_MOVE" },
    { 3, 2, L"MIKU_ROT" },
    { 4, 2, L"MIKU_DISP" },
    { 5, 2, L"MIKU_SHADOW" },
    { 6, 12, L"TARGET" },
    { 7, 4, L"SET_MOTION" },
    { 8, 2, L"SET_PLAYDATA" },
    { 9, 6, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 2, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 3, L"EYE_ANIM" },
    { 19, 5, L"MOUTH_ANIM" },
    { 20, 5, L"HAND_ANIM" },
    { 21, 4, L"LOOK_ANIM" },
    { 22, 4, L"EXPRESSION" },
    { 23, 5, L"LOOK_CAMERA" },
    { 24, 2, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 2, L"MODE_SELECT" },
    { 27, 4, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 2, L"SHADOWHEIGHT" },
    { 30, 1, L"EDIT_FACE" },
    { 31, 21, L"MOVE_CAMERA" },
    { 32, 0, L"PV_END" },
    { 33, 3, L"SHADOWPOS" },
    { 34, 2, L"EDIT_LYRIC" },
    { 35, 5, L"EDIT_TARGET" },
    { 36, 1, L"EDIT_MOUTH" },
    { 37, 1, L"SET_CHARA" },
    { 38, 7, L"EDIT_MOVE" },
    { 39, 1, L"EDIT_SHADOW" },
    { 40, 1, L"EDIT_EYELID" },
    { 41, 2, L"EDIT_EYE" },
    { 42, 1, L"EDIT_ITEM" },
    { 43, 2, L"EDIT_EFFECT" },
    { 44, 1, L"EDIT_DISP" },
    { 45, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, L"AIM" },
    { 47, 3, L"HAND_ITEM" },
    { 48, 1, L"EDIT_BLUSH" },
    { 49, 2, L"NEAR_CLIP" },
    { 50, 2, L"CLOTH_WET" },
    { 51, 3, L"LIGHT_ROT" },
    { 52, 6, L"SCENE_FADE" },
    { 53, 6, L"TONE_TRANS" },
    { 54, 1, L"SATURATE" },
    { 55, 1, L"FADE_MODE" },
    { 56, 2, L"AUTO_BLINK" },
    { 57, 3, L"PARTS_DISP" },
    { 58, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, L"CHARA_SIZE" },
    { 60, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, L"ITEM_ANIM" },
    { 62, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, L"SCENE_ROT" },
    { 64, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, L"PV_BRANCH_MODE" },
    { 66, 2, L"DATA_CAMERA_START" },
    { 67, 1, L"MOVIE_PLAY" },
    { 68, 1, L"MOVIE_DISP" },
    { 69, 3, L"WIND" },
    { 70, 3, L"OSAGE_STEP" },
    { 71, 3, L"OSAGE_MV_CCL" },
    { 72, 2, L"CHARA_COLOR" },
    { 73, 1, L"SE_EFFECT" },
    { 74, 9, L"EDIT_MOVE_XYZ" },
    { 75, 3, L"EDIT_EYELID_ANIM" },
    { 76, 2, L"EDIT_INSTRUMENT_ITEM" },
    { 77, 4, L"EDIT_MOTION_LOOP" },
    { 78, 2, L"EDIT_EXPRESSION" },
    { 79, 3, L"EDIT_EYE_ANIM" },
    { 80, 2, L"EDIT_MOUTH_ANIM" },
    { 81, 22, L"EDIT_CAMERA" },
    { 82, 1, L"EDIT_MODE_SELECT" },
    { 83, 2, L"PV_END_FADEOUT" },
    { 84, 1, L"RESERVE" },
    { 85, 6, L"RESERVE" },
    { 86, 1, L"RESERVE" },
    { 87, 9, L"RESERVE" },
    { 88, 2, L"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, L"PV_CHARA_LIGHT" },
    { 90, 3, L"PV_STAGE_LIGHT" },
    { 91, 11, L"TARGET_EFFECT" },
    { 92, 3, L"FOG" },
    { 93, 2, L"f2_header" },
    { 94, 3, L"COLOR_CORRECTION" },
    { 95, 3, L"DOF" },
    { 96, 4, L"CHARA_ALPHA" },
    { 97, 1, L"AUTO_CAPTURE_BEGIN" },
    { 98, 1, L"MANUAL_CAPTURE" },
    { 99, 3, L"TOON_EDGE" },
    { 100, 3, L"SHIMMER" },
    { 101, 4, L"ITEM_ALPHA" },
    { 102, 1, L"MOVIE_CUT" },
    { 103, 1, L"CROSSFADE" },
    { 104, 1, L"SUBFRAMERENDER" },
    { 105, 36, L"EVENT_JUDGE" },
    { 106, 2, L"TOON\uFF3FEDGE" },
    { 107, 2, L"FOG_ENABLE" },
    { 108, 112, L"EDIT_CAMERA_BOX" },
    { 109, 1, L"EDIT_STAGE_PARAM" },
    { 110, 1, L"EDIT_CHANGE_FIELD" },
};

static const dsc_func dsc_x_func[] = {
    { 0, 0, L"END" },
    { 1, 1, L"TIME" },
    { 2, 4, L"MIKU_MOVE" },
    { 3, 2, L"MIKU_ROT" },
    { 4, 2, L"MIKU_DISP" },
    { 5, 2, L"MIKU_SHADOW" },
    { 6, 12, L"TARGET" },
    { 7, 4, L"SET_MOTION" },
    { 8, 2, L"SET_PLAYDATA" },
    { 9, 6, L"EFFECT" },
    { 10, 2, L"FADEIN_FIELD" },
    { 11, 1, L"EFFECT_OFF" },
    { 12, 6, L"SET_CAMERA" },
    { 13, 2, L"DATA_CAMERA" },
    { 14, 2, L"CHANGE_FIELD" },
    { 15, 1, L"HIDE_FIELD" },
    { 16, 3, L"MOVE_FIELD" },
    { 17, 2, L"FADEOUT_FIELD" },
    { 18, 3, L"EYE_ANIM" },
    { 19, 5, L"MOUTH_ANIM" },
    { 20, 5, L"HAND_ANIM" },
    { 21, 4, L"LOOK_ANIM" },
    { 22, 4, L"EXPRESSION" },
    { 23, 5, L"LOOK_CAMERA" },
    { 24, 2, L"LYRIC" },
    { 25, 0, L"MUSIC_PLAY" },
    { 26, 2, L"MODE_SELECT" },
    { 27, 4, L"EDIT_MOTION" },
    { 28, 2, L"BAR_TIME_SET" },
    { 29, 2, L"SHADOWHEIGHT" },
    { 30, 1, L"EDIT_FACE" },
    { 31, 21, L"DUMMY" },
    { 32, 0, L"PV_END" },
    { 33, 3, L"SHADOWPOS" },
    { 34, 2, L"EDIT_LYRIC" },
    { 35, 5, L"EDIT_TARGET" },
    { 36, 1, L"EDIT_MOUTH" },
    { 37, 1, L"SET_CHARA" },
    { 38, 7, L"EDIT_MOVE" },
    { 39, 1, L"EDIT_SHADOW" },
    { 40, 1, L"EDIT_EYELID" },
    { 41, 2, L"EDIT_EYE" },
    { 42, 1, L"EDIT_ITEM" },
    { 43, 2, L"EDIT_EFFECT" },
    { 44, 1, L"EDIT_DISP" },
    { 45, 2, L"EDIT_HAND_ANIM" },
    { 46, 3, L"AIM" },
    { 47, 3, L"HAND_ITEM" },
    { 48, 1, L"EDIT_BLUSH" },
    { 49, 2, L"NEAR_CLIP" },
    { 50, 2, L"CLOTH_WET" },
    { 51, 3, L"LIGHT_ROT" },
    { 52, 6, L"SCENE_FADE" },
    { 53, 6, L"TONE_TRANS" },
    { 54, 1, L"SATURATE" },
    { 55, 1, L"FADE_MODE" },
    { 56, 2, L"AUTO_BLINK" },
    { 57, 3, L"PARTS_DISP" },
    { 58, 1, L"TARGET_FLYING_TIME" },
    { 59, 2, L"CHARA_SIZE" },
    { 60, 2, L"CHARA_HEIGHT_ADJUST" },
    { 61, 4, L"ITEM_ANIM" },
    { 62, 4, L"CHARA_POS_ADJUST" },
    { 63, 1, L"SCENE_ROT" },
    { 64, 2, L"EDIT_MOT_SMOOTH_LEN" },
    { 65, 1, L"PV_BRANCH_MODE" },
    { 66, 2, L"DATA_CAMERA_START" },
    { 67, 1, L"MOVIE_PLAY" },
    { 68, 1, L"MOVIE_DISP" },
    { 69, 3, L"WIND" },
    { 70, 3, L"OSAGE_STEP" },
    { 71, 3, L"OSAGE_MV_CCL" },
    { 72, 2, L"CHARA_COLOR" },
    { 73, 1, L"SE_EFFECT" },
    { 74, 2, L"CHARA_SHADOW_QUALITY" },
    { 75, 2, L"STAGE_SHADOW_QUALITY" },
    { 76, 2, L"COMMON_LIGHT" },
    { 77, 2, L"TONE_MAP" },
    { 78, 2, L"IBL_COLOR" },
    { 79, 2, L"REFLECTION" },
    { 80, 3, L"CHROMATIC_ABERRATION" },
    { 81, 2, L"STAGE_SHADOW" },
    { 82, 2, L"REFLECTION_QUALITY" },
    { 83, 2, L"PV_END_FADEOUT" },
    { 84, 1, L"CREDIT_TITLE" },
    { 85, 1, L"BAR_POINT" },
    { 86, 1, L"BEAT_POINT" },
    { 87, 0, L"RESERVE" },
    { 88, 2, L"PV_AUTH_LIGHT_PRIORITY" },
    { 89, 3, L"PV_CHARA_LIGHT" },
    { 90, 3, L"PV_STAGE_LIGHT" },
    { 91, 11, L"TARGET_EFFECT" },
    { 92, 3, L"FOG" },
    { 93, 2, L"f2_header" },
    { 94, 3, L"COLOR_CORRECTION" },
    { 95, 3, L"DOF" },
    { 96, 4, L"CHARA_ALPHA" },
    { 97, 1, L"AUTO_CAPTURE_BEGIN" },
    { 98, 1, L"MANUAL_CAPTURE" },
    { 99, 3, L"TOON_EDGE" },
    { 100, 3, L"SHIMMER" },
    { 101, 4, L"ITEM_ALPHA" },
    { 102, 1, L"MOVIE_CUT" },
    { 103, 112, L"EDIT_CAMERA_BOX" },
    { 104, 1, L"EDIT_STAGE_PARAM" },
    { 105, 1, L"EDIT_CHANGE_FIELD" },
    { 106, 7, L"MIKUDAYO_ADJUST" },
    { 107, 2, L"LYRIC_2" },
    { 108, 2, L"LYRIC_READ" },
    { 109, 2, L"LYRIC_READ_2" },
    { 110, 5, L"ANNOTATION" },
    { 111, 2, L"STAGE_EFFECT" },
    { 112, 3, L"SONG_EFFECT" },
    { 113, 3, L"SONG_EFFECT_ATTACH" },
    { 114, 2, L"LIGHT_AUTH" },
    { 115, 2, L"FADE" },
    { 116, 2, L"SET_STAGE_EFFECT_ENV" },
    { 117, 2, L"RESERVE" },
    { 118, 2, L"COMMON_EFFECT_AET_FRONT" },
    { 119, 2, L"COMMON_EFFECT_AET_FRONT_LOW" },
    { 120, 2, L"COMMON_EFFECT_PARTICLE" },
    { 121, 3, L"SONG_EFFECT_ALPHA_SORT" },
    { 122, 5, L"LOOK_CAMERA_FACE_LIMIT" },
    { 123, 3, L"ITEM_LIGHT" },
    { 124, 3, L"CHARA_EFFECT" },
    { 125, 2, L"MARKER" },
    { 126, 3, L"CHARA_EFFECT_CHARA_LIGHT" },
    { 127, 2, L"ENABLE_COMMON_LIGHT_TO_CHARA" },
    { 128, 2, L"ENABLE_FXAA" },
    { 129, 2, L"ENABLE_TEMPORAL_AA" },
    { 130, 2, L"ENABLE_REFLECTION" },
    { 131, 2, L"BANK_BRANCH" },
    { 132, 2, L"BANK_END" },
    { 133, 2, L"" },
    { 134, 2, L"" },
    { 135, 2, L"" },
    { 136, 2, L"" },
    { 137, 2, L"" },
    { 138, 2, L"" },
    { 139, 2, L"" },
    { 140, 2, L"" },
    { 141, 2, L"VR_LIVE_MOVIE" },
    { 142, 2, L"VR_CHEER" },
    { 143, 2, L"VR_CHARA_PSMOVE" },
    { 144, 2, L"VR_MOVE_PATH" },
    { 145, 2, L"VR_SET_BASE" },
    { 146, 2, L"VR_TECH_DEMO_EFFECT" },
    { 147, 2, L"VR_TRANSFORM" },
    { 148, 2, L"GAZE" },
    { 149, 2, L"TECH_DEMO_GESUTRE" },
    { 150, 2, L"VR_CHEMICAL_LIGHT_COLOR" },
    { 151, 5, L"VR_LIVE_MOB" },
    { 152, 9, L"VR_LIVE_HAIR_OSAGE" },
    { 153, 9, L"VR_LIVE_LOOK_CAMERA" },
    { 154, 5, L"VR_LIVE_CHEER" },
    { 155, 3, L"VR_LIVE_GESTURE" },
    { 156, 7, L"VR_LIVE_CLONE" },
    { 157, 7, L"VR_LOOP_EFFECT" },
    { 158, 6, L"VR_LIVE_ONESHOT_EFFECT" },
    { 159, 9, L"VR_LIVE_PRESENT" },
    { 160, 5, L"VR_LIVE_TRANSFORM" },
    { 161, 5, L"VR_LIVE_FLY" },
    { 162, 2, L"VR_LIVE_CHARA_VOICE" },
};

extern int32_t dsc_ac101_get_func_length(int32_t id);
extern int32_t dsc_ac110_get_func_length(int32_t id);
extern int32_t dsc_ac120_get_func_length(int32_t id);
extern int32_t dsc_ac200_get_func_length(int32_t id);
extern int32_t dsc_ac210_get_func_length(int32_t id);
extern int32_t dsc_ac500_get_func_length(int32_t id);
extern int32_t dsc_ac510_get_func_length(int32_t id);
extern int32_t dsc_aft101_get_func_length(int32_t id);
extern int32_t dsc_aft200_get_func_length(int32_t id);
extern int32_t dsc_aft300_get_func_length(int32_t id);
extern int32_t dsc_aft310_get_func_length(int32_t id);
extern int32_t dsc_aft410_get_func_length(int32_t id);
extern int32_t dsc_aft701_get_func_length(int32_t id);
extern int32_t dsc_ft_get_func_length(int32_t id);
extern int32_t dsc_psp_get_func_length(int32_t id);
extern int32_t dsc_2nd_get_func_length(int32_t id);
extern int32_t dsc_f_get_func_length(int32_t id);
extern int32_t dsc_f2_get_func_length(int32_t id);
extern int32_t dsc_mgf_get_func_length(int32_t id);
extern int32_t dsc_x_get_func_length(int32_t id);

extern int32_t dsc_ac200_get_func_length_old(int32_t id);
extern int32_t dsc_ac210_get_func_length_old(int32_t id);
extern int32_t dsc_ac500_get_func_length_old(int32_t id);
extern int32_t dsc_ac510_get_func_length_old(int32_t id);
extern int32_t dsc_aft101_get_func_length_old(int32_t id);
extern int32_t dsc_aft200_get_func_length_old(int32_t id);
extern int32_t dsc_aft300_get_func_length_old(int32_t id);
extern int32_t dsc_aft310_get_func_length_old(int32_t id);
extern int32_t dsc_aft410_get_func_length_old(int32_t id);
extern int32_t dsc_aft701_get_func_length_old(int32_t id);
extern int32_t dsc_ft_get_func_length_old(int32_t id);
extern int32_t dsc_f_get_func_length_old(int32_t id);
