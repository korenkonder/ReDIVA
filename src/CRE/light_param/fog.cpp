/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../shader_ft.hpp"

fog::fog() {
    type = FOG_NONE;
    density = 0.0f;
    start = 20.0f;
    end = 1000.0f;
    index = 0;
    color = { 1.0f, 1.0f, 1.0f, 0.0f };
}

fog_type fog::get_type() {
    return type;
}

void fog::set_type(fog_type value) {
    type = value;
}

float_t fog::get_density() {
    return density;
}

void fog::set_density(float_t value) {
    density = value;
}

float_t fog::get_start() {
    return start;
}

void fog::set_start(float_t value) {
    start = value;
}

float_t fog::get_end() {
    return end;
}

void fog::set_end(float_t value) {
    end = value;
}

int32_t fog::get_index() {
    return index;
}

void fog::set_index(int32_t value) {
    index = value;
}

void fog::get_color(vec4& value) {
    value = color;
}

void fog::set_color(vec4& value) {
    color = value;
}

void fog::set_color(vec4&& value) {
    color = value;
}

void fog::data_set(fog_id id) {
    fog_type type = get_type();
    if (type == FOG_NONE)
        return;

    float_t density = get_density();
    float_t start = get_start();
    float_t end = get_end();
    if (start >= end)
        start = end - 0.01f;

    vec4 params;
    params.x = density;
    params.y = start;
    params.z = end;
    params.w = 1.0f / (end - start);

    switch (id) {
    case FOG_DEPTH: {
        vec4 color;
        get_color(color);
        shaders_ft.env_vert_set(29, color);
        shaders_ft.env_frag_set(4, color);

        shaders_ft.state_fog_set_color(color * (float_t)(1.0 / 8.0));
        shaders_ft.state_fog_set_params(params);
    } break;
    case FOG_HEIGHT: {
        vec4 color;
        get_color(color);
        shaders_ft.env_vert_set(30, color);
        shaders_ft.env_frag_set(11, color);
        shaders_ft.env_vert_set(14, params);
    } break;
    case FOG_BUMP: {
        shaders_ft.env_vert_set(19, params);
    } break;
    }
}
