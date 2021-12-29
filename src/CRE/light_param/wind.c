/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wind.h"

static float_t wind_apply_spc(wind* wind, float_t frame);

wind* wind_init() {
    wind* w = force_malloc(sizeof(wind));
    w->count = 16;
    return w;
}

float_t wind_get_scale(wind* w) {
    return w->scale;
}

void wind_set_scale(wind* w, float_t value) {
    w->scale = value;
}

float_t wind_get_cycle(wind* w) {
    return w->cycle;
}

void wind_set_cycle(wind* w, float_t value) {
    w->cycle = value;
}

void wind_get_rot(wind* w, vec2* value) {
    value->x = w->rot_y;
    value->y = w->rot_z;
}

void wind_set_rot(wind* w, vec2* value) {
    w->rot_y = value->x;
    w->rot_z = value->y;
}

float_t wind_get_rot_y(wind* w) {
    return w->rot_y;
}

void wind_set_rot_y(wind* w, float_t value) {
    w->rot_y = value;
}

float_t wind_get_rot_z(wind* w) {
    return w->rot_z;
}

void wind_set_rot_z(wind* w, float_t value) {
    w->rot_z = value;
}

float_t wind_get_bias(wind* w) {
    return w->bias;
}

void wind_set_bias(wind* w, float_t value) {
    w->bias = value;
}

void wind_reset(wind* w) {
    w->strength = 0.0f;
    w->frame = 0.0f;
}

void wind_update(wind* w) {
    float_t value = wind_apply_spc(w, w->cycle * w->frame) * w->scale;
    mat4 mat = mat4_identity;
    mat4_rotate_y_mult(&mat, w->rot_y * DEG_TO_RAD_FLOAT, &mat);
    mat4_rotate_z_mult(&mat, w->rot_z * DEG_TO_RAD_FLOAT, &mat);
    vec3 wind_direction = vec3_identity;
    wind_direction.x = value;
    mat4_mult_vec3(&mat, &wind_direction, &wind_direction);
    vec3_mult_scalar(wind_direction, w->strength, w->wind_direction);
    if (w->strength >= 1.0f)
        w->strength = 1.0f;
    else
        w->strength = w->strength + (float_t)(1.0 / 60.0);

    w->frame += get_delta_frame();
    if (w->frame > 65535.0f)
        w->frame = 0.0f;
}

void wind_free(wind* w) {
    free(w);
}

static float_t wind_apply_spc(wind* wind, float_t frame) {
    size_t count = wind->count;
    float_t value = 0.0f;
    wind_spc* spc = wind->spc;
    for (size_t i = 0; i < count; i++, spc++) {
        float_t power = (float_t)(1 << (int32_t)i) * frame;
        value += cosf(power) * spc->cos + sinf(power) * spc->sin;
    }
    return value + wind->bias;
}
