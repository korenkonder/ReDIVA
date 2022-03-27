/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
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

typedef enum dsc_type {
    DSC_NONE = 0,
    DSC_AC101,
    DSC_AC110,
    DSC_AC120,
    DSC_AC200,
    DSC_AC210,
    DSC_AC500,
    DSC_AC510,
    DSC_AFT101,
    DSC_AFT200,
    DSC_AFT300,
    DSC_AFT310,
    DSC_AFT410,
    DSC_AFT701,
    DSC_FT,
    DSC_PSP,
    DSC_2ND,
    DSC_DT,
    DSC_DT2,
    DSC_F,
    DSC_F2,
    DSC_MGF,
    DSC_X,
    DSC_VRFL,
} dsc_type;

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
    int32_t target_flying_time;
    int32_t time_signature;
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
    int32_t target_flying_time;
    int32_t time_signature;
    int32_t target_effect;
} dsc_note12;
#pragma endregion

#pragma region dsc_func
typedef struct dsc_func {
    int32_t id;
    int32_t length;
    const char* name;
} dsc_func;

typedef struct dsc_func_ac {
    int32_t id;
    int32_t length_old;
    int32_t length;
    const char* name;
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

typedef struct dsc_data {
    const char* name;
    int32_t func;
    uint32_t data_offset;
} dsc_data;

class dsc_replace;
typedef struct dsc_replace_data dsc_replace_data;

typedef int32_t(*dsc_get_func_id)(const char* name);
typedef int32_t(*dsc_get_func_length)(int32_t id);
typedef const char*(*dsc_get_func_name)(int32_t id);
typedef const void(*dsc_func_convert)(dsc_replace* dr, dsc_replace_data* drd, uint32_t* data);

class dsc {
public:
    dsc_type type;
    uint32_t signature;
    uint32_t id;
    std::vector<dsc_data> data;
    std::vector<uint32_t> data_buffer;

    dsc(dsc_type type = DSC_FT, uint32_t signature = 0x15122517, uint32_t id = 0);
    ~dsc();

    uint32_t* add_func(const char* name, int32_t func, int32_t func_length);
    void convert(dsc_type dst_type);
    dsc_get_func_length get_dsc_get_func_length();
    uint32_t* get_func_data(dsc_data* data);
    void merge(int32_t count, ...);
    bool parse(void* data, size_t length, dsc_type type);
    void rebuild();
    void unparse(void** data, size_t* length);

    static int32_t calculate_target_flying_time(int32_t bpm, int32_t time_signature);
    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct dsc_replace_data {
    const char* name;
    int32_t func_id;
    int32_t length;
    dsc_func_convert func;
};

class dsc_replace {
public:
    dsc_type src_type;
    dsc_type dst_type;
    dsc_replace_data* replace_data;
    dsc dsc;
    bool dst_has_perf_id;

    int32_t bpm;
    int32_t time_signature;
    int32_t target_flying_time;

    int32_t bar_time_set_func_id;
    int32_t target_flying_time_func_id;

    dsc_replace(dsc_type src_type, dsc_type dst_type, bool dst_has_perf_id);
    ~dsc_replace();
};

extern const int32_t dsc_ac101_func_count;
extern const int32_t dsc_ac110_func_count;
extern const int32_t dsc_ac120_func_count;
extern const int32_t dsc_ac200_func_count;
extern const int32_t dsc_ac210_func_count;
extern const int32_t dsc_ac500_func_count;
extern const int32_t dsc_ac510_func_count;
extern const int32_t dsc_aft101_func_count;
extern const int32_t dsc_aft200_func_count;
extern const int32_t dsc_aft300_func_count;
extern const int32_t dsc_aft310_func_count;
extern const int32_t dsc_aft410_func_count;
extern const int32_t dsc_aft701_func_count;
extern const int32_t dsc_ft_func_count;
extern const int32_t dsc_psp_func_count;
extern const int32_t dsc_2nd_func_count;
extern const int32_t dsc_dt_func_count;
extern const int32_t dsc_dt2_func_count;
extern const int32_t dsc_f_func_count;
extern const int32_t dsc_f2_func_count;
extern const int32_t dsc_mgf_func_count;
extern const int32_t dsc_x_func_count;
extern const int32_t dsc_vrfl_func_count;

extern int32_t dsc_ac101_get_func_id(const char* name);
extern int32_t dsc_ac110_get_func_id(const char* name);
extern int32_t dsc_ac120_get_func_id(const char* name);
extern int32_t dsc_ac200_get_func_id(const char* name);
extern int32_t dsc_ac210_get_func_id(const char* name);
extern int32_t dsc_ac500_get_func_id(const char* name);
extern int32_t dsc_ac510_get_func_id(const char* name);
extern int32_t dsc_aft101_get_func_id(const char* name);
extern int32_t dsc_aft200_get_func_id(const char* name);
extern int32_t dsc_aft300_get_func_id(const char* name);
extern int32_t dsc_aft310_get_func_id(const char* name);
extern int32_t dsc_aft410_get_func_id(const char* name);
extern int32_t dsc_aft701_get_func_id(const char* name);
extern int32_t dsc_ft_get_func_id(const char* name);
extern int32_t dsc_psp_get_func_id(const char* name);
extern int32_t dsc_2nd_get_func_id(const char* name);
extern int32_t dsc_dt_get_func_id(const char* name);
extern int32_t dsc_dt2_get_func_id(const char* name);
extern int32_t dsc_f_get_func_id(const char* name);
extern int32_t dsc_f2_get_func_id(const char* name);
extern int32_t dsc_mgf_get_func_id(const char* name);
extern int32_t dsc_x_get_func_id(const char* name);
extern int32_t dsc_vrfl_get_func_id(const char* name);

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
extern int32_t dsc_dt_get_func_length(int32_t id);
extern int32_t dsc_dt2_get_func_length(int32_t id);
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
extern int32_t dsc_dt2_get_func_length_old(int32_t id);
extern int32_t dsc_f_get_func_length_old(int32_t id);

extern const char* dsc_ac101_get_func_name(int32_t id);
extern const char* dsc_ac110_get_func_name(int32_t id);
extern const char* dsc_ac120_get_func_name(int32_t id);
extern const char* dsc_ac200_get_func_name(int32_t id);
extern const char* dsc_ac210_get_func_name(int32_t id);
extern const char* dsc_ac500_get_func_name(int32_t id);
extern const char* dsc_ac510_get_func_name(int32_t id);
extern const char* dsc_aft101_get_func_name(int32_t id);
extern const char* dsc_aft200_get_func_name(int32_t id);
extern const char* dsc_aft300_get_func_name(int32_t id);
extern const char* dsc_aft310_get_func_name(int32_t id);
extern const char* dsc_aft410_get_func_name(int32_t id);
extern const char* dsc_aft701_get_func_name(int32_t id);
extern const char* dsc_ft_get_func_name(int32_t id);
extern const char* dsc_psp_get_func_name(int32_t id);
extern const char* dsc_2nd_get_func_name(int32_t id);
extern const char* dsc_dt_get_func_name(int32_t id);
extern const char* dsc_dt2_get_func_name(int32_t id);
extern const char* dsc_f_get_func_name(int32_t id);
extern const char* dsc_f2_get_func_name(int32_t id);
extern const char* dsc_mgf_get_func_name(int32_t id);
extern const char* dsc_x_get_func_name(int32_t id);
extern const char* dsc_vrfl_get_func_name(int32_t id);

extern const int32_t dsc_func_count_array[];
extern const dsc_get_func_length dsc_get_func_length_array[];
extern const dsc_get_func_length dsc_get_func_length_old_array[];
extern const dsc_get_func_name dsc_get_func_name_array[];

extern int32_t dsc_type_get_dsc_func_count(dsc_type type);
extern dsc_get_func_id dsc_type_get_dsc_get_func_id(dsc_type type);
extern dsc_get_func_length dsc_type_get_dsc_get_func_length(dsc_type type, bool has_perf_id);
extern dsc_get_func_name dsc_type_get_dsc_get_func_name(dsc_type type);
