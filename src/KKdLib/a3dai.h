/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "kf.h"
#include "auth_3d.h"
#include "a3da_head.h"

typedef struct a3dai {
    auth_3d_struct* auth_3d;
    float_t frame;
    float_t time;
    float_t value;
    float_t delta_frame;
    float_t interpolation_framerate;
    float_t requested_framerate;

    kft3* first_key;
    kft3* last_key;
} a3dai;

extern a3dai* a3da_key_to_a3dai(auth_3d_key* key, float_t interpolation_framerate, float_t requested_framerate);
extern a3dai* a3da_head_to_a3dai(auth_3d_struct* head, float_t interpolation_framerate, float_t requested_framerate);
extern void a3dai_set_interpolation_framerate(a3dai* interp, float_t interpolation_framerate);
extern void a3dai_set_requested_framerate(a3dai* interp, float_t requested_framerate);
extern void a3dai_update(a3dai* interp);
extern void a3dai_reset(a3dai* interp);
extern float_t a3dai_set_time(a3dai* interp, float_t time);
extern float_t a3dai_set_frame(a3dai* interp, float_t frame);
extern float_t a3dai_add_time(a3dai* interp, float_t time);
extern float_t a3dai_next_frame(a3dai* interp);
