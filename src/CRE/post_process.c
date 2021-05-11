/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "static_var.h"

static void radius_calculate(radius* rad);
static void radius_calculate_gaussian_kernel(float_t* gaussian_kernel, float_t radius, int32_t spacing, int32_t offset);
static void intensity_calculate(intensity* inten);
static void tone_map_sat_gamma_calculate(tone_map_sat_gamma* tmsg);
static void tone_map_data_calculate(tone_map_data* tmd);

radius* radius_init() {
    radius* rad = force_malloc(sizeof(radius));
    return rad;
}

void radius_initialize(radius* rad, vec3* rgb) {
    rad->rgb = *rgb;
    radius_calculate(rad);
}

vec3* radius_get(radius* rad) {
    return &rad->rgb;
}

void radius_set(radius* rad, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 3.0f);
    temp.y = clamp(value->y, 0.0f, 3.0f);
    temp.z = clamp(value->z, 0.0f, 3.0f);
    if (temp.x != rad->rgb.x || temp.y != rad->rgb.y || temp.z != rad->rgb.z) {
        rad->rgb = temp;
        radius_calculate(rad);
    }
}

static void radius_calculate(radius* rad) {
    vec4 radius = (vec4){ rad->rgb.x, rad->rgb.y, rad->rgb.z, 0.0f };
    vec3_dot(rad->rgb, sv_rgb_to_luma, radius.w);
    rad->update = true;
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.x, 4, 0);
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.y, 4, 1);
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.z, 4, 2);
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.w, 4, 3);
}

static void radius_calculate_gaussian_kernel(float_t* gaussian_kernel, float_t radius, int32_t spacing, int32_t offset) {
    if (spacing < 1)
        spacing = 1;
    if (offset < 0)
        offset = 0;

    gaussian_kernel[0 * spacing + offset] = 1.0f;
    for (int32_t i = 1; i < GAUSSIAN_KERNEL_SIZE; i++)
        gaussian_kernel[i * spacing + offset] = 0.0f;
    double_t temp_gaussian_kernel[GAUSSIAN_KERNEL_SIZE];
    double_t s = radius * 0.8;
    s = -1.0 / (2.0 * s * s);
    double_t sum = 0.5;
    temp_gaussian_kernel[0] = 1.0;
    for (size_t i = 1; i < GAUSSIAN_KERNEL_SIZE; i++)
        sum += temp_gaussian_kernel[i] = exp(i * i * s);

    sum = 0.5 / sum;
    for (size_t i = 0; i < GAUSSIAN_KERNEL_SIZE; i++)
        gaussian_kernel[i * spacing + offset] = (float_t)(temp_gaussian_kernel[i] * sum);
}

void radius_dispose(radius* rad) {
    free(rad);
}

intensity* intensity_init() {
    intensity* inten = force_malloc(sizeof(radius));
    return inten;
}

void intensity_initialize(intensity* inten, vec3* rgb) {
    inten->rgb.x = clamp(rgb->x, 0.0f, 2.0f);
    inten->rgb.y = clamp(rgb->y, 0.0f, 2.0f);
    inten->rgb.z = clamp(rgb->z, 0.0f, 2.0f);
    inten->update = false;
    intensity_calculate(inten);
}

vec3* intensity_get(intensity* inten) {
    return &inten->rgb;
}

void intensity_set(intensity* inten, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 2.0f);
    temp.y = clamp(value->y, 0.0f, 2.0f);
    temp.z = clamp(value->z, 0.0f, 2.0f);
    if (temp.x != inten->rgb.x || temp.y != inten->rgb.y || temp.z != inten->rgb.z) {
        inten->rgb = temp;
        intensity_calculate(inten);
    }
}

static void intensity_calculate(intensity* inten) {
    inten->val = inten->rgb;
    inten->update = true;
}

void intensity_dispose(intensity* inten) {
    free(inten);
}

tone_map_sat_gamma* tone_map_sat_gamma_init() {
    tone_map_sat_gamma* tmsg = force_malloc(sizeof(tone_map_sat_gamma));
    return tmsg;
}

void tone_map_sat_gamma_initialize_rate(tone_map_sat_gamma* tmsg,
    float_t gamma, float_t gamma_rate, int32_t saturate1, float_t saturate2) {
    tmsg->gamma = clamp(gamma, 0.2f, 2.2f);
    tmsg->gamma_rate = clamp(gamma_rate, 0.5f, 2.0f);
    tmsg->saturate1 = clamp(saturate1, 1, 6);
    tmsg->saturate2 = clamp(saturate2, 0.0f, 1.0f);
    tmsg->update = false;
    tone_map_sat_gamma_calculate(tmsg);
}

void tone_map_sat_gamma_initialize(tone_map_sat_gamma* tmsg,
    float_t gamma, int32_t saturate1, float_t saturate2) {
    tmsg->gamma = clamp(gamma, 0.2f, 2.2f);
    tmsg->gamma_rate = clamp(1.0f, 0.5f, 2.0f);
    tmsg->saturate1 = clamp(saturate1, 1, 6);
    tmsg->saturate2 = clamp(saturate2, 0.0f, 1.0f);
    tmsg->update = false;
    tone_map_sat_gamma_calculate(tmsg);
}

