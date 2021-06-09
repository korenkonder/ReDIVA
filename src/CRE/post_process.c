/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "static_var.h"

static void radius_calculate(radius* rad);
static void radius_calculate_gaussian_kernel(float_t* gaussian_kernel, float_t radius, int32_t spacing, int32_t offset);
static void intensity_calculate(intensity* inten);
static void tone_map_calculate_data(tone_map* tm);
static void tone_map_calculate_tex(tone_map* tm);

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

tone_map* tone_map_init() {
    tone_map* tm = force_malloc(sizeof(tone_map));
    return tm;
}

void tone_map_initialize(tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, int32_t saturate1, float_t saturate2,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method) {
    tone_map_initialize_rate(tm, exposure, auto_exposure, 1.0f,
        gamma, saturate1, saturate2, scene_fade_color, scene_fade_alpha, scene_fade_blend_func,
        tone_trans_start, tone_trans_end, tone_map_method);
}

void tone_map_initialize_rate(tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, float_t gamma_rate, int32_t saturate1, float_t saturate2,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method) {
    tm->exposure = clamp(exposure, 0.0f, 4.0f);
    tm->auto_exposure = auto_exposure;
    tm->gamma = clamp(gamma, 0.2f, 2.2f);
    tm->gamma_rate = clamp(gamma_rate, 0.5f, 2.0f);
    tm->saturate1 = clamp(saturate1, 1, 6);
    tm->saturate2 = clamp(saturate2, 0.0f, 1.0f);
    tm->scene_fade_color.x = clamp(scene_fade_color->x, 0.0f, 1.0f);
    tm->scene_fade_color.y = clamp(scene_fade_color->y, 0.0f, 1.0f);
    tm->scene_fade_color.z = clamp(scene_fade_color->z, 0.0f, 1.0f);
    tm->scene_fade_alpha = clamp(scene_fade_alpha, 0.0f, 1.0f);
    tm->scene_fade_blend_func = clamp(scene_fade_blend_func, 0, 2);
    tm->tone_trans_start.x = clamp(tone_trans_start->x, 0.0f, 1.0f);
    tm->tone_trans_start.y = clamp(tone_trans_start->y, 0.0f, 1.0f);
    tm->tone_trans_start.z = clamp(tone_trans_start->z, 0.0f, 1.0f);
    tm->tone_trans_end.x = clamp(tone_trans_end->x, 0.0f, 1.0f);
    tm->tone_trans_end.y = clamp(tone_trans_end->y, 0.0f, 1.0f);
    tm->tone_trans_end.z = clamp(tone_trans_end->z, 0.0f, 1.0f);
    tm->tone_map_method = clamp(tone_map_method, 0, 2);
    tm->update_data = false;
    tm->update_tex = false;
    tone_map_calculate_data(tm);
    tone_map_calculate_tex(tm);
}

float_t tone_map_get_exposure(tone_map* tm) {
    return tm->exposure;
}

void tone_map_set_exposure(tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 4.0f);
    if (value != tm->exposure) {
        tm->exposure = value;
        tone_map_calculate_data(tm);
    }
}

bool tone_map_get_auto_exposure(tone_map* tm) {
    return tm->auto_exposure;
}

void tone_map_set_auto_exposure(tone_map* tm, bool value) {
    if (value != tm->auto_exposure) {
        tm->auto_exposure = value;
        tone_map_calculate_data(tm);
    }
}

float_t tone_map_get_gamma(tone_map* tm) {
    return tm->gamma;
}

void tone_map_set_gamma(tone_map* tm, float_t value) {
    value = clamp(value, 0.2f, 2.2f);
    if (value != tm->gamma) {
        tm->gamma = value;
        tone_map_calculate_data(tm);
        tone_map_calculate_tex(tm);
    }
}

float_t tone_map_get_gamma_rate(tone_map* tm) {
    return tm->gamma_rate;
}

void tone_map_set_gamma_rate(tone_map* tm, float_t value) {
    value = clamp(value, 0.5f, 2.0f);
    if (value != tm->gamma_rate) {
        tm->gamma_rate = value;
        tone_map_calculate_tex(tm);
    }
}

int32_t tone_map_get_saturate1(tone_map* tm) {
    return tm->saturate1;
}

void tone_map_set_saturate1(tone_map* tm, int32_t value) {
    value = clamp(value, 1, 6);
    if (value != tm->saturate1) {
        tm->saturate1 = value;
        tone_map_calculate_tex(tm);
    }
}

float_t tone_map_get_saturate2(tone_map* tm) {
    return tm->saturate2;
}

void tone_map_set_saturate2(tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->saturate2) {
        tm->saturate2 = value;
        tone_map_calculate_tex(tm);
    }
}

vec3* tone_map_get_scene_fade_color(tone_map* tm) {
    return &tm->scene_fade_color;
}

