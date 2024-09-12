/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../shader_ft.hpp"
#include "../render_context.hpp"

fog::fog() {
    type = FOG_NONE;
    density = 0.0f;
    start = 20.0f;
    end = 1000.0f;
    index = 0;
    color = { 1.0f, 1.0f, 1.0f, 0.0f };
}

fog_type fog::get_type() const {
    return type;
}

void fog::set_type(fog_type value) {
    type = value;
}

float_t fog::get_density() const {
    return density;
}

void fog::set_density(float_t value) {
    density = value;
}

float_t fog::get_start() const {
    return start;
}

void fog::set_start(float_t value) {
    start = value;
}

float_t fog::get_end() const {
    return end;
}

void fog::set_end(float_t value) {
    end = value;
}

int32_t fog::get_index() const {
    return index;
}

void fog::set_index(int32_t value) {
    index = value;
}

void fog::get_color(vec4& value) const {
    value = color;
}

void fog::set_color(const vec4& value) {
    color = value;
}

void fog::data_set(fog_id id) {
    fog_type type = get_type();
    if (type == FOG_NONE)
        return;

    extern render_context* rctx_ptr;

    switch (id) {
    case FOG_DEPTH: {
        float_t density = get_density();
        float_t start = get_start();
        float_t end = get_end();
        if (start >= end)
            start = end - 0.01f;
        vec4 color;
        get_color(color);

        render_context::fog_params params = {};
        rctx_ptr->get_scene_fog_params(params);
        params.density = density;
        params.start = start;
        params.end = end;
        params.depth_color = color;
        rctx_ptr->set_scene_fog_params(params);
    } break;
    case FOG_HEIGHT: {
        float_t density = get_density();
        float_t start = get_start();
        float_t end = get_end();
        if (start >= end)
            start = end - 0.01f;
        vec4 color;
        get_color(color);

        render_context::fog_params params = {};
        rctx_ptr->get_scene_fog_params(params);
        params.height_params.x = density;
        params.height_params.y = start;
        params.height_params.z = end;
        params.height_params.w = 1.0f / (end - start);
        params.height_color = color;
        rctx_ptr->set_scene_fog_params(params);
    } break;
    case FOG_BUMP: {
        float_t density = get_density();
        float_t start = get_start();
        float_t end = get_end();
        if (start >= end)
            start = end - 0.01f;

        render_context::fog_params params = {};
        rctx_ptr->get_scene_fog_params(params);
        params.bump_params.x = density;
        params.bump_params.y = start;
        params.bump_params.z = end;
        params.bump_params.w = 1.0f / (end - start);
        rctx_ptr->set_scene_fog_params(params);
    } break;
    }
}