float_t tone_map_sat_gamma_get_gamma(tone_map_sat_gamma* tmsg) {
    return tmsg->gamma;
}

void tone_map_sat_gamma_set_gamma(tone_map_sat_gamma* tmsg, float_t value) {
    value = clamp(value, 0.2f, 2.2f);
    if (value != tmsg->gamma) {
        tmsg->gamma = value;
        tone_map_sat_gamma_calculate(tmsg);
    }
}

float_t tone_map_sat_gamma_get_gamma_rate(tone_map_sat_gamma* tmsg) {
    return tmsg->gamma_rate;
}

void tone_map_sat_gamma_set_gamma_rate(tone_map_sat_gamma* tmsg, float_t value) {
    value = clamp(value, 0.5f, 2.0f);
    if (value != tmsg->gamma_rate) {
        tmsg->gamma_rate = value;
        tone_map_sat_gamma_calculate(tmsg);
    }
}

int32_t tone_map_sat_gamma_get_saturate1(tone_map_sat_gamma* tmsg) {
    return tmsg->saturate1;
}

void tone_map_sat_gamma_set_saturate1(tone_map_sat_gamma* tmsg, int32_t value) {
    value = clamp(value, 1, 6);
    if (value != tmsg->saturate1) {
        tmsg->saturate1 = value;
        tone_map_sat_gamma_calculate(tmsg);
    }
}

float_t tone_map_sat_gamma_get_saturate2(tone_map_sat_gamma* tmsg) {
    return tmsg->saturate2;
}

void tone_map_sat_gamma_set_saturate2(tone_map_sat_gamma* tmsg, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tmsg->saturate2) {
        tmsg->saturate2 = value;
        tone_map_sat_gamma_calculate(tmsg);
    }
}

static void tone_map_sat_gamma_calculate(tone_map_sat_gamma* tmsg) {
    const float_t tone_map_sat_gamma_scale = 1.0f / TONE_MAP_SAT_GAMMA_SAMPLES;
    const int32_t tone_map_sat_gamma_size = 16 * TONE_MAP_SAT_GAMMA_SAMPLES;

    int32_t i, j;
    int32_t saturate1;
    float_t saturate2;

    tmsg->update = true;
    vec2* v = tmsg->val;
    float_t gamma, v10, v11;
    gamma = tmsg->gamma * tmsg->gamma_rate * 1.5f;
    saturate1 = tmsg->saturate1;
    saturate2 = tmsg->saturate2;

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    for (i = 1; i < tone_map_sat_gamma_size; i++) {
        v11 = expf(-i * tone_map_sat_gamma_scale);
        v10 = powf(1.0f - v11, gamma);
        v11 = v10 * 2.0f - 1.0f;
        for (j = 0; j < saturate1; j++) {
            v11 *= v11;
            v11 *= v11;
            v11 *= v11;
            v11 *= v11;
        }

        v[i].x = v10;
        v[i].y = v10 * saturate2 * ((float_t)TONE_MAP_SAT_GAMMA_SAMPLES / (float_t)i) * (1.0f - v11);
    }
}

void tone_map_sat_gamma_dispose(tone_map_sat_gamma* tmsg) {
    free(tmsg);
}

tone_map_data* tone_map_data_init() {
    tone_map_data* tmd = force_malloc(sizeof(tone_map_data));
    return tmd;
}

void tone_map_data_initialize(tone_map_data* tmd, float_t exposure, bool auto_exposure,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method) {
    tmd->exposure = clamp(exposure, 0.0f, 4.0f);
    tmd->auto_exposure = auto_exposure;
    tmd->scene_fade_color.x = clamp(scene_fade_color->x, 0.0f, 1.0f);
    tmd->scene_fade_color.y = clamp(scene_fade_color->y, 0.0f, 1.0f);
    tmd->scene_fade_color.z = clamp(scene_fade_color->z, 0.0f, 1.0f);
    tmd->scene_fade_alpha = clamp(scene_fade_alpha, 0.0f, 1.0f);
    tmd->scene_fade_blend_func = clamp(scene_fade_blend_func, 0, 2);
    tmd->tone_trans_start.x = clamp(tone_trans_start->x, 0.0f, 1.0f);
    tmd->tone_trans_start.y = clamp(tone_trans_start->y, 0.0f, 1.0f);
    tmd->tone_trans_start.z = clamp(tone_trans_start->z, 0.0f, 1.0f);
    tmd->tone_trans_end.x = clamp(tone_trans_end->x, 0.0f, 1.0f);
    tmd->tone_trans_end.y = clamp(tone_trans_end->y, 0.0f, 1.0f);
    tmd->tone_trans_end.z = clamp(tone_trans_end->z, 0.0f, 1.0f);
    tmd->tone_map_method = clamp(tone_map_method, 0, 2);
    tmd->update = false;
    tone_map_data_calculate(tmd);
}

