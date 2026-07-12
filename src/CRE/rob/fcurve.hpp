/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"

enum FcurveKeyKind {
    FC_STATIC_0 = 0,
    FC_STATIC_DATA,
    FC_SPLINE_SLOPE_0,
    FC_SPLINE_DATA,
    FC_MAX,
};

struct FcurveKey {
    FcurveKeyKind kind;
    int32_t sum;
    int32_t cache_idx;
    int32_t last_idx;
    const uint16_t* num;
    const float_t* val;
};

struct struc_369 {
    int32_t field_0;
    float_t field_4;
};

struct Fcurve {
    uint16_t fc_max;
    uint16_t frame_max;
    uint16_t key_num_type;
    FcurveKey* fck_ptr;

    void clear_fcurve_key();
    void dbg_print_fcurve();
    bool fcurve_init(const void* data);
    void fcurve_init_u16(const void* data);
    void get_fcurve_value(float_t frame, float_t* value,
        FcurveKey* fck, uint32_t in_fc_max, const struc_369* a6);
    void get_fcurve_value_end(float_t* value);
    void get_fcurve_value_start(float_t* value, uint8_t in_fc_offset, uint8_t in_fc_max);
    void set_fck_ptr(FcurveKey* ptr);
};
