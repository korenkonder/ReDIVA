/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef enum dsc_diff {
    DSC_DIFF_EASY    = 0,
    DSC_DIFF_NORMAL  = 1,
    DSC_DIFF_HARD    = 2,
    DSC_DIFF_EXTREME = 3,
    DSC_DIFF_ENCORE  = 4,
} dsc_diff;

typedef enum dsc_f2_mode {
    DSC_MODE_SYSTEM = 0,
    DSC_MODE_MOUTH  = 1,
    DSC_MODE_SCENE  = 2,
    DSC_MODE_DAYO   = 3,
} dsc_f2_mode;

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
extern int32_t dsc_vrfl_get_func_length(int32_t id);

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
