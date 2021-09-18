/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "static_var.h"

static void radius_calculate(radius* rad);
static void radius_calculate_gaussian_kernel(float_t* gaussian_kernel,
    float_t radius, int32_t stride, int32_t offset);
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
    inten->val = inten->rgb;
    inten->update = true;
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
        inten->val = inten->rgb;
        inten->update = true;
    }
}

void intensity_dispose(intensity* inten) {
    free(inten);
}

tone_map* tone_map_init() {
    tone_map* tm = force_malloc(sizeof(tone_map));
    return tm;
}

void tone_map_initialize(tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method) {
    tone_map_initialize_rate(tm, exposure, auto_exposure, 1.0f,
        gamma, saturate_power, saturate_coeff, scene_fade_color, scene_fade_alpha, scene_fade_blend_func,
        tone_trans_start, tone_trans_end, tone_map_method);
}

void tone_map_initialize_rate(tone_map* tm, float_t exposure, bool auto_exposure,
    float_t gamma, float_t gamma_rate, int32_t saturate_power, float_t saturate_coeff,
    vec3* scene_fade_color, float_t scene_fade_alpha, int32_t scene_fade_blend_func,
    vec3* tone_trans_start, vec3* tone_trans_end, int32_t tone_map_method) {
    tm->exposure = clamp(exposure, 0.0f, 4.0f);
    tm->auto_exposure = auto_exposure;
    tm->gamma = clamp(gamma, 0.2f, 2.2f);
    tm->gamma_rate = clamp(gamma_rate, 0.5f, 2.0f);
    tm->saturate_power = clamp(saturate_power, 1, 6);
    tm->saturate_coeff = clamp(saturate_coeff, 0.0f, 1.0f);
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

int32_t tone_map_get_saturate_power(tone_map* tm) {
    return tm->saturate_power;
}

void tone_map_set_saturate_power(tone_map* tm, int32_t value) {
    value = clamp(value, 1, 6);
    if (value != tm->saturate_power) {
        tm->saturate_power = value;
        tone_map_calculate_tex(tm);
    }
}

float_t tone_map_get_saturate_coeff(tone_map* tm) {
    return tm->saturate_coeff;
}

void tone_map_set_saturate_coeff(tone_map* tm, float_t value) {
    value = clamp(value, 0.0f, 1.0f);
    if (value != tm->saturate_coeff) {
        tm->saturate_coeff = value;
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

void tone_map_dispose(tone_map* tm) {
    free(tm);
}

static void radius_calculate(radius* rad) {
    float_t radius_scale = 0.8f;
    vec3 radius = rad->rgb;
    rad->update = true;
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.x * radius_scale, 3, 0);
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.y * radius_scale, 3, 1);
    radius_calculate_gaussian_kernel((float_t*)rad->val, radius.z * radius_scale, 3, 2);
}

static void radius_calculate_gaussian_kernel(float_t* gaussian_kernel,
    float_t radius, int32_t stride, int32_t offset) {
    if (stride < 1)
        stride = 1;
    if (offset < 0)
        offset = 0;

    gaussian_kernel[0 * stride + offset] = 1.0f;
    for (int32_t i = 1; i < GAUSSIAN_KERNEL_SIZE; i++)
        gaussian_kernel[i * stride + offset] = 0.0f;
    double_t temp_gaussian_kernel[GAUSSIAN_KERNEL_SIZE];
    double_t s = radius ;
    s = -1.0 / (2.0 * s * s);
    double_t sum = 0.5;
    temp_gaussian_kernel[0] = 1.0;
    for (size_t i = 1; i < GAUSSIAN_KERNEL_SIZE; i++)
        sum += temp_gaussian_kernel[i] = exp(i * i * s);

    sum = 0.5 / sum;
    for (size_t i = 0; i < GAUSSIAN_KERNEL_SIZE; i++)
        gaussian_kernel[i * stride + offset] = (float_t)(temp_gaussian_kernel[i] * sum);
}

static void tone_map_calculate_data(tone_map* tm) {
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
    if (tm->scene_fade_blend_func == 1 || tm->scene_fade_blend_func == 2)
        vec3_mult_scalar(*(vec3*)&v->p_fade_color, v->p_fade_color.w, *(vec3*)&v->p_fade_color);
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
    int32_t saturate_power;
    float_t saturate_coeff;

    tm->update_tex = true;
    vec2* tex = tm->tex;
    float_t gamma_power, gamma, saturation;
    gamma_power = tm->gamma * tm->gamma_rate * 1.5f;
    saturate_power = tm->saturate_power;
    saturate_coeff = tm->saturate_coeff;

    tex[0].x = 0.0f;
    tex[0].y = 0.0f;
    for (i = 1; i < tone_map_size; i++) {
        gamma = powf(1.0f - expf(-i * tone_map_scale), gamma_power);
        saturation = gamma * 2.0f - 1.0f;
        for (j = 0; j < saturate_power; j++) {
            saturation *= saturation;
            saturation *= saturation;
            saturation *= saturation;
            saturation *= saturation;
        }

        tex[i].x = gamma;
        tex[i].y = gamma * saturate_coeff
            * ((float_t)TONE_MAP_SAT_GAMMA_SAMPLES / (float_t)i) * (1.0f - saturation);
    }
}

#include "render_texture.h"
#include "texture.h"
#include "post_process/dof.h"

typedef struct struc_187 {
    texture* field_0;
    render_texture field_8;
    int field_38;
} struc_187;

typedef struct struc_186 {
    struc_187 field_0[4];
    char field_100;
} struc_186;

typedef struct struc_188 {
    vec4 field_0[8];
    float_t field_80;
    GLuint field_84[3];
    GLuint field_90[3];
} struc_188;

typedef struct struc_198 {
    fbo_struct fbo;
    GLuint program;
    GLuint sampler;
    GLuint vao;
} struc_198;

typedef struct post_process_struct {
    int field_0;
    int field_4;
    int ssaa;
    int taa;
    int mlaa;
    int field_14;
    int field_18;
    int field_1C;
    int field_20;
    int field_24;
    render_texture field_28[5];
    render_texture field_118[3];
    texture* field_1A8[3];
    render_texture field_1C0;
    int* field_1F0;
    render_texture field_1F8;
    render_texture field_228[5];
    texture* field_318[5];
    render_texture field_340;
    int field_370;
    int field_374;
    render_texture field_378;
    GLuint field_3A8;
    int field_3AC;
    texture* exposure_history;
    render_texture field_3B8;
    render_texture field_3E8;
    GLuint tonemap_lut_texture;
    int field_41C;
    render_texture field_420[2];
    GLuint field_480;
    int field_484;
    render_texture* field_488;
    int field_490;
    GLuint field_494[3];
    GLuint field_4A0[3];
    int field_4AC[3];
    int field_4B8[3];
    int field_4C4;
    GLuint lens_flare_texture;
    GLuint lens_shaft_texture;
    int lens_ghost_texture;
    int lens_flare_count;
    int width;
    int height;
    int render_width;
    int render_height;
    int field_4E8[5];
    int field_4FC[5];
    int field_510[5];
    int field_524[5];
    float_t field_538;
    float_t field_53C;
    int32_t reduce_width[5];
    int32_t reduce_height[5];
    int taa_texture_selector;
    int taa_texture;
    float taa_blend;
    vec3 view_point;
    vec3 interest;
    vec3 view_point_prev;
    vec3 interest_prev;
    mat4 field_5A4;
    mat4 field_5E4;
    int field_624;
    int field_628;
    char fast_change_happened;
    char field_62D;
    char field_62E;
    char field_62F;
    int screen_x_offset;
    int screen_y_offset;
    int screen_width;
    int screen_height;
    int update_lut;
    int field_644;
    struc_188 field_648[6];
    int exposure_history_counter;
    int field_9F4;
    vec3 lens_flare_pos;
    float lens_shaft_scale;
    float_t lens_shaft_inv_scale;
    float field_A0C;
    float field_A10;
    float field_A14;
    int16_t* field_A18[15];
    int field_A90;
    int field_A94;
    render_texture field_A98[16];
    __int64 field_D98;
    render_texture field_DA0[1];
    int field_DD0;
    int field_DD4;
    post_process_dof* dof;
    texture* field_DE0;
    struc_198* field_DE8;
    int32_t saturate_select;
    int32_t scene_fade_select;
    int32_t tone_trans_select;
    float saturate_coeff[2];
    float scene_fade_color[6];
    float scene_fade_alpha[2];
    int32_t scene_fade_blend_func[2];
    float tone_trans_scale[6];
    float tone_trans_offset[6];
    float tone_trans_start[6];
    float tone_trans_end[6];
    int tone_map;
    float_t exposure;
    float_t exposure_rate;
    int32_t auto_exposure;
    float_t gamma;
    float_t gamma_rate;
    int32_t saturate_power;
    int32_t mag_filter;
    float_t fade_alpha;
    int field_EB0;
    float_t lens_flare;
    float_t lens_shaft;
    float_t lens_ghost;
    vec3 radius;
    vec3 intensity;
    int field_ED8;
    int field_EDC;
    struc_186 field_EE0[6];
} post_process_struct;
