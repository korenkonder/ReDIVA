/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/light_param/wind.h"
#include "../../KKdLib/vec.h"
#include "../static_var.h"

typedef struct wind_spc {
    float_t cos;
    float_t sin;
} wind_spc;

typedef struct wind {
    float_t scale;
    float_t cycle;
    float_t rot_y;
    float_t rot_z;
    float_t bias;
    size_t count;
    wind_spc spc[16];
    vec3 wind_direction;
    float_t frame;
    float_t strength;
} wind;

extern wind* wind_init();
extern void wind_ctrl(wind* w);
extern float_t wind_get_scale(wind* w);
extern void wind_set_scale(wind* w, float_t value);
extern float_t wind_get_cycle(wind* w);
extern void wind_set_cycle(wind* w, float_t value);
extern void wind_get_rot(wind* w, vec2* value);
extern void wind_set_rot(wind* w, vec2* value);
extern float_t wind_get_rot_y(wind* w);
extern void wind_set_rot_y(wind* w, float_t value);
extern float_t wind_get_rot_z(wind* w);
extern void wind_set_rot_z(wind* w, float_t value);
extern float_t wind_get_bias(wind* w);
extern void wind_set_bias(wind* w, float_t value);
extern void wind_reset(wind* w);
extern void wind_free(wind* w);