void tone_map_set_scene_fade_color(tone_map* tm, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    if (temp.x != tm->scene_fade_color.x
        || temp.y != tm->scene_fade_color.y
        || temp.z != tm->scene_fade_color.z) {
        tm->scene_fade_color = temp;
        tone_map_calculate_data(tm);
    }
}

float_t tone_map_get_scene_fade_alpha(tone_map* tm) {
    return tm->scene_fade_alpha;
}

void tone_map_set_scene_fade_alpha(tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->scene_fade_alpha) {
        tm->scene_fade_alpha = value;
        tone_map_calculate_data(tm);
    }
}

int32_t tone_map_get_scene_fade_blend_func(tone_map* tm) {
    return tm->scene_fade_blend_func;
}

void tone_map_set_scene_fade_blend_func(tone_map* tm, int32_t value) {
    value = clamp(value, 0, 2);
    if (value != tm->scene_fade_blend_func) {
        tm->scene_fade_blend_func = value;
        tone_map_calculate_data(tm);
    }
}

vec3* tone_map_get_tone_trans_start(tone_map* tm) {
    return &tm->tone_trans_start;
}

void tone_map_set_tone_trans_start(tone_map* tm, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.0f, 1.0f);
    temp.y = clamp(value->y, 0.0f, 1.0f);
    temp.z = clamp(value->z, 0.0f, 1.0f);
    if (temp.x != tm->tone_trans_start.x
        || temp.y != tm->tone_trans_start.y
        || temp.z != tm->tone_trans_start.z) {
        tm->tone_trans_start = temp;
        tone_map_calculate_data(tm);
    }
}

vec3* tone_map_get_tone_trans_end(tone_map* tm) {
    return &tm->tone_trans_end;
}

void tone_map_set_tone_trans_end(tone_map* tm, vec3* value) {
    vec3 temp;
    temp.x = clamp(value->x, 0.009999999776f, 1.0f);
    temp.y = clamp(value->y, 0.009999999776f, 1.0f);
    temp.z = clamp(value->z, 0.009999999776f, 1.0f);
    if (temp.x != tm->tone_trans_end.x
        || temp.y != tm->tone_trans_end.y
        || temp.z != tm->tone_trans_end.z) {
        tm->tone_trans_end = temp;
        tone_map_calculate_data(tm);
    }
}

int32_t tone_map_get_tone_map_method(tone_map* tm) {
    return tm->tone_map_method;
}

void tone_map_set_tone_map_method(tone_map* tm, int32_t value) {
    value = clamp(value, 0, 2);
    if (value != tm->tone_map_method) {
        tm->tone_map_method = value;
        tone_map_calculate_data(tm);
    }
}

static void tone_map_calculate_data(tone_map* tm) {
    tm->update_data = true;

    vec3 tone_trans, tone_trans_scale, tone_trans_offset;
    vec3_sub(tm->tone_trans_end, tm->tone_trans_start, tone_trans);
    vec3_rcp(tone_trans, tone_trans_scale);
    vec3_mult(tone_trans_scale, tm->tone_trans_start, tone_trans_offset);
    vec3_negate(tone_trans_offset, tone_trans_offset);

    tone_map_data* v = &tm->data;
    v->p_exposure.x = tm->exposure;
    v->p_exposure.y = 0.0625f;
    v->p_exposure.z = tm->exposure * 0.5f;
    v->p_exposure.w = tm->auto_exposure ? 1.0f : 0.0f;
    v->p_fade_color.x = tm->scene_fade_color.x;
    v->p_fade_color.y = tm->scene_fade_color.y;
    v->p_fade_color.z = tm->scene_fade_color.z;
    v->p_fade_color.w = tm->scene_fade_alpha;
    v->p_tone_scale.x = tone_trans_scale.x;
    v->p_tone_scale.y = tone_trans_scale.y;
    v->p_tone_scale.z = tone_trans_scale.z;
    v->p_tone_offset.x = tone_trans_offset.x;
    v->p_tone_offset.y = tone_trans_offset.y;
    v->p_tone_offset.z = tone_trans_offset.z;
    v->p_fade_func.x = (float_t)tm->scene_fade_blend_func;
    v->p_inv_tone.x = tm->gamma > 0.0f ? 2.0f / (tm->gamma * 3.0f) : 0.0f;
}

static void tone_map_calculate_tex(tone_map* tm) {
    const float_t tone_map_scale = 1.0f / TONE_MAP_SAT_GAMMA_SAMPLES;
    const int32_t tone_map_size = 16 * TONE_MAP_SAT_GAMMA_SAMPLES;

    int32_t i, j;
    int32_t saturate1;
    float_t saturate2;

    tm->update_tex = true;
    vec2* v = tm->tex;
    float_t gamma, v10, v11;
    gamma = tm->gamma * tm->gamma_rate * 1.5f;
    saturate1 = tm->saturate1;
    saturate2 = tm->saturate2;

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    for (i = 1; i < tone_map_size; i++) {
        v11 = expf(-i * tone_map_scale);
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

void tone_map_dispose(tone_map* tm) {
    free(tm);
}