float_t tone_map_data_get_exposure(tone_map_data* tmd) {
    return tmd->exposure;
}

void tone_map_data_set_exposure(tone_map_data* tmd, float_t value) {
    value = clamp(value, 0.0f, 4.0f);
    if (value != tmd->exposure) {
        tmd->exposure = value;
        tone_map_data_calculate(tmd);
    }
}

bool tone_map_data_get_auto_exposure(tone_map_data* tmd) {
    return tmd->auto_exposure;
}

void tone_map_data_set_auto_exposure(tone_map_data* tmd, bool value) {
    if (value != tmd->auto_exposure) {
        tmd->auto_exposure = value;
        tone_map_data_calculate(tmd);
    }
}

vec3* tone_map_data_get_scene_fade_color(tone_map_data* tmd) {
    return &tmd->scene_fade_color;
}

void tone_map_data_set_scene_fade_color(tone_map_data* tmd, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    if (temp.x != tmd->scene_fade_color.x
        || temp.y != tmd->scene_fade_color.y
        || temp.z != tmd->scene_fade_color.z) {
        tmd->scene_fade_color = temp;
        tone_map_data_calculate(tmd);
    }
}

float_t tone_map_data_get_scene_fade_alpha(tone_map_data* tmd) {
    return tmd->scene_fade_alpha;
}

void tone_map_data_set_scene_fade_alpha(tone_map_data* tmd, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tmd->scene_fade_alpha) {
        tmd->scene_fade_alpha = value;
        tone_map_data_calculate(tmd);
    }
}

int32_t tone_map_data_get_scene_fade_blend_func(tone_map_data* tmd) {
    return tmd->scene_fade_blend_func;
}

void tone_map_data_set_scene_fade_blend_func(tone_map_data* tmd, int32_t value) {
    value = clamp(value, 0, 2);
    if (value != tmd->scene_fade_blend_func) {
        tmd->scene_fade_blend_func = value;
        tone_map_data_calculate(tmd);
    }
}

vec3* tone_map_data_get_tone_trans_start(tone_map_data* tmd) {
    return &tmd->tone_trans_start;
}

void tone_map_data_set_tone_trans_start(tone_map_data* tmd, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    if (temp.x != tmd->tone_trans_start.x
        || temp.y != tmd->tone_trans_start.y
        || temp.z != tmd->tone_trans_start.z) {
        tmd->tone_trans_start = temp;
        tone_map_data_calculate(tmd);
    }
}

vec3* tone_map_data_get_tone_trans_end(tone_map_data* tmd) {
    return &tmd->tone_trans_end;
}

void tone_map_data_set_tone_trans_end(tone_map_data* tmd, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.009999999776f, 1.0f);
    temp.y = clamp(value->y, 0.009999999776f, 1.0f);
    temp.z = clamp(value->z, 0.009999999776f, 1.0f);
    if (temp.x != tmd->tone_trans_end.x
        || temp.y != tmd->tone_trans_end.y
        || temp.z != tmd->tone_trans_end.z) {
        tmd->tone_trans_end = temp;
        tone_map_data_calculate(tmd);
    }
}

int32_t tone_map_data_get_tone_map_method(tone_map_data* tmd) {
    return tmd->tone_map_method;
}

void tone_map_data_set_tone_map_method(tone_map_data* tmd, int32_t value) {
    value = clamp(value, 0, 2);
    if (value != tmd->tone_map_method) {
        tmd->tone_map_method = value;
        tone_map_data_calculate(tmd);
    }
}

static void tone_map_data_calculate(tone_map_data* tmd) {
    tmd->update = true;

    vec3 tone_trans, tone_trans_scale, tone_trans_offset;
    vec3_sub(tmd->tone_trans_end, tmd->tone_trans_start, tone_trans);
    vec3_rcp(tone_trans, tone_trans_scale);
    vec3_mult(tone_trans_scale, tmd->tone_trans_start, tone_trans_offset);
    vec3_negate(tone_trans_offset, tone_trans_offset);

    float_t* v = tmd->val;
    v[0] = tmd->exposure;
    v[1] = 0.0625f;
    v[2] = tmd->exposure * 0.5f;
    v[3] = tmd->auto_exposure ? 1.0f : 0.0f;
    v[4] = tmd->scene_fade_color.x;
    v[5] = tmd->scene_fade_color.y;
    v[6] = tmd->scene_fade_color.z;
    v[7] = tmd->scene_fade_alpha;
    v[8] = tone_trans_scale.x;
    v[9] = tone_trans_scale.y;
    v[10] = tone_trans_scale.z;
    v[12] = tone_trans_offset.x;
    v[13] = tone_trans_offset.y;
    v[14] = tone_trans_offset.z;
    v[15] = (float_t)tmd->scene_fade_blend_func;
}

void tone_map_data_dispose(tone_map_data* tmd) {
    free(tmd);
}
