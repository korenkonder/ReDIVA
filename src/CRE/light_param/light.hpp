/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/light_param/light.hpp"
#include "../../KKdLib/vec.hpp"
#include "../static_var.hpp"
#include "face.hpp"

struct light_data {
    light_type type;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec3 spot_direction;
    float_t spot_exponent;
    float_t spot_cutoff;
    light_attenuation attenuation;
    vec4 ibl_specular;
    vec4 ibl_back;
    vec4 ibl_direction;
    light_tone_curve tone_curve;
    light_clip_plane clip_plane;

    light_data();

    light_type get_type();
    void set_type(light_type value);
    void get_ambient(vec4& value);
    void set_ambient(const vec4& value);
    void set_ambient(const vec4&& value);
    void set_ambient(const vec4& value, bool set[4]);
    void set_ambient(const vec4&& value, bool set[4]);
    void get_diffuse(vec4& value);
    void set_diffuse(const vec4& value);
    void set_diffuse(const vec4&& value);
    void set_diffuse(const vec4& value, bool set[4]);
    void set_diffuse(const vec4&& value, bool set[4]);
    void get_specular(vec4& value);
    void set_specular(const vec4& value);
    void set_specular(const vec4&& value);
    void set_specular(const vec4& value, bool set[4]);
    void set_specular(const vec4&& value, bool set[4]);
    void get_position(vec3& value);
    void set_position(const vec3& value);
    void set_position(const vec3&& value);
    void get_position(vec4& value);
    void set_position(const vec4& value);
    void set_position(const vec4&& value);
    void get_spot_direction(vec3& value);
    void set_spot_direction(const vec3& value);
    void set_spot_direction(const vec3&& value);
    float_t get_spot_exponent();
    void set_spot_exponent(float_t value);
    float_t get_spot_cutoff();
    void set_spot_cutoff(float_t value);
    float_t get_constant();
    void set_constant(float_t value);
    float_t get_linear();
    void set_linear(float_t value);
    float_t get_quadratic();
    void set_quadratic(float_t value);
    void get_attenuation(light_attenuation& value);
    void set_attenuation(const light_attenuation& value);
    void set_attenuation(const light_attenuation&& value);
    void get_ibl_specular(vec4& value);
    void set_ibl_specular(const vec4& value);
    void set_ibl_specular(const vec4&& value);
    void get_ibl_back(vec4& value);
    void set_ibl_back(const vec4& value);
    void set_ibl_back(const vec4&& value);
    void get_ibl_direction(vec4& value);
    void set_ibl_direction(const vec4& value);
    void set_ibl_direction(const vec4&& value);
    void get_tone_curve(light_tone_curve& value);
    void set_tone_curve(const light_tone_curve& value);
    void set_tone_curve(const light_tone_curve&& value);
    void get_clip_plane(light_clip_plane& value);
    void set_clip_plane(const light_clip_plane& value);
    void set_clip_plane(const light_clip_plane&& value);
};

struct light_set {
    light_data lights[LIGHT_MAX];
    vec4 ambient_intensity;
    mat4 irradiance_r;
    mat4 irradiance_g;
    mat4 irradiance_b;

    light_set();

    void get_ambient_intensity(vec4& value);
    void set_ambient_intensity(const vec4& value);
    void set_ambient_intensity(const vec4&& value);
    void get_irradiance(mat4& r, mat4& g, mat4& b);
    void set_irradiance(const mat4& r, const mat4& g, const mat4& b);
    void set_irradiance(const mat4&& r, const mat4&& g, const mat4&& b);
    void data_set(face& face, light_set_id id);
};
