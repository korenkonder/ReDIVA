/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/light_param/light.hpp"
#include "../../KKdLib/vec.h"
#include "../static_var.h"
#include "face.h"

struct light_data {
    light_type type;
    vec4u ambient;
    vec4u diffuse;
    vec4u specular;
    vec4u position;
    vec3 spot_direction;
    float_t spot_exponent;
    float_t spot_cutoff;
    float_t constant;
    float_t linear;
    float_t quadratic;
    vec4u ibl_specular;
    vec4u ibl_back;
    vec4u ibl_direction;
    vec3 tone_curve;
    bool clip_plane[4];
};

struct light_set {
    light_data lights[LIGHT_MAX];
    vec4u ambient_intensity;
    mat4u irradiance_r;
    mat4u irradiance_g;
    mat4u irradiance_b;
};

extern void light_set_init(light_set* set);
extern light_type light_get_type(light_data* light);
extern void light_set_type(light_data* light, light_type value);
extern void light_get_ambient(light_data* light, vec4* value);
extern void light_set_ambient(light_data* light, const vec4* value);
extern void light_get_diffuse(light_data* light, vec4* value);
extern void light_set_diffuse(light_data* light, const vec4* value);
extern void light_get_specular(light_data* light, vec4* value);
extern void light_set_specular(light_data* light, const vec4* value);
extern void light_get_position(light_data* light, vec3* value);
extern void light_set_position(light_data* light, const vec3* value);
extern void light_get_position_vec4(light_data* light, vec4* value);
extern void light_set_position_vec4(light_data* light, const vec4* value);
extern void light_get_spot_direction(light_data* light, vec3* value);
extern void light_set_spot_direction(light_data* light, const vec3* value);
extern float_t light_get_spot_exponent(light_data* light);
extern void light_set_spot_exponent(light_data* light, float_t value);
extern float_t light_get_spot_cutoff(light_data* light);
extern void light_set_spot_cutoff(light_data* light, float_t value);
extern float_t light_get_constant(light_data* light);
extern void light_set_constant(light_data* light, float_t value);
extern float_t light_get_linear(light_data* light);
extern void light_set_linear(light_data* light, float_t value);
extern float_t light_get_quadratic(light_data* light);
extern void light_set_quadratic(light_data* light, float_t value);
extern void light_get_attenuation(light_data* light, vec3* value);
extern void light_set_attenuation(light_data* light, const vec3* value);
extern void light_get_ibl_specular(light_data* light, vec4* value);
extern void light_set_ibl_specular(light_data* light, const vec4* value);
extern void light_get_ibl_back(light_data* light, vec4* value);
extern void light_set_ibl_back(light_data* light, const vec4* value);
extern void light_get_ibl_direction(light_data* light, vec4* value);
extern void light_set_ibl_direction(light_data* light, const  vec4* value);
extern void light_get_tone_curve(light_data* light, vec3* value);
extern void light_set_tone_curve(light_data* light, const vec3* value);
extern void light_get_clip_plane(light_data* light, bool value[4]);
extern void light_set_clip_plane(light_data* light, const bool value[4]);
extern void light_set_get_ambient_intensity(light_set* set, vec4* value);
extern void light_set_set_ambient_intensity(light_set* set, const vec4* value);
extern void light_set_get_irradiance(light_set* set, mat4* r, mat4* g, mat4* b);
extern void light_set_set_irradiance(light_set* set, const mat4* r, const mat4* g, const mat4* b);
extern void light_set_data_set(light_set* set, face* face, light_set_id id);
